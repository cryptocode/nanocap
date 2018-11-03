#pragma once
#include <nano.h>
#include <string>
#include <mutex>
#include <config.hpp>
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nanocap
{
	class app;

	/** Manages the capture database */
	class db
	{
	public:
		db(nanocap::app& app);
		~db();
		
		std::error_code put(nano::protocol::nano_t::msg_keepalive_t& msg);
		std::error_code put(nano::protocol::nano_t::msg_confirm_ack_t& msg);
		std::error_code put(nano::protocol::nano_t::msg_confirm_req_t& msg);
		
		// TODO:
		
		int64_t count_packets();
		
		/**
		 * Returns the database schema as a json object
		 */
		json get_schema();
		
		/**
		 * Returns the database schema as a json string
		 */
		std::string get_schema_json();
		
		/**
		 * Flush data to database
		 */
		std::error_code flush();

	private:
		std::unique_ptr<SQLite::Transaction> primary_tx;
		
		/**
		 * Set the header portion of the message. This is shared across message types.
		 * @note This must be called under a \db_mutex lock.
		 */
		template <typename T>
		inline std::error_code set_header(SQLite::Statement* stmt, T&& msg, int64_t id)
		{
			if (next_id % 10000 == 0)
			{
				primary_tx->commit();
				primary_tx = std::make_unique<SQLite::Transaction> (*sqlite);
			}
			// TODO: set synthetic inbound vs. outbound flag
			std::error_code ec;
			stmt->bind(":id", id);
			stmt->bind(":hdr_msg_type", msg._parent()->header()->message_type());
			stmt->bind(":hdr_version_using", msg._parent()->header()->version_using());
			stmt->bind(":hdr_version_min", msg._parent()->header()->version_min());
			stmt->bind(":hdr_block_type", static_cast<int>(msg._parent()->header()->block_type()));
			
			return ec;
		}

		nanocap::app& app;
		std::unique_ptr<SQLite::Database> sqlite;
		std::unique_ptr<SQLite::Statement> stmt_packet;
		std::unique_ptr<SQLite::Statement> stmt_block_state;
		std::unique_ptr<SQLite::Statement> stmt_host;
		
		/** Next ID for sqlite insert (note that sqlite3 only supports signed integers */
		std::atomic_int64_t next_id {0};
		
		std::mutex db_mutex;
	};
}
