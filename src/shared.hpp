#pragma once

#include <string>
#include <cstdint>

namespace nanocap
{
	struct tcp_flow_data;
	/** Information about the currently processed package */
	struct nano_packet
	{
		std::string src_ip;
		std::string dst_ip;
		uint16_t src_port;
		uint16_t dst_port;
		/** Seconds since unix epoch */
		time_t timestamp;
		char time_string[32];
		/** Microseconds */
		long timestamp_usec {0};
		/** IP version (4 or 6) */
		uint8_t ip_version;
		std::string data;
		int side;
		uint32_t flow_key{0};
		std::shared_ptr<tcp_flow_data> flow_data;
	};
}

