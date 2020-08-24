#include <util/nanoutil.hpp>
#include <blake2.h>
#include <assert.h>
#include <boost/multiprecision/cpp_int.hpp>

static char account_encode (uint8_t value)
{
	static char const * account_lookup ("13456789abcdefghijkmnopqrstuwxyz");
	assert (value < 32);
	auto result (account_lookup[value]);
	return result;
}

boost::multiprecision::uint256_t number (std::string bytes)
{
	boost::multiprecision::uint256_t result;
	auto shift (0);
	for (auto i (bytes.begin ()), n (bytes.end ()); i != n; ++i)
	{
		result <<= shift;
		result |= (uint8_t)*i;
		shift = 8;
	}
	return result;
}

std::string nanocap::pub_to_account (std::string public_bytes)
{
	std::string destination_a;
	destination_a.reserve (64);
	uint64_t check (0);
	blake2b_state hash;
	blake2b_init (&hash, 5);
	blake2b_update (&hash, public_bytes.data (), public_bytes.size ());
	blake2b_final (&hash, reinterpret_cast<uint8_t *> (&check), 5);
	boost::multiprecision::uint512_t number_l (number (public_bytes));
	number_l <<= 40;
	number_l |= boost::multiprecision::uint512_t (check);

	for (auto i (0); i < 60; ++i)
	{
		uint8_t r (number_l & static_cast<uint8_t> (0x1f));
		number_l >>= 5;
		destination_a.push_back (account_encode (r));
	}
	destination_a.append ("_onan");
	std::reverse (destination_a.begin (), destination_a.end ());
	return destination_a;
}

std::string nanocap::hash_of(nano::protocol::nano_t::block_state_t* block)
{
	uint8_t preamble[32] {};
	uint8_t result[32];
	preamble[31] = nano::protocol::nano_t::enum_blocktype_t::ENUM_BLOCKTYPE_STATE;
	blake2b_state hash_state;
	auto status (blake2b_init (&hash_state, sizeof (result)));
	assert (status == 0);
	
	blake2b_update (&hash_state, preamble, sizeof(preamble));
	blake2b_update (&hash_state, block->account().data (), block->account().size());
	blake2b_update (&hash_state, block->previous().data (), block->previous().size());
	blake2b_update (&hash_state, block->representative().data (), block->representative().size());
	blake2b_update (&hash_state, block->balance().data (), block->balance().size());
	blake2b_update (&hash_state, block->link().data (), block->link().size());
	
	status = blake2b_final (&hash_state, result, sizeof (result));
	assert (status == 0);
	
	return std::string((const char*)result, sizeof(result));
}

std::string nanocap::hash_of(nano::protocol::nano_t::block_send_t* block)
{
	uint8_t result[32];
	blake2b_state hash_state;
	auto status (blake2b_init (&hash_state, sizeof (result)));
	assert (status == 0);
	
	blake2b_update (&hash_state, block->previous().data (), block->previous().size());
	blake2b_update (&hash_state, block->destination().data (), block->destination().size());
	blake2b_update (&hash_state, block->balance().data (), block->balance().size());
	
	status = blake2b_final (&hash_state, result, sizeof (result));
	assert (status == 0);
	
	return std::string((const char*)result, sizeof(result));
}

std::string nanocap::hash_of(nano::protocol::nano_t::block_receive_t* block)
{
	uint8_t result[32];
	blake2b_state hash_state;
	auto status (blake2b_init (&hash_state, sizeof (result)));
	assert (status == 0);
	
	blake2b_update (&hash_state, block->previous().data (), block->previous().size());
	blake2b_update (&hash_state, block->source().data (), block->source().size());
	
	status = blake2b_final (&hash_state, result, sizeof (result));
	assert (status == 0);
	
	return std::string((const char*)result, sizeof(result));
}

std::string nanocap::hash_of(nano::protocol::nano_t::block_open_t* block)
{
	uint8_t result[32];
	blake2b_state hash_state;
	auto status (blake2b_init (&hash_state, sizeof (result)));
	assert (status == 0);
	
	blake2b_update (&hash_state, block->source().data (), block->source().size());
	blake2b_update (&hash_state, block->representative().data (), block->representative().size());
	blake2b_update (&hash_state, block->account().data (), block->account().size());
	
	status = blake2b_final (&hash_state, result, sizeof (result));
	assert (status == 0);
	
	return std::string((const char*)result, sizeof(result));
}

std::string nanocap::hash_of(nano::protocol::nano_t::block_change_t* block)
{
	uint8_t result[32];
	blake2b_state hash_state;
	auto status (blake2b_init (&hash_state, sizeof (result)));
	assert (status == 0);
	
	blake2b_update (&hash_state, block->previous().data (), block->previous().size());
	blake2b_update (&hash_state, block->representative().data (), block->representative().size());
	
	status = blake2b_final (&hash_state, result, sizeof (result));
	assert (status == 0);
	
	return std::string((const char*)result, sizeof(result));
}
