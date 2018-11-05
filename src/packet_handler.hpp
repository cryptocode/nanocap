#pragma once

#include <list>
#include <mutex>
#include <nano.h>
#include <shared.hpp>

namespace pcpp
{
	class PcapLiveDevice;
	class Packet;
}

namespace nanocap
{
	class app;
	
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
		void print_stats(std::ostream & ostream);
		void consume(pcpp::Packet& packet);
		
		void handle_udp(pcpp::Packet& packet);
		void handle_message (nano::protocol::nano_t::msg_keepalive_t & msg, udp_packet& info);
		void handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg, udp_packet& info);
		void handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg, udp_packet& info);
		void handle_message (nano::protocol::nano_t::msg_publish_t & msg, udp_packet& info);

		inline pcpp::PcapLiveDevice* get_device() { return dev; }
		inline bool is_live_capturing () const { return capturing; };
		inline bool is_file_importing () const { return importing; };
		
	private:
		nanocap::app& app;
		pcpp::PcapLiveDevice* dev {nullptr};
		std::mutex queue_mutex;
		std::list<std::string> queue;
		std::atomic_bool capturing {false};
		std::atomic_bool importing {false};

		int ethPacketCount = 0;
		int ipv4PacketCount = 0;
		int ipv6PacketCount = 0;
		int tcpPacketCount = 0;
		int udpPacketCount = 0;
		int dnsPacketCount = 0;
		int httpPacketCount = 0;
		int sslPacketCount = 0;
	};
}
