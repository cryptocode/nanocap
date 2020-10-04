#pragma once

#include <string>
#include <nano.h>

namespace nanocap
{
	std::string pub_to_account(std::string const& public_bytes);
	std::string balance_to_dec(std::string const& balance_raw_bytes);
	std::string hash_of(nano::protocol::nano_t::block_state_t* block);
	std::string hash_of(nano::protocol::nano_t::block_send_t* block);
	std::string hash_of(nano::protocol::nano_t::block_receive_t* block);
	std::string hash_of(nano::protocol::nano_t::block_open_t* block);
	std::string hash_of(nano::protocol::nano_t::block_change_t* block);
}
