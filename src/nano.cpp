// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild
#include "nano.h"

namespace nano {
    namespace protocol {

        nano_t::nano_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = this;
            m_header = 0;
            f_const_block_zero = false;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            case nano_t::ENUM_MSGTYPE_ASC_PULL_ACK: {
                m_body = new msg_asc_pull_ack_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_BULK_PUSH: {
                m_body = new msg_bulk_push_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_TELEMETRY_ACK: {
                m_body = new msg_telemetry_ack_t(m__io, this, m__root);
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
            case nano_t::ENUM_MSGTYPE_PUBLISH: {
                m_body = new msg_publish_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_TELEMETRY_REQ: {
                m_body = new msg_telemetry_req_t(m__io, this, m__root);
                break;
            }
            case nano_t::ENUM_MSGTYPE_ASC_PULL_REQ: {
                m_body = new msg_asc_pull_req_t(m__io, this, m__root);
                break;
            }
            default: {
                m_body = new ignore_until_eof_t(m__io, this, m__root);
                break;
            }
            }
        }

        nano_t::~nano_t() {
            _clean_up();
        }

        void nano_t::_clean_up() {
            if (m_header) {
                delete m_header; m_header = 0;
            }
            if (m_body) {
                delete m_body; m_body = 0;
            }
            if (f_const_block_zero) {
            }
        }

        nano_t::block_send_t::block_send_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::block_send_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_destination = m__io->read_bytes(32);
            m_balance = m__io->read_bytes(16);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_send_t::~block_send_t() {
            _clean_up();
        }

        void nano_t::block_send_t::_clean_up() {
        }

        nano_t::confirm_request_by_hash_t::confirm_request_by_hash_t(kaitai::kstream* p__io, nano_t::msg_confirm_req_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_pairs = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::confirm_request_by_hash_t::_read() {
            n_pairs = true;
            if (!(_io()->is_eof())) {
                n_pairs = false;
                m_pairs = new std::vector<hash_pair_t*>();
                {
                    int i = 0;
                    hash_pair_t* _;
                    do {
                        _ = new hash_pair_t(m__io, this, m__root);
                        m_pairs->push_back(_);
                        i++;
                    } while (!( ((i == _root()->header()->item_count_int()) || (_io()->is_eof())) ));
                }
            }
        }

        nano_t::confirm_request_by_hash_t::~confirm_request_by_hash_t() {
            _clean_up();
        }

        void nano_t::confirm_request_by_hash_t::_clean_up() {
            if (!n_pairs) {
                if (m_pairs) {
                    for (std::vector<hash_pair_t*>::iterator it = m_pairs->begin(); it != m_pairs->end(); ++it) {
                        delete *it;
                    }
                    delete m_pairs; m_pairs = 0;
                }
            }
        }

        nano_t::msg_telemetry_req_t::msg_telemetry_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_telemetry_req_t::_read() {
        }

        nano_t::msg_telemetry_req_t::~msg_telemetry_req_t() {
            _clean_up();
        }

        void nano_t::msg_telemetry_req_t::_clean_up() {
        }

        nano_t::block_selector_t::block_selector_t(uint8_t p_arg_block_type, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_arg_block_type = p_arg_block_type;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::block_selector_t::_clean_up() {
            if (m_block) {
                delete m_block; m_block = 0;
            }
        }

        nano_t::node_id_query_t::node_id_query_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::node_id_query_t::_read() {
            m_cookie = m__io->read_bytes(32);
        }

        nano_t::node_id_query_t::~node_id_query_t() {
            _clean_up();
        }

        void nano_t::node_id_query_t::_clean_up() {
        }

        nano_t::block_receive_t::block_receive_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::block_receive_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_source = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_receive_t::~block_receive_t() {
            _clean_up();
        }

        void nano_t::block_receive_t::_clean_up() {
        }

        nano_t::block_change_t::block_change_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::block_change_t::_read() {
            m_previous = m__io->read_bytes(32);
            m_representative = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_change_t::~block_change_t() {
            _clean_up();
        }

        void nano_t::block_change_t::_clean_up() {
        }

        nano_t::msg_bulk_pull_t::msg_bulk_pull_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_extended = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_bulk_pull_t::_read() {
            m_start = m__io->read_bytes(32);
            m_end = m__io->read_bytes(32);
            n_extended = true;
            if (_root()->header()->extended_params_present() != 0) {
                n_extended = false;
                m_extended = new extended_parameters_t(m__io, this, m__root);
            }
        }

        nano_t::msg_bulk_pull_t::~msg_bulk_pull_t() {
            _clean_up();
        }

        void nano_t::msg_bulk_pull_t::_clean_up() {
            if (!n_extended) {
                if (m_extended) {
                    delete m_extended; m_extended = 0;
                }
            }
        }

        nano_t::msg_bulk_pull_t::extended_parameters_t::extended_parameters_t(kaitai::kstream* p__io, nano_t::msg_bulk_pull_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_bulk_pull_t::extended_parameters_t::_read() {
            m_zero = m__io->read_u1();
            m_count = m__io->read_u4le();
            m_reserved = m__io->read_bytes(3);
        }

        nano_t::msg_bulk_pull_t::extended_parameters_t::~extended_parameters_t() {
            _clean_up();
        }

        void nano_t::msg_bulk_pull_t::extended_parameters_t::_clean_up() {
        }

        nano_t::peer_t::peer_t(kaitai::kstream* p__io, nano_t::msg_keepalive_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::peer_t::_read() {
            m_address = m__io->read_bytes(16);
            m_port = m__io->read_u2le();
        }

        nano_t::peer_t::~peer_t() {
            _clean_up();
        }

        void nano_t::peer_t::_clean_up() {
        }

        nano_t::asc_pull_base_t::asc_pull_base_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::asc_pull_base_t::_read() {
            m_type = static_cast<nano_t::enum_asc_pull_type_t>(m__io->read_u1());
            m_id = m__io->read_u8be();
        }

        nano_t::asc_pull_base_t::~asc_pull_base_t() {
            _clean_up();
        }

        void nano_t::asc_pull_base_t::_clean_up() {
        }

        nano_t::block_open_t::block_open_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::block_open_t::_read() {
            m_source = m__io->read_bytes(32);
            m_representative = m__io->read_bytes(32);
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_work = m__io->read_u8le();
        }

        nano_t::block_open_t::~block_open_t() {
            _clean_up();
        }

        void nano_t::block_open_t::_clean_up() {
        }

        nano_t::ignore_until_eof_t::ignore_until_eof_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_empty = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::ignore_until_eof_t::_clean_up() {
            if (!n_empty) {
                if (m_empty) {
                    delete m_empty; m_empty = 0;
                }
            }
        }

        nano_t::msg_confirm_ack_t::msg_confirm_ack_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_common = 0;
            m_votebyhash = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_confirm_ack_t::_read() {
            m_common = new vote_common_t(m__io, this, m__root);
            m_votebyhash = new vote_by_hash_t(m__io, this, m__root);
        }

        nano_t::msg_confirm_ack_t::~msg_confirm_ack_t() {
            _clean_up();
        }

        void nano_t::msg_confirm_ack_t::_clean_up() {
            if (m_common) {
                delete m_common; m_common = 0;
            }
            if (m_votebyhash) {
                delete m_votebyhash; m_votebyhash = 0;
            }
        }

        nano_t::msg_node_id_handshake_t::msg_node_id_handshake_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_query = 0;
            m_response = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_node_id_handshake_t::_read() {
            n_query = true;
            if (_root()->header()->query_flag() != 0) {
                n_query = false;
                m_query = new node_id_query_t(m__io, this, m__root);
            }
            n_response = true;
            if (_root()->header()->response_flag() != 0) {
                n_response = false;
                m_response = new node_id_response_t(m__io, this, m__root);
            }
        }

        nano_t::msg_node_id_handshake_t::~msg_node_id_handshake_t() {
            _clean_up();
        }

        void nano_t::msg_node_id_handshake_t::_clean_up() {
            if (!n_query) {
                if (m_query) {
                    delete m_query; m_query = 0;
                }
            }
            if (!n_response) {
                if (m_response) {
                    delete m_response; m_response = 0;
                }
            }
        }

        nano_t::msg_frontier_req_t::msg_frontier_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_frontier_req_t::_read() {
            m_start = m__io->read_bytes(32);
            m_age = m__io->read_u4le();
            m_count = m__io->read_u4le();
        }

        nano_t::msg_frontier_req_t::~msg_frontier_req_t() {
            _clean_up();
        }

        void nano_t::msg_frontier_req_t::_clean_up() {
        }

        nano_t::msg_bulk_push_t::msg_bulk_push_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::msg_bulk_push_t::_clean_up() {
            if (m_entry) {
                for (std::vector<bulk_push_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                    delete *it;
                }
                delete m_entry; m_entry = 0;
            }
        }

        nano_t::msg_bulk_push_t::bulk_push_entry_t::bulk_push_entry_t(kaitai::kstream* p__io, nano_t::msg_bulk_push_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_block = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_bulk_push_t::bulk_push_entry_t::_read() {
            m_block_type = m__io->read_u1();
            m_block = new block_selector_t(block_type(), m__io, this, m__root);
        }

        nano_t::msg_bulk_push_t::bulk_push_entry_t::~bulk_push_entry_t() {
            _clean_up();
        }

        void nano_t::msg_bulk_push_t::bulk_push_entry_t::_clean_up() {
            if (m_block) {
                delete m_block; m_block = 0;
            }
        }

        nano_t::node_id_response_t::node_id_response_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::node_id_response_t::_read() {
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
        }

        nano_t::node_id_response_t::~node_id_response_t() {
            _clean_up();
        }

        void nano_t::node_id_response_t::_clean_up() {
        }

        nano_t::msg_asc_pull_ack_t::msg_asc_pull_ack_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_base = 0;
            m_payload = 0;
            m__io__raw_payload = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_ack_t::_read() {
            m_base = new asc_pull_base_t(m__io, this, m__root);
            m__raw_payload = m__io->read_bytes(_root()->header()->asc_pull_size());
            m__io__raw_payload = new kaitai::kstream(m__raw_payload);
            m_payload = new asc_pull_ack_payload_t(m__io__raw_payload, this, m__root);
        }

        nano_t::msg_asc_pull_ack_t::~msg_asc_pull_ack_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_ack_t::_clean_up() {
            if (m_base) {
                delete m_base; m_base = 0;
            }
            if (m__io__raw_payload) {
                delete m__io__raw_payload; m__io__raw_payload = 0;
            }
            if (m_payload) {
                delete m_payload; m_payload = 0;
            }
        }

        nano_t::msg_asc_pull_ack_t::account_info_ack_payload_t::account_info_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_ack_t::account_info_ack_payload_t::_read() {
            m_account = m__io->read_bytes(32);
            m_account_open = m__io->read_bytes(32);
            m_account_head = m__io->read_bytes(32);
            m_block_count = m__io->read_u8be();
            m_conf_frontier = m__io->read_bytes(32);
            m_conf_height = m__io->read_u8be();
        }

        nano_t::msg_asc_pull_ack_t::account_info_ack_payload_t::~account_info_ack_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_ack_t::account_info_ack_payload_t::_clean_up() {
        }

        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::blocks_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::_read() {
            m_entry = new std::vector<asc_pull_entry_t*>();
            {
                int i = 0;
                asc_pull_entry_t* _;
                do {
                    _ = new asc_pull_entry_t(m__io, this, m__root);
                    m_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (_->block_type() == nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK)) ));
            }
        }

        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::~blocks_ack_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::_clean_up() {
            if (m_entry) {
                for (std::vector<asc_pull_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                    delete *it;
                }
                delete m_entry; m_entry = 0;
            }
        }

        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::asc_pull_entry_t::asc_pull_entry_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_block = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::asc_pull_entry_t::_read() {
            m_block_type = m__io->read_u1();
            m_block = new block_selector_t(block_type(), m__io, this, m__root);
        }

        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::asc_pull_entry_t::~asc_pull_entry_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t::asc_pull_entry_t::_clean_up() {
            if (m_block) {
                delete m_block; m_block = 0;
            }
        }

        nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t::asc_pull_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_account_info = 0;
            m_blocks = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t::_read() {
            n_account_info = true;
            if (_parent()->base()->type() == nano_t::ENUM_ASC_PULL_TYPE_ACCOUNT_PULL_TYPE) {
                n_account_info = false;
                m_account_info = new account_info_ack_payload_t(m__io, this, m__root);
            }
            n_blocks = true;
            if (_parent()->base()->type() == nano_t::ENUM_ASC_PULL_TYPE_BLOCK_PULL_TYPE) {
                n_blocks = false;
                m_blocks = new blocks_ack_payload_t(m__io, this, m__root);
            }
        }

        nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t::~asc_pull_ack_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t::_clean_up() {
            if (!n_account_info) {
                if (m_account_info) {
                    delete m_account_info; m_account_info = 0;
                }
            }
            if (!n_blocks) {
                if (m_blocks) {
                    delete m_blocks; m_blocks = 0;
                }
            }
        }

        nano_t::bulk_push_response_t::bulk_push_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::bulk_push_response_t::_read() {
        }

        nano_t::bulk_push_response_t::~bulk_push_response_t() {
            _clean_up();
        }

        void nano_t::bulk_push_response_t::_clean_up() {
        }

        nano_t::bulk_pull_response_t::bulk_pull_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::bulk_pull_response_t::_clean_up() {
            if (m_entry) {
                for (std::vector<bulk_pull_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                    delete *it;
                }
                delete m_entry; m_entry = 0;
            }
        }

        nano_t::bulk_pull_response_t::bulk_pull_entry_t::bulk_pull_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_block = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::bulk_pull_response_t::bulk_pull_entry_t::_read() {
            m_block_type = m__io->read_u1();
            m_block = new block_selector_t(block_type(), m__io, this, m__root);
        }

        nano_t::bulk_pull_response_t::bulk_pull_entry_t::~bulk_pull_entry_t() {
            _clean_up();
        }

        void nano_t::bulk_pull_response_t::bulk_pull_entry_t::_clean_up() {
            if (m_block) {
                delete m_block; m_block = 0;
            }
        }

        nano_t::msg_publish_t::msg_publish_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_body = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_publish_t::_read() {
            m_body = new block_selector_t(_root()->header()->block_type_int(), m__io, this, m__root);
        }

        nano_t::msg_publish_t::~msg_publish_t() {
            _clean_up();
        }

        void nano_t::msg_publish_t::_clean_up() {
            if (m_body) {
                delete m_body; m_body = 0;
            }
        }

        nano_t::block_state_t::block_state_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::block_state_t::_clean_up() {
        }

        nano_t::hash_pair_t::hash_pair_t(kaitai::kstream* p__io, nano_t::confirm_request_by_hash_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::hash_pair_t::_read() {
            m_first = m__io->read_bytes(32);
            m_second = m__io->read_bytes(32);
        }

        nano_t::hash_pair_t::~hash_pair_t() {
            _clean_up();
        }

        void nano_t::hash_pair_t::_clean_up() {
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_response_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_flags = p_flags;
            m_frontier_entry = 0;
            m_pending_entry = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::bulk_pull_account_response_t::_read() {
            m_frontier_entry = new frontier_balance_entry_t(m__io, this, m__root);
            m_pending_entry = new std::vector<bulk_pull_account_entry_t*>();
            {
                int i = 0;
                bulk_pull_account_entry_t* _;
                do {
                    _ = new bulk_pull_account_entry_t(flags(), m__io, this, m__root);
                    m_pending_entry->push_back(_);
                    i++;
                } while (!( ((_io()->is_eof()) || (pending_entry()->at(i)->hash() == _root()->const_block_zero())) ));
            }
        }

        nano_t::bulk_pull_account_response_t::~bulk_pull_account_response_t() {
            _clean_up();
        }

        void nano_t::bulk_pull_account_response_t::_clean_up() {
            if (m_frontier_entry) {
                delete m_frontier_entry; m_frontier_entry = 0;
            }
            if (m_pending_entry) {
                for (std::vector<bulk_pull_account_entry_t*>::iterator it = m_pending_entry->begin(); it != m_pending_entry->end(); ++it) {
                    delete *it;
                }
                delete m_pending_entry; m_pending_entry = 0;
            }
        }

        nano_t::bulk_pull_account_response_t::frontier_balance_entry_t::frontier_balance_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::bulk_pull_account_response_t::frontier_balance_entry_t::_read() {
            m_frontier_hash = m__io->read_bytes(32);
            m_balance = m__io->read_bytes(16);
        }

        nano_t::bulk_pull_account_response_t::frontier_balance_entry_t::~frontier_balance_entry_t() {
            _clean_up();
        }

        void nano_t::bulk_pull_account_response_t::frontier_balance_entry_t::_clean_up() {
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::bulk_pull_account_entry_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_flags = p_flags;
            f_pending_address_only = false;
            f_pending_include_address = false;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            if ( ((pending_address_only()) || (pending_include_address())) ) {
                n_source = false;
                m_source = m__io->read_bytes(32);
            }
        }

        nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::~bulk_pull_account_entry_t() {
            _clean_up();
        }

        void nano_t::bulk_pull_account_response_t::bulk_pull_account_entry_t::_clean_up() {
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

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::msg_keepalive_t::_clean_up() {
            if (!n_peers) {
                if (m_peers) {
                    for (std::vector<peer_t*>::iterator it = m_peers->begin(); it != m_peers->end(); ++it) {
                        delete *it;
                    }
                    delete m_peers; m_peers = 0;
                }
            }
        }

        nano_t::msg_confirm_req_t::msg_confirm_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_reqbyhash = 0;
            m_block = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_confirm_req_t::_read() {
            n_reqbyhash = true;
            if (_root()->header()->block_type() == nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK) {
                n_reqbyhash = false;
                m_reqbyhash = new confirm_request_by_hash_t(m__io, this, m__root);
            }
            n_block = true;
            if (_root()->header()->block_type() != nano_t::ENUM_BLOCKTYPE_NOT_A_BLOCK) {
                n_block = false;
                m_block = new block_selector_t(_root()->header()->block_type_int(), m__io, this, m__root);
            }
        }

        nano_t::msg_confirm_req_t::~msg_confirm_req_t() {
            _clean_up();
        }

        void nano_t::msg_confirm_req_t::_clean_up() {
            if (!n_reqbyhash) {
                if (m_reqbyhash) {
                    delete m_reqbyhash; m_reqbyhash = 0;
                }
            }
            if (!n_block) {
                if (m_block) {
                    delete m_block; m_block = 0;
                }
            }
        }

        nano_t::msg_bulk_pull_account_t::msg_bulk_pull_account_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_bulk_pull_account_t::_read() {
            m_account = m__io->read_bytes(32);
            m_minimum_amount = m__io->read_bytes(16);
            m_flags = static_cast<nano_t::enum_bulk_pull_account_t>(m__io->read_u1());
        }

        nano_t::msg_bulk_pull_account_t::~msg_bulk_pull_account_t() {
            _clean_up();
        }

        void nano_t::msg_bulk_pull_account_t::_clean_up() {
        }

        nano_t::vote_by_hash_t::vote_by_hash_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_hashes = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
                    } while (!( ((i == _root()->header()->item_count_int()) || (_io()->is_eof())) ));
                }
            }
        }

        nano_t::vote_by_hash_t::~vote_by_hash_t() {
            _clean_up();
        }

        void nano_t::vote_by_hash_t::_clean_up() {
            if (!n_hashes) {
                if (m_hashes) {
                    delete m_hashes; m_hashes = 0;
                }
            }
        }

        nano_t::vote_common_t::vote_common_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            f_timestamp = false;
            f_vote_duration_bits = false;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::vote_common_t::_read() {
            m_account = m__io->read_bytes(32);
            m_signature = m__io->read_bytes(64);
            m_timestamp_and_vote_duration = m__io->read_u8le();
        }

        nano_t::vote_common_t::~vote_common_t() {
            _clean_up();
        }

        void nano_t::vote_common_t::_clean_up() {
        }

        int32_t nano_t::vote_common_t::timestamp() {
            if (f_timestamp)
                return m_timestamp;
            m_timestamp = (timestamp_and_vote_duration() & 18446744073709551600ULL);
            f_timestamp = true;
            return m_timestamp;
        }

        int32_t nano_t::vote_common_t::vote_duration_bits() {
            if (f_vote_duration_bits)
                return m_vote_duration_bits;
            m_vote_duration_bits = (timestamp_and_vote_duration() & 15);
            f_vote_duration_bits = true;
            return m_vote_duration_bits;
        }

        nano_t::message_header_t::message_header_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            f_bulk_pull_ascending_flag = false;
            f_response_flag = false;
            f_block_type_int = false;
            f_telemetry_size = false;
            f_extended_params_present = false;
            f_block_type = false;
            f_item_count_int = false;
            f_query_flag = false;
            f_confirmed_present = false;
            f_asc_pull_size = false;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::message_header_t::_clean_up() {
        }

        int32_t nano_t::message_header_t::bulk_pull_ascending_flag() {
            if (f_bulk_pull_ascending_flag)
                return m_bulk_pull_ascending_flag;
            m_bulk_pull_ascending_flag = (extensions() & 2);
            f_bulk_pull_ascending_flag = true;
            return m_bulk_pull_ascending_flag;
        }

        int32_t nano_t::message_header_t::response_flag() {
            if (f_response_flag)
                return m_response_flag;
            m_response_flag = (extensions() & 2);
            f_response_flag = true;
            return m_response_flag;
        }

        int32_t nano_t::message_header_t::block_type_int() {
            if (f_block_type_int)
                return m_block_type_int;
            m_block_type_int = ((extensions() & 3840) >> 8);
            f_block_type_int = true;
            return m_block_type_int;
        }

        int32_t nano_t::message_header_t::telemetry_size() {
            if (f_telemetry_size)
                return m_telemetry_size;
            m_telemetry_size = (extensions() & 1023);
            f_telemetry_size = true;
            return m_telemetry_size;
        }

        int32_t nano_t::message_header_t::extended_params_present() {
            if (f_extended_params_present)
                return m_extended_params_present;
            m_extended_params_present = (extensions() & 1);
            f_extended_params_present = true;
            return m_extended_params_present;
        }

        nano_t::enum_blocktype_t nano_t::message_header_t::block_type() {
            if (f_block_type)
                return m_block_type;
            m_block_type = static_cast<nano_t::enum_blocktype_t>(((extensions() & 3840) >> 8));
            f_block_type = true;
            return m_block_type;
        }

        int32_t nano_t::message_header_t::item_count_int() {
            if (f_item_count_int)
                return m_item_count_int;
            m_item_count_int = ((extensions() & 61440) >> 12);
            f_item_count_int = true;
            return m_item_count_int;
        }

        int32_t nano_t::message_header_t::query_flag() {
            if (f_query_flag)
                return m_query_flag;
            m_query_flag = (extensions() & 1);
            f_query_flag = true;
            return m_query_flag;
        }

        int32_t nano_t::message_header_t::confirmed_present() {
            if (f_confirmed_present)
                return m_confirmed_present;
            m_confirmed_present = (extensions() & 2);
            f_confirmed_present = true;
            return m_confirmed_present;
        }

        int32_t nano_t::message_header_t::asc_pull_size() {
            if (f_asc_pull_size)
                return m_asc_pull_size;
            m_asc_pull_size = (extensions() & 65535);
            f_asc_pull_size = true;
            return m_asc_pull_size;
        }

        nano_t::frontier_response_t::frontier_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_entry = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
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
            _clean_up();
        }

        void nano_t::frontier_response_t::_clean_up() {
            if (m_entry) {
                for (std::vector<frontier_entry_t*>::iterator it = m_entry->begin(); it != m_entry->end(); ++it) {
                    delete *it;
                }
                delete m_entry; m_entry = 0;
            }
        }

        nano_t::frontier_response_t::frontier_entry_t::frontier_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::frontier_response_t::frontier_entry_t::_read() {
            m_account = m__io->read_bytes(32);
            m_frontier_hash = m__io->read_bytes(32);
        }

        nano_t::frontier_response_t::frontier_entry_t::~frontier_entry_t() {
            _clean_up();
        }

        void nano_t::frontier_response_t::frontier_entry_t::_clean_up() {
        }

        nano_t::msg_asc_pull_req_t::msg_asc_pull_req_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_base = 0;
            m_payload = 0;
            m__io__raw_payload = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_req_t::_read() {
            m_base = new asc_pull_base_t(m__io, this, m__root);
            m__raw_payload = m__io->read_bytes(_root()->header()->asc_pull_size());
            m__io__raw_payload = new kaitai::kstream(m__raw_payload);
            m_payload = new asc_pull_req_payload_t(m__io__raw_payload, this, m__root);
        }

        nano_t::msg_asc_pull_req_t::~msg_asc_pull_req_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_req_t::_clean_up() {
            if (m_base) {
                delete m_base; m_base = 0;
            }
            if (m__io__raw_payload) {
                delete m__io__raw_payload; m__io__raw_payload = 0;
            }
            if (m_payload) {
                delete m_payload; m_payload = 0;
            }
        }

        nano_t::msg_asc_pull_req_t::account_info_req_payload_t::account_info_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_req_t::account_info_req_payload_t::_read() {
            m_start = m__io->read_bytes(32);
            m_start_type = static_cast<nano_t::enum_asc_hash_type_t>(m__io->read_u1());
        }

        nano_t::msg_asc_pull_req_t::account_info_req_payload_t::~account_info_req_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_req_t::account_info_req_payload_t::_clean_up() {
        }

        nano_t::msg_asc_pull_req_t::blocks_req_payload_t::blocks_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_req_t::blocks_req_payload_t::_read() {
            m_start = m__io->read_bytes(32);
            m_count = m__io->read_u1();
            m_start_type = static_cast<nano_t::enum_asc_hash_type_t>(m__io->read_u1());
        }

        nano_t::msg_asc_pull_req_t::blocks_req_payload_t::~blocks_req_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_req_t::blocks_req_payload_t::_clean_up() {
        }

        nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t::asc_pull_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_account_info = 0;
            m_blocks = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t::_read() {
            n_account_info = true;
            if (_parent()->base()->type() == nano_t::ENUM_ASC_PULL_TYPE_ACCOUNT_PULL_TYPE) {
                n_account_info = false;
                m_account_info = new account_info_req_payload_t(m__io, this, m__root);
            }
            n_blocks = true;
            if (_parent()->base()->type() == nano_t::ENUM_ASC_PULL_TYPE_BLOCK_PULL_TYPE) {
                n_blocks = false;
                m_blocks = new blocks_req_payload_t(m__io, this, m__root);
            }
        }

        nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t::~asc_pull_req_payload_t() {
            _clean_up();
        }

        void nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t::_clean_up() {
            if (!n_account_info) {
                if (m_account_info) {
                    delete m_account_info; m_account_info = 0;
                }
            }
            if (!n_blocks) {
                if (m_blocks) {
                    delete m_blocks; m_blocks = 0;
                }
            }
        }

        nano_t::msg_telemetry_ack_t::msg_telemetry_ack_t(kaitai::kstream* p__io, nano_t* p__parent, nano_t* p__root) : kaitai::kstruct(p__io) {
            m__parent = p__parent;
            m__root = p__root;
            m_unknown_data = 0;

            try {
                _read();
            } catch(...) {
                _clean_up();
                throw;
            }
        }

        void nano_t::msg_telemetry_ack_t::_read() {
            m_signature = m__io->read_bytes(64);
            m_nodeid = m__io->read_bytes(32);
            m_blockcount = m__io->read_u8be();
            m_cementedcount = m__io->read_u8be();
            m_uncheckedcount = m__io->read_u8be();
            m_accountcount = m__io->read_u8be();
            m_bandwidthcap = m__io->read_u8be();
            m_peercount = m__io->read_u4be();
            m_protocolversion = m__io->read_u1();
            m_uptime = m__io->read_u8be();
            m_genesisblock = m__io->read_bytes(32);
            m_majorversion = m__io->read_u1();
            m_minorversion = m__io->read_u1();
            m_patchversion = m__io->read_u1();
            m_prereleaseversion = m__io->read_u1();
            m_maker = m__io->read_u1();
            m_timestamp = m__io->read_u8be();
            m_activedifficulty = m__io->read_u8be();
            n_unknown_data = true;
            if (_io()->pos() < _root()->header()->telemetry_size()) {
                n_unknown_data = false;
                m_unknown_data = new std::vector<uint64_t>();
                {
                    int i = 0;
                    uint64_t _;
                    do {
                        _ = m__io->read_u8le();
                        m_unknown_data->push_back(_);
                        i++;
                    } while (!(_io()->pos() == _root()->header()->telemetry_size()));
                }
            }
        }

        nano_t::msg_telemetry_ack_t::~msg_telemetry_ack_t() {
            _clean_up();
        }

        void nano_t::msg_telemetry_ack_t::_clean_up() {
            if (!n_unknown_data) {
                if (m_unknown_data) {
                    delete m_unknown_data; m_unknown_data = 0;
                }
            }
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