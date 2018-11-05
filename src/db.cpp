#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <sqlite3.h>

nanocap::db::db(nanocap::app & app) : app (app)
{
	std::cout << "Opening database: " << app.get_config().db << std::endl;
	sqlite = std::make_unique<SQLite::Database> (app.get_config().db, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
	std::cout << "Using sqlite3 " << SQLite::getLibVersion() << std::endl;
	
	//sqlite->exec("DROP TABLE IF EXISTS packet");
	//sqlite->exec("DROP TABLE IF EXISTS host");
	
	sqlite->exec("PRAGMA journal_mode = WAL");
	//sqlite->exec("PRAGMA synchronous = OFF");

	// Maintaining an atomic int initialized at startup from a dispenser table is much faster than autoincrement.
	sqlite->exec("CREATE TABLE IF NOT EXISTS runs (id_multiplier INTEGER PRIMARY KEY AUTOINCREMENT, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS packet "
				 "("
				 	"id INTEGER NOT NULL, ipv INTEGER NOT NULL, msg_type INTEGER NOT NULL, version_using INTEGER, version_min INTEGER, block_type INTEGER, "
				 	"block_id TEXT DEFAULT NULL, "
				 	"srcip TEXT DEFAULT NULL, srcport INTEGER DEFAULT NULL, dstip TEXT DEFAULT NULL, dstport INTEGER DEFAULT NULL, "
				 	"time TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
				 ")"
				 );
	
	// Ensure packet-by-type queries are fast
	sqlite->exec("CREATE INDEX IF NOT EXISTS idx_msg_type ON packet (msg_type)");
	
	// TODO: add detected version
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS keepalive_host "
				 "(id INTEGER NOT NULL, keepalive_id INTEGER, address TEXT NOT NULL, port INTEGER NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_state"
				 "( "
				 	"id INTEGER NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
				 	"account BLOB, previous BLOB, representative BLOB, balance BLOB, link BLOB, signature BLOB, work INTEGER"
				 ")"
				 );
				 
	// Insert a new run. This increments the id multiplier.
	assert (sqlite->exec("INSERT INTO runs DEFAULT VALUES") == 1);
	// Each runs gives a budget of one billion id's.
	next_id = 1'000'000'000 * sqlite->execAndGet("SELECT MAX(id_multiplier) FROM runs").getInt64();

	stmt_packet = std::make_unique<SQLite::Statement>(*sqlite,
													  "INSERT INTO packet VALUES (:id, :ipv, :hdr_msg_type, :hdr_version_using, :hdr_version_min, :hdr_block_type, :block_id, :srcip, :srcport, :dstip, :dstport, :time)");
	stmt_block_state = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_state VALUES (:id, :time, :account, :previous, :representative, :balance, :link, :signature, :work)");
	
	// Insert host. If seen before, update seen-count and timestamp.
	stmt_host = std::make_unique<SQLite::Statement>(*sqlite,
													"INSERT INTO keepalive_host VALUES (:id, :keepalive_id, :address, :port, CURRENT_TIMESTAMP)");

	stmt_packet_per_msg_type = std::make_unique<SQLite::Statement>(*sqlite,
																   "SELECT COUNT(*), msg_type FROM packet GROUP BY msg_type");
	
	// Start transaction; this will be periodically committed
	primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
}

nanocap::db::~db()
{
	primary_tx->commit();
}

std::error_code nanocap::db::flush()
{
	std::error_code ec;
	std::lock_guard<std::mutex> guard (db_mutex);
	primary_tx->commit();
	primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
	return ec;
}

int pragma_table_info_callback(void *ctx, int argc, char **argv, char **azColName)
{
	json* cols = static_cast<json*>(ctx);
	
	cols->push_back(json::object());
	json& col = cols->back();
	
	for (int i = 0; i < argc; i++)
	{
		col[azColName[i]] = argv[i] ? argv[i] : "NULL";
	}
	
	return 0;
}

