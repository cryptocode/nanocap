#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace nanocap
{
	inline std::string to_hex(const std::string& str)
	{
		std::stringstream ss;
		for (size_t i=0 ; i<str.length() ; ++i)
		{
			ss << std::setw(2) << std::setfill('0')
			<< std::uppercase << std::hex << (int(str[i])&0xFF);
		}
		return ss.str();
	}
}
