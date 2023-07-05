#pragma once

#include <list>
#include <map>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <thread>
#include <deque>
#include <atomic>
#include <nano.h>
#include <shared.hpp>
#include <boost/circular_buffer.hpp>

namespace pcpp
{
	class PcapLiveDevice;
	class Packet;
	class TcpReassembly;
}

namespace nanocap
{
	class app;

	/** Per connection state */
	struct tcp_flow_data
	{
		tcp_flow_data()
		{
			reset();
		}

		/** Reset to initial values */
		void reset(nano::protocol::nano_t::enum_msgtype_t message_type = nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_INVALID)
		{
			requesting_side = 0;
			step = 0;
			packet_id = 0;
			message_id = 0;
			response_count = 0;
			invalid_response_count= 0;
			request_flags = 0;
			current_msg_type = message_type;
			accumulated_data.clear();
		}

		uint8_t requesting_side;

		/** At which step in the parsing process are we on this connect? The use of this depends on message type. */
		uint8_t step;

		/** Some requests may have flags which affect how responses are interpreted, such as bulk_pull_account */
		uint8_t request_flags;

		/** The message this connection is currently dealing with */
		nano::protocol::nano_t::enum_msgtype_t current_msg_type;

		/** Unique packet id */
		int64_t packet_id;

		/** If > 0, this is the id of the message that causes this flow, such as a frontier req */
		int64_t message_id;

		/** Debugging aid to keep track of number of entries processed from a response stream */
		uint64_t response_count;

		/** Debugging aid to keep track of number of invalid entries in a response stream */
		uint64_t invalid_response_count;

		/**
		 * This is used to parse responses, entry by entry. We use a circular buffer to deal with entries that cross
		 * packets. The buffer only needs to be large enough to hold a single entry, for instance a frontier
		 * account/block pair. We keep it short as the length affect linearize() performance.
		 */
		boost::circular_buffer<uint8_t> accumulated_data {4096};
	};
	
	/** Packet capture */
	class packet_handler
	{
	public:
		packet_handler(nanocap::app& app);
		~packet_handler();

		void start_capture();
		void stop_capture();
		void stop_packet_processing_thread();
		void analyze_pcap_file(std::string path);
		void print_device_info(std::ostream & ostream);
		void print_devices();
		void print_stats();
		void run_packet_processing_loop();
		size_t packet_queue_size();
		void handle_tcp(nanocap::nano_packet& packet);
		void handle_message (nano::protocol::nano_t::msg_keepalive_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_publish_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_node_id_handshake_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_telemetry_req_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_telemetry_ack_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_frontier_req_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_bulk_pull_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_bulk_push_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_bulk_pull_account_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_asc_pull_ack_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_asc_pull_req_t & msg, nano_packet& info);


		pcpp::PcapLiveDevice* get_device() { return dev; }
		nanocap::app& get_app() const { return app; }
		
		bool is_live_capturing () const { return capturing; };
		bool is_file_importing () const { return importing; };
		
		/** flowKey -> data map*/
		std::map<uint32_t, std::shared_ptr<tcp_flow_data>> connMgr;
		std::unique_ptr<pcpp::TcpReassembly> tcpReassembly;

		std::condition_variable packet_queue_cond;
		std::mutex packet_queue_mutex;
		std::deque<nanocap::nano_packet> packet_queue;
		std::atomic<bool> stopped{false};
		std::atomic<int> packet_parse_error_count{0};

	private:
		nanocap::app& app;
		pcpp::PcapLiveDevice* dev {nullptr};
		std::thread packet_processing_thread;
		std::mutex queue_mutex;
		std::atomic<bool> capturing {false};
		std::atomic<bool> importing {false};
	};
}