json nanocap::db::get_schema()
{
	json schema = json::array();

	std::lock_guard<std::mutex> guard (db_mutex);
	SQLite::Statement query(*sqlite, "SELECT name FROM sqlite_master WHERE type='table'");
	
	while (query.executeStep())
	{
		const std::string table_name = query.getColumn("name");
		if (table_name != "sqlite_sequence")
		{
			schema.push_back(json::object());
			json& table = schema.back();

			table["table"] = table_name;
			table["columns"] = json::array();
			std::string pragma_stmt = "PRAGMA table_info(" + table_name + ");";
			
			char *err_msg = 0;
			
			// Pragmas doesn't work via the wrapper, so we'll do it the hard way
			auto rc = sqlite3_exec(sqlite->getHandle(), pragma_stmt.c_str(), pragma_table_info_callback, (void*)&table["columns"], &err_msg);
			if (rc != SQLITE_OK ) {
				
				fprintf(stderr, "Failed to select data\n");
				fprintf(stderr, "SQL error: %s\n", err_msg);
				
				sqlite3_free(err_msg);
			}
		}
	}

	return schema;
}

std::string nanocap::db::get_schema_json()
{
	auto schema = get_schema();
	std::stringstream ss;
	ss << schema.dump(4) << std::endl;
	return ss.str();
}

json nanocap::db::query(std::string query)
{
	std::lock_guard<std::mutex> guard (db_mutex);
	json res;
	try
	{
		SQLite::Statement stmt(*sqlite, query);
		// TODO: config
		int maxrows = 5000;
		
		json rows = json::array();
		for (int i=0; i < maxrows && stmt.executeStep(); i++)
		{
			if (i == 0)
			{
				json colnames = json::array();
				for (int i=0; i < stmt.getColumnCount(); i++)
				{
					auto col = stmt.getColumn(i);
					colnames.push_back(col.getName());
				}
				res["columns"] = colnames;
			}

			json column_vals = json::array();
			for (int i=0; i < stmt.getColumnCount(); i++)
			{
				auto col = stmt.getColumn(i);
				column_vals.push_back(col.getText());
			}
			rows.push_back(column_vals);
		}
		res["rows"] = rows;
	}
	catch (std::runtime_error& ex)
	{
		res["error"] = ex.what();
	}
	
	return res;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_keepalive_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, id);
	bind_udp_fields(stmt_packet.get(), info);
	stmt_packet->bind(":block_id", nullptr);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// TODO: only if configured to save peers
	for (auto peer : *msg.peers ())
	{
		// std::string#data() is contiguous, so we simply cast it to std::array (which boost expects) to save a copy.
		std::array<uint8_t, 16>* arr = reinterpret_cast<std::array<uint8_t, 16>*> ((void*)peer->address().data());
		boost::asio::ip::udp::endpoint endpoint (boost::asio::ip::address_v6 (*arr), peer->port ());
		
		stmt_host->bind(":id", next_id.fetch_add(1));
		stmt_host->bind(":keepalive_id", id);
		stmt_host->bind(":address", endpoint.address().to_string ());
		stmt_host->bind(":port", peer->port());
		stmt_host->exec();
		stmt_host->reset();
	}

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_ack_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	
	bind_header_fields(stmt_packet.get(), msg, id);
	bind_udp_fields(stmt_packet.get(), info);
	
	// Regular vote
	if (msg.block())
	{
		auto id = next_id.fetch_add(1);
		stmt_packet->bind(":block_id", id);
	}
	// Vote-by-hash
	else
	{
		stmt_packet->bind(":block_id", nullptr);
	}

	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_req_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	bind_header_fields(stmt_packet.get(), msg, id);
	bind_udp_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	
	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_publish_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	bind_header_fields(stmt_packet.get(), msg, id);
	bind_udp_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	
	// Prepare for next use
	stmt_packet->reset();

	return ec;
}

int64_t nanocap::db::count_packets()
{
	std::lock_guard<std::mutex> guard (db_mutex);
	return sqlite->execAndGet("select count(*) from packet").getInt64();
}

std::string nanocap::db::count_packets_per_type()
{
	std::lock_guard<std::mutex> guard (db_mutex);
	std::string res;
	try
	{
		json arr = json::array();
		while (stmt_packet_per_msg_type->executeStep())
		{
			arr.push_back(json::object());
			json& obj = arr.back();
			obj["count"] = stmt_packet_per_msg_type->getColumn(0).getInt64();
			obj["type"] = stmt_packet_per_msg_type->getColumn(1).getInt64();
		}
		res = arr.dump(5);
	}
	catch (std::runtime_error& ex)
	{
		res = ex.what();
	}
	
	stmt_packet_per_msg_type->reset();
	return res;
}
