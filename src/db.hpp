#pragma once

#include <atomic>
#include <string>
#include <mutex>
#include <nano.h>
#include <shared.hpp>
#include <config.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nanocap
{
	class app;
	struct udp_packet;

	/** Manages the capture database */
	class db
	{
	public:
		db(nanocap::app& app);
		~db();
		
		std::error_code put(nano::protocol::nano_t::msg_keepalive_t& msg, udp_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_confirm_ack_t& msg, udp_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_confirm_req_t& msg, udp_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_publish_t& msg, udp_packet& info);
		std::error_code put(nano::protocol::nano_t::msg_node_id_handshake_t& msg, udp_packet& info);

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
		std::atomic_bool max_reached {false};

	private:
		std::unique_ptr<SQLite::Transaction> primary_tx;
		std::error_code put_block(nano::protocol::nano_t::block_selector_t* block_selector, int64_t packet_id);
		
		/**
		 * Set the header portion of the message. This is shared across message types.
		 * @note This must be called under a \db_mutex lock.
		 */
		template <typename T>
		inline std::error_code bind_header_fields(SQLite::Statement* stmt, T&& msg, int64_t id)
		{
			// Periodically commit and check db size against maximum
			if (next_id % 10000 == 0)
			{
				primary_tx->commit();
				primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
				
				int64_t page_size = sqlite->execAndGet("PRAGMA page_size").getInt64();
				int64_t page_count = sqlite->execAndGet("PRAGMA page_count").getInt64();
				auto size (page_size * page_count);
				if (size > app.get_config().capture.max_capture_megabytes * 1024LL * 1024LL)
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
			stmt->bind(":block_table", nullptr);
			return ec;
		}
		
		inline std::error_code bind_udp_fields(SQLite::Statement* stmt, nanocap::udp_packet& info)
		{
			std::error_code ec;
			stmt->bind(":srcip", info.src_ip);
			stmt->bind(":srcport", info.src_port);
			stmt->bind(":dstip", info.dst_ip);
			stmt->bind(":dstport", info.dst_port);
			stmt->bind(":time", info.timestamp);
			stmt->bind(":time_usec", info.timestamp_usec);
			stmt->bind(":ipv", info.ip_version);
			return ec;
		}

		nanocap::app& app;
		std::unique_ptr<SQLite::Database> sqlite;
		std::unique_ptr<SQLite::Statement> stmt_packet;
		std::unique_ptr<SQLite::Statement> stmt_block_state;
		std::unique_ptr<SQLite::Statement> stmt_block_send;
		std::unique_ptr<SQLite::Statement> stmt_block_receive;
		std::unique_ptr<SQLite::Statement> stmt_block_open;
		std::unique_ptr<SQLite::Statement> stmt_block_change;
		std::unique_ptr<SQLite::Statement> 		stmt_vote;
		std::unique_ptr<SQLite::Statement> stmt_host;
		std::unique_ptr<SQLite::Statement> stmt_packet_per_msg_type;
		
		/** Next ID for sqlite insert (note that sqlite3 only supports signed integers */
		std::atomic<std::int64_t> next_id {0};
		
		std::mutex db_mutex;
	};
}
