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
		tcp_flow_data() {
			accumulated_data.clear();
		}
		
		int requesting_side {0};

		int64_t packet_id {0};
		/** If > 0, this is the id of the message that causes this flow, such as a frontier req */
		int64_t message_id {0};

		/** The message this connection is currently dealing with */
		nano::protocol::nano_t::enum_msgtype_t current_msg_type { nano::protocol::nano_t::enum_msgtype_t::ENUM_MSGTYPE_INVALID };

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
		void analyze_pcap_file(std::string path);
		void print_device_info(std::ostream & ostream);
		void print_devices();
		void print_stats();
		void run_packet_processing_loop();

		void handle_udp(pcpp::Packet& packet);
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
		void handle_message (nano::protocol::nano_t::msg_bulk_pull_blocks_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_bulk_push_t & msg, nano_packet& info);
		void handle_message (nano::protocol::nano_t::msg_bulk_pull_account_t & msg, nano_packet& info);

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

	private:
		nanocap::app& app;
		pcpp::PcapLiveDevice* dev {nullptr};
		std::thread packet_processing_thread;
		std::mutex queue_mutex;
		std::list<std::string> queue;
		std::atomic_bool capturing {false};
		std::atomic_bool importing {false};
	};
}
