#include <iostream>
#include <app.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <util/hex.hpp>
#include <util/nanoutil.hpp>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <sqlite3.h>

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
				 	"id INTEGER PRIMARY KEY, flow_key INTEGER NOT NULL, ipv INTEGER NOT NULL, msg_type INTEGER NOT NULL, version_using INTEGER, version_min INTEGER, block_type INTEGER, extensions INTEGER, "
				 	"content_table TEXT DEFAULT NULL, content_id INTEGER DEFAULT NULL, srcip TEXT DEFAULT NULL, srcport INTEGER DEFAULT NULL, dstip TEXT DEFAULT NULL, dstport INTEGER DEFAULT NULL, "
				 	"time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, time_usec INTEGER DEFAULT 0"
				 ")"
				 );
	
	// Ensure packet-by-type queries are fast
	sqlite->exec("CREATE INDEX IF NOT EXISTS idx_msg_type ON packet (msg_type)");

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS connection "
				 "(id INTEGER PRIMARY KEY, ipv INTEGER NOT NULL, srcip TEXT NOT NULL, srcport INTEGER NOT NULL, dstip TEXT NOT NULL, dstport INTEGER NOT NULL, flowkey INTEGER NOT NULL, event TEXT NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
				 );

	// Hosts enlisted in a keepalive packet
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS keepalive_host "
				 "(id INTEGER PRIMARY KEY, packet_id INTEGER, address TEXT NOT NULL, port INTEGER NOT NULL, time TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"
				 );

	// Table with a comma-separated list of vote-by-hash hahes for a given packet
	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS vote "
				 "(id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, content_table TEXT DEFAULT NULL, content_id INTEGER DEFAULT NULL, vote_count INTEGER DEFAULT 0, vbh INTEGER, account TEXT, signature TEXT, sequence INTEGER, hashes TEXT)"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_state "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, account TEXT, previous TEXT, representative TEXT, balance TEXT, link TEXT, signature TEXT, work INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_send "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, destination TEXT, balance TEXT, signature TEXT, work INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_receive "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, source TEXT, signature TEXT, work INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_open "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, source TEXT, representative TEXT, account TEXT, signature TEXT, work INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS block_change "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"hash TEXT, previous TEXT, representative TEXT, signature TEXT, work INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS nodeid_query "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"cookie TEXT"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS nodeid_response "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"nodeid TEXT, nodeid_as_account TEXT, signature TEXT"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS telemetry "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"signature TEXT, nodeid TEXT, nodeid_as_account TEXT, blockcount INTEGER, cementedcount INTEGER, uncheckedcount INTEGER, accountcount INTEGER, bandwidthcap INTEGER, uptime INTEGER, peercount INTEGER, protocolversion INTEGER, genesisblock TEXT, majorversion INTEGER, minorversion INTEGER, patchversion INTEGER, prereleaseversion INTEGER, maker INTEGER, timestamp INTEGER, activedifficulty INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS frontier_request "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"start TEXT, age INTEGER, count INTEGER"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS frontier_response "
				 "( "
				 	"id INTEGER PRIMARY KEY, frontier_request_id NOT NULL, "
				 	"account TEXT, frontier_hash TEXT"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_pull_request "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"start TEXT, end TEXT, extended_count INTEGER DEFAULT 0"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_pull_response "
				 "( "
				 	"id INTEGER PRIMARY KEY, bulk_pull_request_id NOT NULL, "
				 	"content_id INTEGER NOT NULL, content_table TEXT NOT NULL"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_pull_account_request "
				 "( "
				 	"id INTEGER PRIMARY KEY, packet_id INTEGER NOT NULL, "
				 	"account TEXT, minimum_amount TEXT, flags INTEGER DEFAULT 0"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_pull_account_response "
				 "( "
				 	"id INTEGER PRIMARY KEY, bulk_pull_account_request_id NOT NULL, "
				 	"frontier_hash TEXT, balance TEXT"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_pull_account_response_entry "
				 "( "
				 	"id INTEGER PRIMARY KEY, bulk_pull_account_request_id NOT NULL, "
				 	"hash TEXT, amount TEXT, source TEXT"
				 ")"
				 );

	sqlite->exec(
				 "CREATE TABLE IF NOT EXISTS bulk_push_entry "
				 "( "
					"id INTEGER PRIMARY KEY, packet_id NOT NULL, "
					"content_id INTEGER NOT NULL, content_table TEXT NOT NULL"
				 ")"
				 );

	// Insert a new run. This increments the id multiplier.
	sqlite->exec("INSERT INTO runs DEFAULT VALUES");

	// Each runs gives a budget of one billion id's.
	next_id = 1'000'000'000 * sqlite->execAndGet("SELECT MAX(id_multiplier) FROM runs").getInt64();

	stmt_connection = std::make_unique<SQLite::Statement>(*sqlite,
														 "INSERT INTO connection VALUES (:id, :ipv, :srcip, :srcport, :dstip, :dstport, :flowkey, :event, CURRENT_TIMESTAMP)");
	stmt_packet = std::make_unique<SQLite::Statement>(*sqlite,
													  "INSERT INTO packet VALUES (:id, :flow_key, :ipv, :hdr_msg_type, :hdr_version_using, :hdr_version_min, :hdr_block_type, :hdr_extensions, :content_table, :content_id, :srcip, :srcport, :dstip, :dstport, :time, :time_usec)");
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
	
	stmt_nodeid_query = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO nodeid_query VALUES (:id, :packet_id, :cookie)");
	stmt_nodeid_response = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO nodeid_response VALUES (:id, :packet_id, :nodeid, :nodeid_as_account, :signature)");
	
	stmt_telemetry = std::make_unique<SQLite::Statement>(*sqlite,
														 "INSERT INTO telemetry VALUES (:id, :packet_id, :signature, :nodeid, :nodeid_as_account, :blockcount, :cementedcount, :uncheckedcount, :accountcount, :bandwidthcap, :uptime, :peercount, :protocolversion, :genesisblock, :majorversion, :minorversion, :patchversion, :prereleaseversion, :maker, :timestamp, :activedifficulty)");
	
	stmt_frontier_request = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO frontier_request VALUES (:id, :packet_id, :start, :age, :count)");
	stmt_frontier_response = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO frontier_response VALUES (:id, :frontier_request_id, :account, :frontier_hash)");

	stmt_bulk_pull_request = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO bulk_pull_request VALUES (:id, :packet_id, :start, :end, :extended_count)");
	stmt_bulk_pull_response = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO bulk_pull_response VALUES (:id, :bulk_pull_request_id, :content_id, :content_table)");

	stmt_bulk_pull_account_request = std::make_unique<SQLite::Statement>(*sqlite,
																		 "INSERT INTO bulk_pull_account_request VALUES (:id, :packet_id, :account, :minimum_amount, :flags)");
	stmt_bulk_pull_account_response = std::make_unique<SQLite::Statement>(*sqlite,
																		  "INSERT INTO bulk_pull_account_response VALUES (:id, :bulk_pull_account_request_id, :frontier_hash, :balance)");
	stmt_bulk_pull_account_response_entry = std::make_unique<SQLite::Statement>(*sqlite,
																				"INSERT INTO bulk_pull_account_response_entry VALUES (:id, :bulk_pull_account_request_id, :hash, :amount, :source)");

	stmt_bulk_push_entry = std::make_unique<SQLite::Statement>(*sqlite, "INSERT INTO bulk_push_entry VALUES (:id, :packet_id, :content_id, :content_table)");

	// Start transaction; this will be periodically committed
	primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);

	init_packets_per_type_from_database();
}

