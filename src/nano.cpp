// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "nano.h"


namespace nano {
    namespace protocol {

        nano_t::nano_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = this;
            m_header = 0;
            f_const_block_zero = false;
            _read();
        }

        void nano_t::_read() {
            m_header = new message_header_t(m__io, this, m__root);
            switch (header()->message_type()) {
            case nano_t::ENUM_MSGTYPE_CONFIRM_REQ: {
                m_body = new msg_confirm_req_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_CONFIRM_ACK: {
                m_body = new msg_confirm_ack_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_BULK_PULL: {
                m_body = new msg_bulk_pull_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_BULK_PULL_ACCOUNT: {
                m_body = new msg_bulk_pull_account_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_KEEPALIVE: {
                m_body = new msg_keepalive_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_BULK_PUSH: {
                m_body = new msg_bulk_push_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_NODE_ID_HANDSHAKE: {
                m_body = new msg_node_id_handshake_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_FRONTIER_REQ: {
                m_body = new msg_frontier_req_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_BULK_PULL_BLOCKS: {
                m_body = new msg_bulk_pull_blocks_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_PUBLISH: {
                m_body = new msg_publish_t(m__io, this, m__root);
                break;
            }
            default: {
                m_body = new ignore_until_eof_t(m__io, this, m__root);
                break;
            }
            }
        }

        nano_t::~nano_t() {
            delete m_header;
            delete m_body;
            if (f_const_block_zero) {
            }
        }

        nano_t::block_send_t::block_send_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::block_send_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_destination = m__io->read_bytes(32);
            m_balance = m__io->read_bytes(16);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_send_t::~block_send_t() {
        }

        nano_t::block_selector_t::block_selector_t(uint8_t p_arg_block_type, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_arg_block_type = p_arg_block_type;
            _read();
        }

        void nano_t::block_selector_t::_read() {
            switch (arg_block_type()) {
            case nano_t::ENUM_BLOCKTYPE_RECEIVE: {
                m_block = new block_receive_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_BLOCKTYPE_CHANGE: {
                m_block = new block_change_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_BLOCKTYPE_STATE: {
                m_block = new block_state_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_BLOCKTYPE_OPEN: {
                m_block = new block_open_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_BLOCKTYPE_SEND: {
                m_block = new block_send_t(m__io, this, m__root);
                break;
            }
            default: {
                m_block = new ignore_until_eof_t(m__io, this, m__root);
                break;
            }
            }
        }

        nano_t::block_selector_t::~block_selector_t() {
            delete m_block;
        }

        nano_t::node_id_query_t::node_id_query_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::node_id_query_t::_read() {
            m_node_id = m__io->read_bytes(32);
        }

        nano_t::node_id_query_t::~node_id_query_t() {
        }

        nano_t::block_receive_t::block_receive_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::block_receive_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_source = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_receive_t::~block_receive_t() {
        }

        nano_t::block_change_t::block_change_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::block_change_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_representative = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_change_t::~block_change_t() {
        }

        nano_t::msg_bulk_pull_t::msg_bulk_pull_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::msg_bulk_pull_t::_read() {
            m_start = m__io->read_bytes(32);
            m_end = m__io->read_bytes(32);
        }

        nano_t::msg_bulk_pull_t::~msg_bulk_pull_t() {
        }

        nano_t::peer_t::peer_t(kaitai::kstream* p__io, nano_t::msg_keepalive_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::peer_t::_read() {
            m_address = m__io->read_bytes(16);
            m_port = m__io->read_u2le();
        }

        nano_t::peer_t::~peer_t() {
        }

        nano_t::msg_bulk_pull_blocks_t::msg_bulk_pull_blocks_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::msg_bulk_pull_blocks_t::_read() {
            m_block_type = m__io->read_u1();
        }

        nano_t::msg_bulk_pull_blocks_t::~msg_bulk_pull_blocks_t() {
        }

        nano_t::block_open_t::block_open_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::block_open_t::_read() {
            m_source = m__io->read_bytes(32);
            m_representative = m__io->read_bytes(32);
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_open_t::~block_open_t() {
        }

        nano_t::ignore_until_eof_t::ignore_until_eof_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_empty = 0;
            _read();
        }

        void nano_t::ignore_until_eof_t::_read() {
            n_empty = true;
            if (!(_io()->is_eof())) {
                n_empty = false;
                m_empty = new std::vector<uint8_t>();
                {
                    int i = 0;
                    uint8_t _;
                    do {
                        _ = m__io->read_u1();
                        m_empty->push_back(_);
                        i++;
                    } while (!(_io()->is_eof()));
                }
            }
        }

        nano_t::ignore_until_eof_t::~ignore_until_eof_t() {
            if (!n_empty) {
                delete m_empty;
            }
        }

        nano_t::msg_confirm_ack_t::msg_confirm_ack_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_common = 0;
            m_votebyhash = 0;
            m_block = 0;
            _read();
        }

        void nano_t::msg_confirm_ack_t::_read() {
            m_common = new vote_common_t(m__io, this, m__root);
            n_votebyhash = true;
            if (_root()->header()->block_type() == nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK) {
                n_votebyhash = false;
                m_votebyhash = new vote_by_hash_t(m__io, this, m__root);
            }
            n_block = true;
            if (_root()->header()->block_type() != nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK) {
                n_block = false;
                m_block = new block_selector_t(_root()->header()->block_type_int(), m__io, this, m__root);
            }
        }

        nano_t::msg_confirm_ack_t::~msg_confirm_ack_t() {
            delete m_common;
            if (!n_votebyhash) {
                delete m_votebyhash;
            }
            if (!n_block) {
                delete m_block;
            }
        }

        nano_t::msg_node_id_handshake_t::msg_node_id_handshake_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_query = 0;
            m_response = 0;
            _read();
        }

        void nano_t::msg_node_id_handshake_t::_read() {
            n_query = true;
            if (_root()->header()->query_flag() == 1) {
                n_query = false;
                m_query = new node_id_query_t(m__io, this, m__root);
            }
            n_response = true;
            if (_root()->header()->response_flag() == 1) {
                n_response = false;
                m_response = new node_id_response_t(m__io, this, m__root);
            }
        }

        nano_t::msg_node_id_handshake_t::~msg_node_id_handshake_t() {
            if (!n_query) {
                delete m_query;
            }
            if (!n_response) {
                delete m_response;
            }
        }

        nano_t::msg_frontier_req_t::msg_frontier_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::msg_frontier_req_t::_read() {
            m_start = m__io->read_bytes(32);
            m_age = m__io->read_u4le();
            m_count = m__io->read_u4le();
        }

        nano_t::msg_frontier_req_t::~msg_frontier_req_t() {
        }

        nano_t::msg_bulk_push_t::msg_bulk_push_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;
            _read();
        }

        void nano_t::msg_bulk_push_t::_read() {
            m_entry = new std::vector<bulk_push_entry_t*>();
            {
                int i = 0;
                bulk_push_entry_t* _;
                do {
                    _ = new bulk_push_entry_t(m__io, this, m__root);
                    m_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (entry()->at(i)->block_type() == nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK)) ));
            }
        }

        nano_t::msg_bulk_push_t::~msg_bulk_push_t() {
            for (std::vector<bulk_push_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                delete *it;
            }
            delete m_entry;
        }

        nano_t::msg_bulk_push_t::bulk_push_entry_t::bulk_push_entry_t(kaitai::kstream* p__io, nano_t::msg_bulk_push_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_block = 0;
            _read();
        }

        void nano_t::msg_bulk_push_t::bulk_push_entry_t::_read() {
            m_block_type = m__io->read_u1();
            m_block = new block_selector_t(block_type(), m__io, this, m__root);
        }

        nano_t::msg_bulk_push_t::bulk_push_entry_t::~bulk_push_entry_t() {
            delete m_block;
        }

        nano_t::node_id_response_t::node_id_response_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::node_id_response_t::_read() {
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
        }

        nano_t::node_id_response_t::~node_id_response_t() {
        }

        nano_t::bulk_push_response_t::bulk_push_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::bulk_push_response_t::_read() {
        }

        nano_t::bulk_push_response_t::~bulk_push_response_t() {
        }

        nano_t::bulk_pull_response_t::bulk_pull_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;
            _read();
        }

        void nano_t::bulk_pull_response_t::_read() {
            m_entry = new std::vector<bulk_pull_entry_t*>();
            {
                int i = 0;
                bulk_pull_entry_t* _;
                do {
                    _ = new bulk_pull_entry_t(m__io, this, m__root);
                    m_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (entry()->at(i)->block_type() == nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK)) ));
            }
        }

        nano_t::bulk_pull_response_t::~bulk_pull_response_t() {
            for (std::vector<bulk_pull_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                delete *it;
            }
            delete m_entry;
        }

        nano_t::bulk_pull_response_t::bulk_pull_entry_t::bulk_pull_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_block = 0;
            _read();
        }

        void nano_t::bulk_pull_response_t::bulk_pull_entry_t::_read() {
            m_block_type = m__io->read_u1();
            m_block = new block_selector_t(block_type(), m__io, this, m__root);
        }

        nano_t::bulk_pull_response_t::bulk_pull_entry_t::~bulk_pull_entry_t() {
            delete m_block;
        }

        nano_t::msg_publish_t::msg_publish_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_body = 0;
            _read();
        }

        void nano_t::msg_publish_t::_read() {
            m_body = new block_selector_t(_root()->header()->block_type_int(), m__io, this, m__root);
        }

        nano_t::msg_publish_t::~msg_publish_t() {
            delete m_body;
        }

        nano_t::block_state_t::block_state_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::block_state_t::_read() {
            m_account = m__io->read_bytes(32);
            m_previous = m__io->read_bytes(32);
            m_representative = m__io->read_bytes(32);
            m_balance = m__io->read_bytes(16);
            m_link = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8be();
        }

        nano_t::block_state_t::~block_state_t() {
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_response_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_flags = p_flags;
            m_entry = 0;
            _read();
        }

        void nano_t::bulk_pull_account_response_t::_read() {
            m_entry = new std::vector<bulk_pull_account_entry_t*>();
            {
                int i = 0;
                bulk_pull_account_entry_t* _;
                do {
                    _ = new bulk_pull_account_entry_t(flags(), m__io, this, m__root);
                    m_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (entry()->at(i)->hash() == _root()->const_block_zero())) ));
            }
        }

        nano_t::bulk_pull_account_response_t::~bulk_pull_account_response_t() {
            for (std::vector<bulk_pull_account_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                delete *it;
            }
            delete m_entry;
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::bulk_pull_account_entry_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_flags = p_flags;
            f_pending_address_only = false;
            f_pending_include_address = false;
            _read();
        }

        void nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::_read() {
            n_hash = true;
            if (!(pending_address_only())) {
                n_hash = false;
                m_hash = m__io->read_bytes(32);
            }
            n_amount = true;
            if (!(pending_address_only())) {
                n_amount = false;
                m_amount = m__io->read_bytes(16);
            }
            n_source = true;
            if ( ((!(pending_address_only())) || (pending_include_address())) ) {
                n_source = false;
                m_source = m__io->read_bytes(32);
            }
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::~bulk_pull_account_entry_t() {
            if (!n_hash) {
            }
            if (!n_amount) {
            }
            if (!n_source) {
            }
        }

        bool nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::pending_address_only() {
            if (f_pending_address_only)
                return m_pending_address_only;
            m_pending_address_only = flags() == nano_t::ENUM_BULK_PULL_ACCOUNT_PENDING_ADDRESS_ONLY;
            f_pending_address_only = true;
            return m_pending_address_only;
        }

        bool nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::pending_include_address() {
            if (f_pending_include_address)
                return m_pending_include_address;
            m_pending_include_address = flags() == nano_t::ENUM_BULK_PULL_ACCOUNT_PENDING_HASH_AMOUNT_AND_ADDRESS;
            f_pending_include_address = true;
            return m_pending_include_address;
        }

        nano_t::msg_keepalive_t::msg_keepalive_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_peers = 0;
            _read();
        }

        void nano_t::msg_keepalive_t::_read() {
            n_peers = true;
            if (!(_io()->is_eof())) {
                n_peers = false;
                m_peers = new std::vector<peer_t*>();
                {
                    int i = 0;
                    peer_t* _;
                    do {
                        _ = new peer_t(m__io, this, m__root);
                        m_peers->push_back(_);
                        i++;
                    } while (!( ((i == 8) || (_io()->is_eof())) ));
                }
            }
        }

        nano_t::msg_keepalive_t::~msg_keepalive_t() {
            if (!n_peers) {
                for (std::vector<peer_t*>::iterator it = m_peers->begin(); it != m_peers->end(); ++it) {
                    delete *it;
                }
                delete m_peers;
            }
        }

        nano_t::msg_confirm_req_t::msg_confirm_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_body = 0;
            _read();
        }

        void nano_t::msg_confirm_req_t::_read() {
            m_body = new block_selector_t(_root()->header()->block_type_int(), m__io, this, m__root);
        }

        nano_t::msg_confirm_req_t::~msg_confirm_req_t() {
            delete m_body;
        }

        nano_t::msg_bulk_pull_account_t::msg_bulk_pull_account_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::msg_bulk_pull_account_t::_read() {
            m_account = m__io->read_bytes(32);
            m_minimum_amount = m__io->read_bytes(16);
            m_flags = static_cast<nano_t::enum_bulk_pull_account_t>(m__io->read_u1());
        }

        nano_t::msg_bulk_pull_account_t::~msg_bulk_pull_account_t() {
        }

        nano_t::vote_by_hash_t::vote_by_hash_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_hashes = 0;
            _read();
        }

