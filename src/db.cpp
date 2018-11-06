#include <iostream>
#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <util/hex.hpp>
#include <util/nanoutil.hpp>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <sqlite3.h>

std::error_code nanocap::db::destroy_capture_data()
{
	std::error_code ec;
	sqlite->exec("DELETE FROM packet");
	sqlite->exec("DELETE FROM keepalive_host");
	sqlite->exec("DELETE FROM vote");
	sqlite->exec("DELETE FROM block_state");
	sqlite->exec("DELETE FROM block_send");
	sqlite->exec("DELETE FROM block_receive");
	sqlite->exec("DELETE FROM block_open");
	sqlite->exec("DELETE FROM block_change");
	
	return ec;
}

nanocap::db::db(nanocap::app & app) : app (app)
{
	std::cout << "Using database " << app.get_config().db << " (sqlite version " << SQLite::getLibVersion() << ")" << std::endl << std::endl;
	sqlite = std::make_unique<SQLite::Database> (app.get_config().db, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

	// Fast journal mode at the cost of risking corruption on OS crash/power outage.
	sqlite->exec("PRAGMA journal_mode = WAL");

	// Maintaining an atomic int initialized at startup from a dispenser table is much faster than autoincrement.
	sqlite->exec("CREATE TABLE IF NOT EXISTS runs (id_multiplier INTEGER PRIMARY KEY AUTOINCREMENT, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS packet "
				 "("
				 	"id INTEGER NOT NULL, ipv INTEGER NOT NULL, msg_type INTEGER NOT NULL, version_using INTEGER, version_min INTEGER, block_type INTEGER, extensions INTEGER, "
				 	"content_table TEXT DEFAULT NULL, content_id INTEGER DEFAULT NULL, srcip TEXT DEFAULT NULL, srcport INTEGER DEFAULT NULL, dstip TEXT DEFAULT NULL, dstport INTEGER DEFAULT NULL, "
				 	"time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, time_usec INTEGER DEFAULT 0"
				 ")"
				 );
	
	// Ensure packet-by-type queries are fast
	sqlite->exec("CREATE INDEX IF NOT EXISTS idx_msg_type ON packet (msg_type)");

	// Hosts enlisted in a keepalive packet
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS keepalive_host "
				 "(id INTEGER NOT NULL, packet_id INTEGER, address TEXT NOT NULL, port INTEGER NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
				 );
	
	// Table with a comma-separated list of vote-by-hash hahes for a given packet
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS vote "
				 "(id INTEGER NOT NULL, packet_id INTEGER NOT NULL, content_table TEXT DEFAULT NULL, content_id INTEGER DEFAULT NULL, vote_count INTEGER DEFAULT 0, vbh INTEGER, account TEXT, signature TEXT, sequence INTEGER, hashes TEXT)"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_state "
				 "( "
				 	"id INTEGER NOT NULL, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, account TEXT, previous TEXT, representative TEXT, balance TEXT, link TEXT, signature TEXT, work INTEGER"
				 ")"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_send "
				 "( "
				 	"id INTEGER NOT NULL, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, destination TEXT, balance TEXT, signature TEXT, work INTEGER"
				 ")"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_receive "
				 "( "
				 	"id INTEGER NOT NULL, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, source TEXT, signature TEXT, work INTEGER"
				 ")"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_open "
				 "( "
				 	"id INTEGER NOT NULL, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, source TEXT, representative TEXT, account TEXT, signature TEXT, work INTEGER"
				 ")"
				 );
	
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_change "
				 "( "
				 	"id INTEGER NOT NULL, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, representative TEXT, signature TEXT, work INTEGER"
				 ")"
				 );
				 
	// Insert a new run. This increments the id multiplier.
	assert (sqlite->exec("INSERT INTO runs DEFAULT VALUES") == 1);

	// Each runs gives a budget of one billion id's.
	next_id = 1'000'000'000 * sqlite->execAndGet("SELECT MAX(id_multiplier) FROM runs").getInt64();

	stmt_packet = std::make_unique<SQLite::Statement>(*sqlite,
													  "INSERT INTO packet VALUES (:id, :ipv, :hdr_msg_type, :hdr_version_using, :hdr_version_min, :hdr_block_type, :hdr_extensions, :content_table, :content_id, :srcip, :srcport, :dstip, :dstport, :time, :time_usec)");
	stmt_vote = std::make_unique<SQLite::Statement>(*sqlite,
													"INSERT INTO vote VALUES (:id, :packet_id, :content_table, :content_id, :vote_count, :vbh, :account, :signature, :sequence, :hashes)");
	stmt_block_state = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_state VALUES (:id, :packet_id, :hash, :account, :previous, :representative, :balance, :link, :signature, :work)");
	stmt_block_send = std::make_unique<SQLite::Statement>(*sqlite,
														  "INSERT INTO block_send VALUES (:id, :packet_id, :hash, :previous, :destination, :balance, :signature, :work)");
	stmt_block_receive = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_receive VALUES (:id, :packet_id, :hash, :previous, :source, :signature, :work)");
	stmt_block_open = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_open VALUES (:id, :packet_id, :hash, :source, :representative, :account, :signature, :work)");
	stmt_block_change = std::make_unique<SQLite::Statement>(*sqlite,
														   "INSERT INTO block_change VALUES (:id, :packet_id, :hash, :previous, :representative, :signature, :work)");

	stmt_host = std::make_unique<SQLite::Statement>(*sqlite,
													"INSERT INTO keepalive_host VALUES (:id, :packet_id, :address, :port, CURRENT_TIMESTAMP)");

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
		int max_rows = app.get_config().web.max_query_result;
		
		json rows = json::array();
		for (int i=0; i < max_rows && stmt.executeStep(); i++)
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
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// TODO: only if configured to save peers
	for (auto peer : *msg.peers ())
	{
		// std::string#data() is contiguous, so we simply cast it to std::array (which boost expects) to save a copy.
		std::array<uint8_t, 16>* arr = reinterpret_cast<std::array<uint8_t, 16>*> ((void*)peer->address().data());
		boost::asio::ip::udp::endpoint endpoint (boost::asio::ip::address_v6 (*arr), peer->port ());
		
		stmt_host->bind(":id", next_id.fetch_add(1));
		stmt_host->bind(":packet_id", id);
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
	int64_t packet_id = next_id.fetch_add(1);

	std::lock_guard<std::mutex> guard (db_mutex);
	
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_udp_fields(stmt_packet.get(), info);
	
	// Regular vote
	if (msg.block())
	{
		int64_t block_content_id = next_id.fetch_add(1);
		std::string content_table;
		put_block(msg.block(), block_content_id, packet_id, content_table);
		
		stmt_vote->bind(":content_id", block_content_id);
		stmt_vote->bind(":content_table", content_table);
		stmt_vote->bind(":vote_count", 1);
		stmt_vote->bind(":vbh", 0);
	}
	// Vote-by-hash
	else if (msg.votebyhash() && msg.votebyhash()->hashes() && !msg.votebyhash()->hashes()->empty())
	{
		stmt_vote->bind(":vbh", 1);
		std::ostringstream hashes;
		
		auto size = msg.votebyhash()->hashes()->size();
		stmt_vote->bind(":vote_count", static_cast<int64_t>(size));
		size_t count = 0;
		for (auto& hash : *msg.votebyhash()->hashes())
		{
			hashes << nanocap::to_hex(hash);
			if (++count < size)
			{
				hashes << ",";
			}
		}

		stmt_vote->bind(":hashes", hashes.str());
	}
	else
	{
		// Shouldn't happen unless the packet is malformed
		stmt_vote->bind(":vbh", 2);
		stmt_vote->bind(":vote_count", 0);
	}
	
	// Common fields
	if (msg.common())
	{
		stmt_vote->bind(":account", to_hex(msg.common()->account()));
		stmt_vote->bind(":signature", to_hex(msg.common()->signature()));
		stmt_vote->bind(":sequence", static_cast<int64_t>(msg.common()->sequence()));
	}
	else
	{
		stmt_vote->bind(":account", nullptr);
		stmt_vote->bind(":signature", nullptr);
		stmt_vote->bind(":sequence", nullptr);
	}

	int64_t vote_content_id = next_id.fetch_add(1);
	stmt_vote->bind(":id", vote_content_id);
	stmt_vote->bind(":packet_id", packet_id);
	stmt_vote->exec();
	stmt_vote->reset();

	stmt_packet->bind(":content_id", vote_content_id);
	stmt_packet->bind(":content_table", "vote");

	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_req_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_udp_fields(stmt_packet.get(), info);
	int64_t content_id = next_id.fetch_add(1);
	std::string content_table;
	put_block(msg.body(), content_id, packet_id, content_table);

	stmt_packet->bind(":content_id", content_id);
	stmt_packet->bind(":content_table", content_table);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_node_id_handshake_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_udp_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put_block(nano::protocol::nano_t::block_selector_t* block_selector,
									   int64_t id, int64_t packet_id, std::string& content_table)
{
	std::error_code ec;
	if (app.get_config().capture.block_details && block_selector && block_selector->block())
	{
		switch (block_selector->arg_block_type())
		{
			case nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_STATE:
			{
				//stmt_packet->bind(":content_table", "block_receive");
				content_table = "block_state";
				auto block = static_cast<nano::protocol::nano_t::block_state_t*>(block_selector->block());
				stmt_block_state->bind(":id", id);
				stmt_block_state->bind(":packet_id", packet_id);
				stmt_block_state->bind(":hash", to_hex(hash_of(block)));
				stmt_block_state->bind(":account", to_hex(block->account()));
				stmt_block_state->bind(":previous", to_hex(block->previous()));
				stmt_block_state->bind(":representative", to_hex(block->representative()));
				stmt_block_state->bind(":balance", to_hex(block->balance()));
				stmt_block_state->bind(":link", to_hex(block->link()));
				stmt_block_state->bind(":signature", to_hex(block->signature()));
				stmt_block_state->bind(":work", static_cast<int64_t>(block->work()));
				stmt_block_state->exec();
				stmt_block_state->reset();
				break;
			}
			case nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_SEND:
			{
				content_table = "block_send";
				auto block = static_cast<nano::protocol::nano_t::block_send_t*>(block_selector->block());
				stmt_block_send->bind(":id", id);
				stmt_block_send->bind(":packet_id", packet_id);
				stmt_block_send->bind(":hash", to_hex(hash_of(block)));
				stmt_block_send->bind(":previous", to_hex(block->previous()));
				stmt_block_send->bind(":destination", to_hex(block->destination()));
				stmt_block_send->bind(":balance", to_hex(block->balance()));
				stmt_block_send->bind(":signature", to_hex(block->signature()));
				stmt_block_send->bind(":work", static_cast<int64_t>(block->work()));
				stmt_block_send->exec();
				stmt_block_send->reset();
				break;
			}
			case nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_RECEIVE:
			{
				content_table = "block_receive";
				auto block = static_cast<nano::protocol::nano_t::block_receive_t*>(block_selector->block());
				stmt_block_receive->bind(":id", id);
				stmt_block_receive->bind(":packet_id", packet_id);
				stmt_block_receive->bind(":hash", to_hex(hash_of(block)));
				stmt_block_receive->bind(":previous", to_hex(block->previous()));
				stmt_block_receive->bind(":source", to_hex(block->source()));
				stmt_block_receive->bind(":signature", to_hex(block->signature()));
				stmt_block_receive->bind(":work", static_cast<int64_t>(block->work()));
				stmt_block_receive->exec();
				stmt_block_receive->reset();
				break;
			}
			case nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_OPEN:
			{
				content_table = "block_open";
				auto block = static_cast<nano::protocol::nano_t::block_open_t*>(block_selector->block());
				stmt_block_open->bind(":id", id);
				stmt_block_open->bind(":packet_id", packet_id);
				stmt_block_open->bind(":hash", to_hex(hash_of(block)));
				stmt_block_open->bind(":source", to_hex(block->source()));
				stmt_block_open->bind(":representative", to_hex(block->representative()));
				stmt_block_open->bind(":account", to_hex(block->account()));
				stmt_block_open->bind(":signature", to_hex(block->signature()));
				stmt_block_open->bind(":work", static_cast<int64_t>(block->work()));
				stmt_block_open->exec();
				stmt_block_open->reset();
				break;
			}
			case nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_CHANGE:
			{
				content_table = "block_change";
				auto block = static_cast<nano::protocol::nano_t::block_change_t*>(block_selector->block());
				stmt_block_change->bind(":id", id);
				stmt_block_change->bind(":packet_id", packet_id);
				stmt_block_change->bind(":hash", to_hex(hash_of(block)));
				stmt_block_change->bind(":previous", to_hex(block->previous()));
				stmt_block_change->bind(":representative", to_hex(block->representative()));
				stmt_block_change->bind(":signature", to_hex(block->signature()));
				stmt_block_change->bind(":work", static_cast<int64_t>(block->work()));
				stmt_block_change->exec();
				stmt_block_change->reset();
				break;
			}
		}
	}

	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_publish_t& msg, nanocap::udp_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	
	std::lock_guard<std::mutex> guard (db_mutex);
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_udp_fields(stmt_packet.get(), info);

	int64_t content_id = next_id.fetch_add(1);
	std::string content_table;
	put_block(msg.body(), content_id, packet_id, content_table);
	
	stmt_packet->bind(":content_id", content_id);
	stmt_packet->bind(":content_table", content_table);
	
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
