#include <app.hpp>
#include <db.hpp>
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
				 	"id INTEGER NOT NULL, hdr_msg_type INTEGER NOT NULL, hdr_version_using INTEGER, hdr_version_min INTEGER, hdr_block_type, "
				 	"block_id TEXT DEFAULT NULL"
				 ")"
				 );
	
	// TODO: add detected version
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS host "
				 "(id INTEGER NOT NULL, keepalive_id INTEGER, address TEXT NOT NULL UNIQUE, port INTEGER NOT NULL, seen INTEGER DEFAULT 1, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_state"
				 "( "
				 	"id INTEGER NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
				 	"account BLOB, previous BLOB, representative BLOB, balance BLOB, link BLOB, signature BLOB, work INTEGER"
				 ")"
				 );
				 
	//int nb = sqlite->exec(R"(INSERT INTO packet VALUES (4, "test 4"))");
	
	// Insert a new run. This increments the id multiplier.
	assert (sqlite->exec("INSERT INTO runs DEFAULT VALUES") == 1);
	// Each runs gives a budget of one billion id's.
	next_id = 1'000'000'000 * sqlite->execAndGet("SELECT MAX(id_multiplier) FROM runs").getInt64();
	std::cout << "Next ID is " << next_id << std::endl;

	stmt_packet = std::make_unique<SQLite::Statement>(*sqlite,
													  "INSERT INTO packet VALUES (:id, :hdr_msg_type, :hdr_version_using, :hdr_version_min, :hdr_block_type, :block_id)");
	stmt_block_state = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_state VALUES (:id, :time, :account, :previous, :representative, :balance, :link, :signature, :work)");
	
	// Insert host. If seen before, update seen-count and timestamp.
	stmt_host = std::make_unique<SQLite::Statement>(*sqlite,
													"INSERT INTO host VALUES (:id, :keepalive_id, :address, :port, 1, CURRENT_TIMESTAMP) ON CONFLICT (address) DO UPDATE SET seen=seen+1, time=CURRENT_TIMESTAMP");

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
		schema.push_back(json::object());
		json& table = schema.back();

		const std::string table_name = query.getColumn("name");
		table["table"] = table_name;
		table["columns"] = json::array();
		std::string pragma_stmt = "PRAGMA table_info(" + table_name + ");";
		
		char *err_msg = 0;
		
		// Pragmas doesn't work via the SQLite wrapper, so we'll do it the hard way
		auto rc = sqlite3_exec(sqlite->getHandle(), pragma_stmt.c_str(), pragma_table_info_callback, (void*)&table["columns"], &err_msg);
		if (rc != SQLITE_OK ) {
			
			fprintf(stderr, "Failed to select data\n");
			fprintf(stderr, "SQL error: %s\n", err_msg);
			
			sqlite3_free(err_msg);
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

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_keepalive_t& msg)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);

	//SQLite::Transaction transaction (*sqlite);
	set_header(stmt_packet.get(), msg, id);
	stmt_packet->bind(":block_id", nullptr);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// TODO: only if configured to save peers, lots of overhead
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
	
	//transaction.commit();

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_ack_t& msg)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	
	set_header(stmt_packet.get(), msg, id);
	
	// Regular vote
	if (msg.block()){
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

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_req_t& msg)
{
	std::error_code ec;
	int64_t id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	set_header(stmt_packet.get(), msg, id);
	
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
