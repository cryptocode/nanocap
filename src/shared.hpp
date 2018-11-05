#pragma once

#include <string>
#include <cstdint>

namespace nanocap
{
	/** Information about the currently processed udp package */
	struct udp_packet
	{
		std::string src_ip;
		std::string dst_ip;
		uint16_t src_port;
		uint16_t dst_port;
		/** Seconds since unix epoch */
		time_t timestamp;
		/** Microseconds */
		long timestamp_usec {0};
		/** IP version (4 or 6) */
		uint8_t ip_version;
	};
}