nanocap::db::~db()
{
	std::lock_guard<std::mutex> guard (db_mutex);
	primary_tx->commit();
}

std::error_code nanocap::db::flush()
{
	std::lock_guard<std::mutex> guard (db_mutex);
	return flush_internal();
}

std::error_code nanocap::db::flush_internal()
{
	std::error_code ec;
	primary_tx->commit();
	primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
	return ec;
}


std::error_code nanocap::db::destroy_capture_data()
{
	std::error_code ec;
	
	std::lock_guard<std::mutex> guard (db_mutex);
	SQLite::Statement query(*sqlite, "SELECT name FROM sqlite_master WHERE type='table'");
	
	while (query.executeStep())
	{
		const std::string table_name = query.getColumn("name");
		if (table_name != "sqlite_sequence")
		{
			std::cout << "Deleting table " << table_name << std::endl;
			sqlite->exec("DELETE FROM " + table_name);
			flush_internal();
		}
	}
	
	sqlite->exec("INSERT INTO runs DEFAULT VALUES");
	flush_internal();

	count_per_type = std::array<size_t, 32>{};

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

std::error_code nanocap::db::put_connection(int ipVersion, std::string srcip, uint16_t srcport, std::string dstip, uint16_t dstport, uint32_t flow_key, std::string event)
{
	std::error_code ec;
	std::lock_guard<std::mutex> guard (db_mutex);

	stmt_connection->bind(":id", next_id.fetch_add(1));
	stmt_connection->bind(":ipv", ipVersion);
	stmt_connection->bind(":srcip", srcip);
	stmt_connection->bind(":srcport", srcport);
	stmt_connection->bind(":dstip", dstip);
	stmt_connection->bind(":dstport", dstport);
	stmt_connection->bind(":flowkey", flow_key);
	stmt_connection->bind(":event", event);
	stmt_connection->exec();
	stmt_connection->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_telemetry_req_t& msg, nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);
	
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Telemetry requests do not have any data beyond the header
	
	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_telemetry_ack_t& msg, nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);
	
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	stmt_telemetry->bind(":id", next_id.fetch_add(1));
	stmt_telemetry->bind(":packet_id", packet_id);
	
	stmt_telemetry->bind(":signature", nanocap::to_hex(msg.signature()));
	stmt_telemetry->bind(":nodeid", nanocap::to_hex(msg.nodeid()));
	stmt_telemetry->bind(":nodeid_as_account", pub_to_account(msg.nodeid()));
	stmt_telemetry->bind(":blockcount", static_cast<int64_t>(msg.blockcount()));
	stmt_telemetry->bind(":cementedcount", static_cast<int64_t>(msg.cementedcount()));
	stmt_telemetry->bind(":uncheckedcount", static_cast<int64_t>(msg.uncheckedcount()));
	stmt_telemetry->bind(":accountcount", static_cast<int64_t>(msg.accountcount()));
	stmt_telemetry->bind(":bandwidthcap", static_cast<int64_t>(msg.bandwidthcap()));
	stmt_telemetry->bind(":uptime", static_cast<int64_t>(msg.uptime()));
	stmt_telemetry->bind(":peercount", msg.peercount());
	stmt_telemetry->bind(":protocolversion", msg.protocolversion());
	stmt_telemetry->bind(":genesisblock", nanocap::to_hex(msg.genesisblock()));
	stmt_telemetry->bind(":majorversion", msg.majorversion());
	stmt_telemetry->bind(":minorversion", msg.minorversion());
	stmt_telemetry->bind(":patchversion", msg.patchversion());
	stmt_telemetry->bind(":prereleaseversion", msg.prereleaseversion());
	stmt_telemetry->bind(":maker", msg.maker());
	stmt_telemetry->bind(":timestamp", static_cast<int64_t>(msg.timestamp()));
	stmt_telemetry->bind(":activedifficulty", static_cast<int64_t>(msg.activedifficulty()));
	
	stmt_telemetry->exec();
	stmt_telemetry->reset();
	
	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_frontier_req_t& msg, nano_packet& info)
{
	std::error_code ec;
	info.flow_data->packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, info.flow_data->packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	
	info.flow_data->message_id = next_id.fetch_add(1);
	stmt_frontier_request->bind(":id", info.flow_data->message_id);
	stmt_frontier_request->bind(":packet_id", info.flow_data->packet_id);
	stmt_frontier_request->bind(":start", msg.start());
	stmt_frontier_request->bind(":age", msg.age());
	stmt_frontier_request->bind(":count", msg.count());
	stmt_frontier_request->exec();
	stmt_frontier_request->reset();
	
	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::frontier_response_t::frontier_entry_t& entry, nano_packet& info)
{
	std::error_code ec;
	std::lock_guard<std::mutex> guard (db_mutex);
	stmt_frontier_response->bind(":id", next_id.fetch_add(1));
	stmt_frontier_response->bind(":frontier_request_id", info.flow_data->message_id);
	stmt_frontier_response->bind(":account", pub_to_account(entry.account()));
	stmt_frontier_response->bind(":frontier_hash", to_hex(entry.frontier_hash()));
	stmt_frontier_response->exec();
	stmt_frontier_response->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_bulk_pull_t& msg, nano_packet& info)
{
	std::error_code ec;
	info.flow_data->packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, info.flow_data->packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	
	info.flow_data->message_id = next_id.fetch_add(1);
	stmt_bulk_pull_request->bind(":id", info.flow_data->message_id);
	stmt_bulk_pull_request->bind(":packet_id", info.flow_data->packet_id);
	stmt_bulk_pull_request->bind(":start", pub_to_account(msg.start()));
	stmt_bulk_pull_request->bind(":end", pub_to_account(msg.end()));
	
	if (msg.extended())
	{
		stmt_bulk_pull_request->bind(":extended_count", msg.extended()->count());
	}
	else
	{
		stmt_bulk_pull_request->bind(":extended_count", 0);
	}
	stmt_bulk_pull_request->exec();
	stmt_bulk_pull_request->reset();

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::bulk_pull_response_t::bulk_pull_entry_t& entry, nano_packet& info)
{
	std::error_code ec;
	std::unique_lock<std::mutex> guard (db_mutex);
	
	if (entry.block())
	{
		int64_t block_content_id = next_id.fetch_add(1);
		std::string content_table;
		put_block(entry.block(), block_content_id, info.flow_data->packet_id, content_table);

		if (!content_table.empty())
		{
			stmt_bulk_pull_response->bind(":id", next_id.fetch_add(1));
			stmt_bulk_pull_response->bind(":bulk_pull_request_id", info.flow_data->message_id);
			stmt_bulk_pull_response->bind(":content_id", block_content_id);
			stmt_bulk_pull_response->bind(":content_table", content_table);
			stmt_bulk_pull_response->exec();
			stmt_bulk_pull_response->reset();
		}
		else
		{
			guard.unlock();
			if (++info.flow_data->invalid_response_count == 1)
			{
				put_connection(info.ip_version, info.src_ip, info.src_port, info.dst_ip, info.dst_port, info.flow_key, "first invalid bulk_pull response");
			}

		}
	}
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_bulk_push_t::bulk_push_entry_t& entry, nano_packet& info)
{
	std::error_code ec;
	std::unique_lock<std::mutex> guard (db_mutex);

	if (entry.block())
	{
		int64_t block_content_id = next_id.fetch_add(1);
		std::string content_table;
		put_block(entry.block(), block_content_id, info.flow_data->packet_id, content_table);

		if (!content_table.empty())
		{
			stmt_bulk_push_entry->bind(":id", next_id.fetch_add(1));
			stmt_bulk_push_entry->bind(":packet_id", info.flow_data->packet_id);
			stmt_bulk_push_entry->bind(":content_id", block_content_id);
			stmt_bulk_push_entry->bind(":content_table", content_table);
			stmt_bulk_push_entry->exec();
			stmt_bulk_push_entry->reset();
		}
		else
		{
			std::cout << "BOGUS! block type: " << (int)entry.block_type() << std::endl;
			guard.unlock();
			if (++info.flow_data->invalid_response_count == 1)
			{
				put_connection(info.ip_version, info.src_ip, info.src_port, info.dst_ip, info.dst_port, info.flow_key, "first invalid bulk_push entry");
			}

		}
	}
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_bulk_pull_account_t& msg, nano_packet& info)
{
	std::error_code ec;
	info.flow_data->packet_id = next_id.fetch_add(1);
	info.flow_data->message_id = next_id.fetch_add(1);

	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, info.flow_data->packet_id);
	bind_packet_fields(stmt_packet.get(), info);

	auto rows = stmt_packet->exec();
	assert (rows == 1);

	stmt_bulk_pull_account_request->bind(":id", info.flow_data->message_id);
	stmt_bulk_pull_account_request->bind(":packet_id", info.flow_data->packet_id);
	stmt_bulk_pull_account_request->bind(":account", pub_to_account(msg.account()));
	stmt_bulk_pull_account_request->bind(":minimum_amount", balance_to_dec(msg.minimum_amount()));
	stmt_bulk_pull_account_request->bind(":flags", static_cast<int64_t>(msg.flags()));
	stmt_bulk_pull_account_request->exec();
	stmt_bulk_pull_account_request->reset();

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::bulk_pull_account_response_t::frontier_balance_entry_t& entry, nano_packet& info)
{
	std::error_code ec;
	std::unique_lock<std::mutex> guard (db_mutex);
	stmt_bulk_pull_account_response->bind(":id", next_id.fetch_add(1));
	stmt_bulk_pull_account_response->bind(":bulk_pull_account_request_id", info.flow_data->message_id);
	stmt_bulk_pull_account_response->bind(":frontier_hash", to_hex(entry.frontier_hash()));
	stmt_bulk_pull_account_response->bind(":balance", balance_to_dec(entry.balance()));
	stmt_bulk_pull_account_response->exec();
	stmt_bulk_pull_account_response->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t& entry, nano_packet& info)
{
	std::error_code ec;
	std::unique_lock<std::mutex> guard (db_mutex);
	stmt_bulk_pull_account_response_entry->bind(":id", next_id.fetch_add(1));
	stmt_bulk_pull_account_response_entry->bind(":bulk_pull_account_request_id", info.flow_data->message_id);

	// This logic copies that of the nano node
	bool pending_include_address = false;
	bool pending_address_only = false;
	if (info.flow_data->request_flags == nano::protocol::nano_t::enum_bulk_pull_account_t::ENUM_BULK_PULL_ACCOUNT_PENDING_ADDRESS_ONLY)
	{
		pending_address_only = true;
	}
	else if (info.flow_data->request_flags == nano::protocol::nano_t::enum_bulk_pull_account_t::ENUM_BULK_PULL_ACCOUNT_PENDING_HASH_AMOUNT_AND_ADDRESS)
	{
		pending_include_address = true;
	}

	if (!pending_address_only)
	{
		stmt_bulk_pull_account_response_entry->bind(":hash", to_hex(entry.hash()));
		stmt_bulk_pull_account_response_entry->bind(":amount", balance_to_dec(entry.amount()));
	}

	if (pending_address_only || pending_include_address)
	{
		stmt_bulk_pull_account_response_entry->bind(":source", to_hex(entry.source()));
	}

	stmt_bulk_pull_account_response_entry->exec();
	stmt_bulk_pull_account_response_entry->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_bulk_push_t& msg, nano_packet& info)
{
	std::error_code ec;
	info.flow_data->packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, info.flow_data->packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	
	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_keepalive_t& msg, nanocap::nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);

	auto rows = stmt_packet->exec();
	assert (rows == 1);

	if (msg.peers () != nullptr)
	{
		for (auto& peer : *msg.peers ())
		{
			// std::string#data() is contiguous, so we simply cast it to std::array (which boost expects) to save a copy.
			auto address = peer->address();
			std::array<uint8_t, 16>* arr = reinterpret_cast<std::array<uint8_t, 16>*> ((void*)address.data());
			boost::asio::ip::address_v6 addr(*arr);
			boost::asio::ip::udp::endpoint endpoint (addr, peer->port ());

			stmt_host->bind(":id", next_id.fetch_add(1));
			stmt_host->bind(":packet_id", packet_id);
			stmt_host->bind(":address", endpoint.address().to_string ());
			stmt_host->bind(":port", peer->port());
			stmt_host->exec();
			stmt_host->reset();
		}
	}

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_ack_t& msg, nanocap::nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);
	
	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	// Vote-by-hash
	if (msg.votebyhash() && msg.votebyhash()->hashes() && !msg.votebyhash()->hashes()->empty())
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

	if (msg.common())
	{
		stmt_vote->bind(":account", pub_to_account(msg.common()->account()));
		stmt_vote->bind(":signature", to_hex(msg.common()->signature()));
		stmt_vote->bind(":sequence", static_cast<int64_t>(msg.common()->timestamp_and_vote_duration()));
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

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_confirm_req_t& msg, nanocap::nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	int64_t content_id = next_id.fetch_add(1);
	std::string content_table;
	put_block(msg.block(), content_id, packet_id, content_table);

	stmt_packet->bind(":content_id", content_id);
	stmt_packet->bind(":content_table", content_table);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_node_id_handshake_t& msg, nanocap::nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);

	// Note that a msg can be both a query and a handshake

	if (msg.query() != nullptr)
	{
		stmt_nodeid_query->bind(":id", next_id.fetch_add(1));
		stmt_nodeid_query->bind(":packet_id", packet_id);
		stmt_nodeid_query->bind(":cookie", nanocap::to_hex(msg.query()->cookie()));
		stmt_nodeid_query->exec();
		stmt_nodeid_query->reset();
	}

	if (msg.response() != nullptr)
	{
		stmt_nodeid_response->bind(":id", next_id.fetch_add(1));
		stmt_nodeid_response->bind(":packet_id", packet_id);
		stmt_nodeid_response->bind(":nodeid", nanocap::to_hex(msg.response()->account()));
		stmt_nodeid_response->bind(":nodeid_as_account", pub_to_account(msg.response()->account()));
		stmt_nodeid_response->bind(":signature", nanocap::to_hex(msg.response()->signature()));
		stmt_nodeid_response->exec();
		stmt_nodeid_response->reset();
	}

	// Prepare for next use
	stmt_packet->reset();
	return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_publish_t& msg, nanocap::nano_packet& info)
{
	std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
	std::lock_guard<std::mutex> guard (db_mutex);

	bind_header_fields(stmt_packet.get(), msg, packet_id);
	bind_packet_fields(stmt_packet.get(), info);

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

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_asc_pull_ack_t& msg, nanocap::nano_packet& info)
{	
    std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
    std::lock_guard<std::mutex> guard (db_mutex);    
    bind_header_fields(stmt_packet.get(), msg, packet_id);
    bind_packet_fields(stmt_packet.get(), info);
	stmt_packet->bind(":content_id", packet_id);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	// Prepare for next use
	stmt_packet->reset();

    // Continue as per your existing structure
    return ec;
}

