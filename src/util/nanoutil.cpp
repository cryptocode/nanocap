#include <util/nanoutil.hpp>
#include <blake2.h>

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
