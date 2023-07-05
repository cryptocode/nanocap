#pragma once

#include <array>
#include <atomic>
#include <string>
#include <mutex>
#include <nano.h>
#include <shared.hpp>
#include <config.hpp>
#include <ctime>
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nanocap
{
	class app;
	struct nano_packet;

	/** Manages the capture database */
	class db
	{
	public:
		db(nanocap::app& app);
		~db();
		
		std::error_code put_connection(int ipVersion, std::string srcip, uint16_t srcport, std::string dstip, uint16_t dstport, uint32_t flow_key, std::string event);
		std::error_code put(nano::protocol::nano_t::msg_keepalive_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_confirm_ack_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_confirm_req_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_publish_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_node_id_handshake_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_frontier_req_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::frontier_response_t::frontier_entry_t& entry, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_telemetry_req_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_telemetry_ack_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_bulk_pull_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::bulk_pull_response_t::bulk_pull_entry_t& entry, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_bulk_push_t::bulk_push_entry_t& entry, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_bulk_push_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_bulk_pull_account_t& msg, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::bulk_pull_account_response_t::frontier_balance_entry_t& entry, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t& entry, nano_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_asc_pull_ack_t& msg, nano_packet& info);		
		std::error_code put(nano::protocol::nano_t::msg_asc_pull_req_t& msg, nano_packet& info);

		/** Arbitrary query returned as json, including (real and synthentic) column names */
		json query(std::string query);
		
		int64_t count_packets();

		/** Returns a json string containing packet count per message type [{count: number, msg_type: number}, ...] */
		std::string count_packets_per_type();
		
		/**
		 * Returns the database schema as a json object
		 */
		json get_schema();
		
		/**
		 * Returns the database schema as a json string
		 */
		std::string get_schema_json();
		
		/**
		 * Remove all captured data
		 */
		std::error_code destroy_capture_data();

		/**
		 * Flush data to database
		 */
		std::error_code flush();
		
		/** If true, the maximum db size is reached */
		std::atomic<bool> max_reached {false};

	private:
		std::unique_ptr<SQLite::Transaction> primary_tx;
		std::error_code put_block(nano::protocol::nano_t::block_selector_t* block_selector, int64_t id, int64_t packet_id, /*out*/ std::string& content_table);
		std::error_code flush_internal();
		void init_packets_per_type_from_database();
		// Larger than necessary to accommodate future message types (max enum type not available in generated header, and space overhead is miniscule)
		std::array<size_t, 32> count_per_type {};

		/**
		 * Set the header portion of the message. This is shared across message types.
		 * @note This must be called under a \db_mutex lock.
		 */
		template <typename T>
		std::error_code bind_header_fields(SQLite::Statement* stmt, T&& msg, int64_t id)
		{
			// Periodically commit and check db size against maximum
			if (next_id % 10000 == 0)
			{
				primary_tx->commit();
				primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
				
				int64_t page_size = sqlite->execAndGet("PRAGMA page_size").getInt64();
				int64_t page_count = sqlite->execAndGet("PRAGMA page_count").getInt64();
				auto size_bytes (page_size * page_count);
				if (size_bytes > app.get_config().capture.max_capture_megabytes * 1024LL * 1024LL)
				{
					max_reached = true;
				}
			}

			std::error_code ec;
			stmt->clearBindings();
			stmt->bind(":id", id);
			stmt->bind(":hdr_msg_type", msg._parent()->header()->message_type());
			stmt->bind(":hdr_version_using", msg._parent()->header()->version_using());
			stmt->bind(":hdr_version_min", msg._parent()->header()->version_min());
			stmt->bind(":hdr_block_type", static_cast<int>(msg._parent()->header()->block_type()));
			stmt->bind(":hdr_extensions", static_cast<int>(msg._parent()->header()->extensions()));
			// This is overriden by certain messages
			stmt->bind(":content_table", nullptr);
			stmt->bind(":content_id", nullptr);

			count_per_type[msg._parent()->header()->message_type()]++;
			return ec;
		}
		
		std::error_code bind_packet_fields(SQLite::Statement* stmt, nanocap::nano_packet& info)
		{
			std::error_code ec;
			stmt->bind(":srcip", info.src_ip);
			stmt->bind(":srcport", info.src_port);
			stmt->bind(":dstip", info.dst_ip);
			stmt->bind(":dstport", info.dst_port);
			stmt->bind(":time_usec", info.timestamp_usec);
			stmt->bind(":flow_key", info.flow_key);
			stmt->bind(":ipv", info.ip_version);

			if (std::strftime(info.time_string, sizeof(info.time_string), "%Y-%m-%d %H:%M:%S", std::gmtime(&info.timestamp))) {
				stmt->bind(":time", info.time_string);
			}
			
			return ec;
		}

		nanocap::app& app;
		std::unique_ptr<SQLite::Database> sqlite;
		std::unique_ptr<SQLite::Statement> stmt_connection;
		std::unique_ptr<SQLite::Statement> stmt_packet;
		std::unique_ptr<SQLite::Statement> stmt_block_state;
		std::unique_ptr<SQLite::Statement> stmt_block_send;
		std::unique_ptr<SQLite::Statement> stmt_block_receive;
		std::unique_ptr<SQLite::Statement> stmt_block_open;
		std::unique_ptr<SQLite::Statement> stmt_block_change;
		std::unique_ptr<SQLite::Statement> stmt_vote;
		std::unique_ptr<SQLite::Statement> stmt_host;
		std::unique_ptr<SQLite::Statement> stmt_packet_per_msg_type;
		std::unique_ptr<SQLite::Statement> stmt_nodeid_query;
		std::unique_ptr<SQLite::Statement> stmt_nodeid_response;
		std::unique_ptr<SQLite::Statement> stmt_telemetry;
		std::unique_ptr<SQLite::Statement> stmt_frontier_request;
		std::unique_ptr<SQLite::Statement> stmt_frontier_response;
		std::unique_ptr<SQLite::Statement> stmt_bulk_pull_request;
		std::unique_ptr<SQLite::Statement> stmt_bulk_pull_response;
		std::unique_ptr<SQLite::Statement> stmt_bulk_push_entry;
		std::unique_ptr<SQLite::Statement> stmt_bulk_pull_account_request;
		std::unique_ptr<SQLite::Statement> stmt_bulk_pull_account_response;
		std::unique_ptr<SQLite::Statement> stmt_bulk_pull_account_response_entry;
		std::unique_ptr<SQLite::Statement> stmt_msg_asc_pull;

		/** Next ID for sqlite insert (note that sqlite3 only supports signed integers */
		std::atomic<std::int64_t> next_id {0};
		
		std::mutex db_mutex;
	};
}