std::error_code nanocap::db::put(nano::protocol::nano_t::msg_asc_pull_req_t& msg, nanocap::nano_packet& info)
{
    std::error_code ec;
	int64_t packet_id = next_id.fetch_add(1);
    std::lock_guard<std::mutex> guard (db_mutex);    
    bind_header_fields(stmt_packet.get(), msg, packet_id);
    bind_packet_fields(stmt_packet.get(), info);
	stmt_packet->bind(":content_id", packet_id);
	
	auto rows = stmt_packet->exec();
	assert (rows == 1);
	// Prepare for next use
	stmt_packet->reset();

    // Continue as per your existing structure
    return ec;
}




// Must be called with db_mutex locked
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
				content_table = "block_state";
				auto block = static_cast<nano::protocol::nano_t::block_state_t*>(block_selector->block());
				stmt_block_state->bind(":id", id);
				stmt_block_state->bind(":packet_id", packet_id);
				stmt_block_state->bind(":hash", to_hex(hash_of(block)));
				stmt_block_state->bind(":account", pub_to_account(block->account()));
				stmt_block_state->bind(":previous", to_hex(block->previous()));
				stmt_block_state->bind(":representative", pub_to_account(block->representative()));
				stmt_block_state->bind(":balance", balance_to_dec(block->balance()));
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
				stmt_block_send->bind(":destination", pub_to_account(block->destination()));
				stmt_block_send->bind(":balance", balance_to_dec(block->balance()));
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
				stmt_block_open->bind(":representative", pub_to_account(block->representative()));
				stmt_block_open->bind(":account", pub_to_account(block->account()));
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
				stmt_block_change->bind(":representative", pub_to_account(block->representative()));
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
		int i = 0;
		for (auto const& count : count_per_type)
		{
			arr.push_back(json::object());
			json& obj = arr.back();
			obj["count"] = count;
			obj["type"] = i++;
		}
		res = arr.dump(5);
	}
	catch (std::runtime_error& ex)
	{
		res = ex.what();
	}
	
	return res;
}

void nanocap::db::init_packets_per_type_from_database()
{
	std::lock_guard<std::mutex> guard (db_mutex);
	while (stmt_packet_per_msg_type->executeStep())
	{
		count_per_type[stmt_packet_per_msg_type->getColumn(1).getInt64()] = stmt_packet_per_msg_type->getColumn(0).getInt64();
	}
	stmt_packet_per_msg_type->reset();
}