        void nano_t::vote_by_hash_t::_read() {
            n_hashes = true;
            if (!(_io()->is_eof())) {
                n_hashes = false;
                m_hashes = new std::vector<std::string>();
                {
                    int i = 0;
                    std::string _;
                    do {
                        _ = m__io->read_bytes(32);
                        m_hashes->push_back(_);
                        i++;
                    } while (!( ((i == 12) || (_io()->is_eof())) ));
                }
            }
        }

        nano_t::vote_by_hash_t::~vote_by_hash_t() {
            if (!n_hashes) {
                delete m_hashes;
            }
        }

        nano_t::vote_common_t::vote_common_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::vote_common_t::_read() {
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_sequence = m__io->read_u8le();
        }

        nano_t::vote_common_t::~vote_common_t() {
        }

        nano_t::message_header_t::message_header_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            f_block_type_int = false;
            f_block_type = false;
            f_query_flag = false;
            f_response_flag = false;
            _read();
        }

        void nano_t::message_header_t::_read() {
            m_magic = m__io->ensure_fixed_contents(std::string("\x52", 1));
            m_network_id = static_cast<nano_t::enum_network_t>(m__io->read_u1());
            m_version_max = m__io->read_u1();
            m_version_using = m__io->read_u1();
            m_version_min = m__io->read_u1();
            m_message_type = static_cast<nano_t::enum_msgtype_t>(m__io->read_u1());
            m_extensions = m__io->read_u2le();
        }

        nano_t::message_header_t::~message_header_t() {
        }

        int32_t nano_t::message_header_t::block_type_int() {
            if (f_block_type_int)
                return m_block_type_int;
            m_block_type_int = ((extensions() & 3840) >> 8);
            f_block_type_int = true;
            return m_block_type_int;
        }

        nano_t::enum_blocktype_t nano_t::message_header_t::block_type() {
            if (f_block_type)
                return m_block_type;
            m_block_type = static_cast<nano_t::enum_blocktype_t>(((extensions() & 3840) >> 8));
            f_block_type = true;
            return m_block_type;
        }

        int32_t nano_t::message_header_t::query_flag() {
            if (f_query_flag)
                return m_query_flag;
            m_query_flag = (extensions() & 1);
            f_query_flag = true;
            return m_query_flag;
        }

        int32_t nano_t::message_header_t::response_flag() {
            if (f_response_flag)
                return m_response_flag;
            m_response_flag = (extensions() & 2);
            f_response_flag = true;
            return m_response_flag;
        }

        nano_t::frontier_response_t::frontier_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;
            _read();
        }

        void nano_t::frontier_response_t::_read() {
            m_entry = new std::vector<frontier_entry_t*>();
            {
                int i = 0;
                frontier_entry_t* _;
                do {
                    _ = new frontier_entry_t(m__io, this, m__root);
                    m_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (entry()->at(i)->frontier_hash() == _root()->const_block_zero())) ));
            }
        }

        nano_t::frontier_response_t::~frontier_response_t() {
            for (std::vector<frontier_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                delete *it;
            }
            delete m_entry;
        }

        nano_t::frontier_response_t::frontier_entry_t::frontier_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            _read();
        }

        void nano_t::frontier_response_t::frontier_entry_t::_read() {
            m_account = m__io->read_bytes(32);
            m_frontier_hash = m__io->read_bytes(32);
        }

        nano_t::frontier_response_t::frontier_entry_t::~frontier_entry_t() {
        }

        std::string nano_t::const_block_zero() {
            if (f_const_block_zero)
                return m_const_block_zero;
            m_const_block_zero = m__io->ensure_fixed_contents(std::string("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 32));
            f_const_block_zero = true;
            return m_const_block_zero;
        }
    }
}
