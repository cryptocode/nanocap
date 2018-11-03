#pragma once

#include <nano.h>
#include <list>
#include <mutex>
#include "UdpLayer.h"
#include "TcpLayer.h"
#include "IPv4Layer.h"
#include "IPv6Layer.h"
#include "PcapLiveDeviceList.h"
#include "PlatformSpecificUtils.h"

namespace nanocap
{
	class app;
	class packet_handler
	{
	public:
		packet_handler(nanocap::app& app);
		~packet_handler();
		void start_capture();
		void stop_capture();
		void print_device_info(std::ostream & ostream);
		void print_devices();
		void print_stats(std::ostream & ostream);
		void consume(pcpp::Packet& packet);
		
		void handle_udp(pcpp::Packet& packet);
		void handle_message (nano::protocol::nano_t::msg_keepalive_t & msg);
		void handle_message (nano::protocol::nano_t::msg_confirm_ack_t & msg);
		void handle_message (nano::protocol::nano_t::msg_confirm_req_t & msg);

		inline pcpp::PcapLiveDevice* get_device() { return dev; }

		template <class T, size_t N, class V>
		std::array<T, N> str_to_array (const V && v)
		{
			assert (v.size () == N);
			std::array<T, N> d;
			std::copy (v.begin (), v.end (), d.data ());
			return d;
		}
		
	private:
		nanocap::app& app;
		pcpp::PcapLiveDevice* dev {nullptr};
		std::mutex queue_mutex;
		std::list<std::string> queue;
		
		uint16_t current_dst_port;
		uint16_t current_src_port;
		std::string current_dst_ip;
		std::string current_src_ip;
		bool current_inbound;
		bool current_outbound;
		
		int ethPacketCount = 0;
		int ipv4PacketCount = 0;
		int ipv6PacketCount = 0;
		int tcpPacketCount = 0;
		int udpPacketCount = 0;
		int dnsPacketCount = 0;
		int httpPacketCount = 0;
		int sslPacketCount = 0;
		
		int in_keepalive = 0;
		int out_keepalive = 0;
	};
}
