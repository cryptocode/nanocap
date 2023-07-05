#ifndef NANO_H_
#define NANO_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif
namespace nano {
    namespace protocol {

        class nano_t : public kaitai::kstruct {

        public:
            class block_send_t;
            class confirm_request_by_hash_t;
            class msg_telemetry_req_t;
            class block_selector_t;
            class node_id_query_t;
            class block_receive_t;
            class block_change_t;
            class msg_bulk_pull_t;
            class peer_t;
            class asc_pull_base_t;
            class block_open_t;
            class ignore_until_eof_t;
            class msg_confirm_ack_t;
            class msg_node_id_handshake_t;
            class msg_frontier_req_t;
            class msg_bulk_push_t;
            class node_id_response_t;
            class msg_asc_pull_ack_t;
            class bulk_push_response_t;
            class bulk_pull_response_t;
            class msg_publish_t;
            class block_state_t;
            class hash_pair_t;
            class bulk_pull_account_response_t;
            class msg_keepalive_t;
            class msg_confirm_req_t;
            class msg_bulk_pull_account_t;
            class vote_by_hash_t;
            class vote_common_t;
            class message_header_t;
            class frontier_response_t;
            class msg_asc_pull_req_t;
            class msg_telemetry_ack_t;

            enum enum_blocktype_t {
                ENUM_BLOCKTYPE_INVALID = 0,
                ENUM_BLOCKTYPE_NOT_A_BLOCK = 1,
                ENUM_BLOCKTYPE_SEND = 2,
                ENUM_BLOCKTYPE_RECEIVE = 3,
                ENUM_BLOCKTYPE_OPEN = 4,
                ENUM_BLOCKTYPE_CHANGE = 5,
                ENUM_BLOCKTYPE_STATE = 6
            };

            enum enum_msgtype_t {
                ENUM_MSGTYPE_INVALID = 0,
                ENUM_MSGTYPE_NOT_A_TYPE = 1,
                ENUM_MSGTYPE_KEEPALIVE = 2,
                ENUM_MSGTYPE_PUBLISH = 3,
                ENUM_MSGTYPE_CONFIRM_REQ = 4,
                ENUM_MSGTYPE_CONFIRM_ACK = 5,
                ENUM_MSGTYPE_BULK_PULL = 6,
                ENUM_MSGTYPE_BULK_PUSH = 7,
                ENUM_MSGTYPE_FRONTIER_REQ = 8,
                ENUM_MSGTYPE_NODE_ID_HANDSHAKE = 10,
                ENUM_MSGTYPE_BULK_PULL_ACCOUNT = 11,
                ENUM_MSGTYPE_TELEMETRY_REQ = 12,
                ENUM_MSGTYPE_TELEMETRY_ACK = 13,
                ENUM_MSGTYPE_ASC_PULL_REQ = 14,
                ENUM_MSGTYPE_ASC_PULL_ACK = 15
            };

            enum enum_network_t {
                ENUM_NETWORK_NETWORK_TEST = 65,
                ENUM_NETWORK_NETWORK_BETA = 66,
                ENUM_NETWORK_NETWORK_LIVE = 67
            };

            enum enum_asc_hash_type_t {
                ENUM_ASC_HASH_TYPE_ACCOUNT_HASH_TYPE = 0,
                ENUM_ASC_HASH_TYPE_BLOCK_HASH_TYPE = 1
            };

            enum enum_bulk_pull_account_t {
                ENUM_BULK_PULL_ACCOUNT_PENDING_HASH_AND_AMOUNT = 0,
                ENUM_BULK_PULL_ACCOUNT_PENDING_ADDRESS_ONLY = 1,
                ENUM_BULK_PULL_ACCOUNT_PENDING_HASH_AMOUNT_AND_ADDRESS = 2
            };

            enum protocol_version_t {
                PROTOCOL_VERSION_VALUE = 19
            };

            enum enum_asc_pull_type_t {
                ENUM_ASC_PULL_TYPE_INVALID = 0,
                ENUM_ASC_PULL_TYPE_BLOCK_PULL_TYPE = 1,
                ENUM_ASC_PULL_TYPE_ACCOUNT_PULL_TYPE = 2
            };

            nano_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

        private:
            void _read();
            void _clean_up();

        public:
            ~nano_t();

            class block_send_t : public kaitai::kstruct {

            public:

                block_send_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_send_t();

            private:
                std::string m_previous;
                std::string m_destination;
                std::string m_balance;
                std::string m_signature;
                uint64_t m_work;
                nano_t* m__root;
                nano_t::block_selector_t* m__parent;

            public:

                /**
                * Hash of the previous block
                */
                std::string previous() const { return m_previous; }

                /**
                * Public key of destination account
                */
                std::string destination() const { return m_destination; }

                /**
                * 128-bit big endian balance
                */
                std::string balance() const { return m_balance; }

                /**
                * ed25519-blake2b signature
                */
                std::string signature() const { return m_signature; }

                /**
                * Proof of work
                */
                uint64_t work() const { return m_work; }
                nano_t* _root() const { return m__root; }
                nano_t::block_selector_t* _parent() const { return m__parent; }
            };

            /**
            * A sequence of hash,root pairs
            */

            class confirm_request_by_hash_t : public kaitai::kstruct {

            public:

                confirm_request_by_hash_t(kaitai::kstream* p__io, nano_t::msg_confirm_req_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~confirm_request_by_hash_t();

            private:
                std::vector<hash_pair_t*>* m_pairs;
                bool n_pairs;

            public:
                bool _is_null_pairs() { pairs(); return n_pairs; };

            private:
                nano_t* m__root;
                nano_t::msg_confirm_req_t* m__parent;

            public:

                /**
                * Up to "count" pairs of hash (first) and root (second), where count is read from header.
                */
                std::vector<hash_pair_t*>* pairs() const { return m_pairs; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_confirm_req_t* _parent() const { return m__parent; }
            };

            /**
            * Request node telemetry metrics
            */

            class msg_telemetry_req_t : public kaitai::kstruct {

            public:

                msg_telemetry_req_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_telemetry_req_t();

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * Selects a block based on the argument
            */

            class block_selector_t : public kaitai::kstruct {

            public:

                block_selector_t(uint8_t p_arg_block_type, kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_selector_t();

            private:
                kaitai::kstruct* m_block;
                uint8_t m_arg_block_type;
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                kaitai::kstruct* block() const { return m_block; }
                uint8_t arg_block_type() const { return m_arg_block_type; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            class node_id_query_t : public kaitai::kstruct {

            public:

                node_id_query_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~node_id_query_t();

            private:
                std::string m_cookie;
                nano_t* m__root;
                nano_t::msg_node_id_handshake_t* m__parent;

            public:

                /**
                * Per-endpoint random number
                */
                std::string cookie() const { return m_cookie; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_node_id_handshake_t* _parent() const { return m__parent; }
            };

            class block_receive_t : public kaitai::kstruct {

            public:

                block_receive_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_receive_t();

            private:
                std::string m_previous;
                std::string m_source;
                std::string m_signature;
                uint64_t m_work;
                nano_t* m__root;
                nano_t::block_selector_t* m__parent;

            public:

                /**
                * Hash of the previous block
                */
                std::string previous() const { return m_previous; }

                /**
                * Hash of the source send block
                */
                std::string source() const { return m_source; }

                /**
                * ed25519-blake2b signature
                */
                std::string signature() const { return m_signature; }

                /**
                * Proof of work
                */
                uint64_t work() const { return m_work; }
                nano_t* _root() const { return m__root; }
                nano_t::block_selector_t* _parent() const { return m__parent; }
            };

            class block_change_t : public kaitai::kstruct {

            public:

                block_change_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_change_t();

            private:
                std::string m_previous;
                std::string m_representative;
                std::string m_signature;
                uint64_t m_work;
                nano_t* m__root;
                nano_t::block_selector_t* m__parent;

            public:

                /**
                * Hash of the previous block
                */
                std::string previous() const { return m_previous; }

                /**
                * Public key of new representative account
                */
                std::string representative() const { return m_representative; }

                /**
                * ed25519-blake2b signature
                */
                std::string signature() const { return m_signature; }

                /**
                * Proof of work
                */
                uint64_t work() const { return m_work; }
                nano_t* _root() const { return m__root; }
                nano_t::block_selector_t* _parent() const { return m__parent; }
            };

            /**
            * Bulk pull request.
            */

            class msg_bulk_pull_t : public kaitai::kstruct {

            public:
                class extended_parameters_t;

                msg_bulk_pull_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_bulk_pull_t();

                class extended_parameters_t : public kaitai::kstruct {

                public:

                    extended_parameters_t(kaitai::kstream* p__io, nano_t::msg_bulk_pull_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~extended_parameters_t();

                private:
                    uint8_t m_zero;
                    uint32_t m_count;
                    std::string m_reserved;
                    nano_t* m__root;
                    nano_t::msg_bulk_pull_t* m__parent;

                public:

                    /**
                    * Must be 0
                    */
                    uint8_t zero() const { return m_zero; }

                    /**
                    * little endian "count" parameter to limit the response set.
                    */
                    uint32_t count() const { return m_count; }

                    /**
                    * Reserved extended parameter bytes
                    */
                    std::string reserved() const { return m_reserved; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_bulk_pull_t* _parent() const { return m__parent; }
                };

            private:
                std::string m_start;
                std::string m_end;
                extended_parameters_t* m_extended;
                bool n_extended;

            public:
                bool _is_null_extended() { extended(); return n_extended; };

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:

                /**
                * Account public key or block hash.
                */
                std::string start() const { return m_start; }

                /**
                * End block hash. May be zero.
                */
                std::string end() const { return m_end; }
                extended_parameters_t* extended() const { return m_extended; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * A peer entry in the keepalive message
            */

            class peer_t : public kaitai::kstruct {

            public:

                peer_t(kaitai::kstream* p__io, nano_t::msg_keepalive_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~peer_t();

            private:
                std::string m_address;
                uint16_t m_port;
                nano_t* m__root;
                nano_t::msg_keepalive_t* m__parent;

            public:

                /**
                * ipv6 address, or ipv6-mapped ipv4 address.
                */
                std::string address() const { return m_address; }

                /**
                * Port number. Default port is 7075.
                */
                uint16_t port() const { return m_port; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_keepalive_t* _parent() const { return m__parent; }
            };

            /**
            * Ascending pull sub-header, data common in asc pull packets.
            */

            class asc_pull_base_t : public kaitai::kstruct {

            public:

                asc_pull_base_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~asc_pull_base_t();

            private:
                enum_asc_pull_type_t m_type;
                uint64_t m_id;
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                enum_asc_pull_type_t type() const { return m_type; }

                /**
                * An identifier to associate replies with requests
                */
                uint64_t id() const { return m_id; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            class block_open_t : public kaitai::kstruct {

            public:

                block_open_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_open_t();

            private:
                std::string m_source;
                std::string m_representative;
                std::string m_account;
                std::string m_signature;
                uint64_t m_work;
                nano_t* m__root;
                nano_t::block_selector_t* m__parent;

            public:

                /**
                * Hash of the source send block
                */
                std::string source() const { return m_source; }

                /**
                * Public key of initial representative account
                */
                std::string representative() const { return m_representative; }

                /**
                * Public key of account being opened
                */
                std::string account() const { return m_account; }

                /**
                * ed25519-blake2b signature
                */
                std::string signature() const { return m_signature; }

                /**
                * Proof of work
                */
                uint64_t work() const { return m_work; }
                nano_t* _root() const { return m__root; }
                nano_t::block_selector_t* _parent() const { return m__parent; }
            };

            class ignore_until_eof_t : public kaitai::kstruct {

            public:

                ignore_until_eof_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~ignore_until_eof_t();

            private:
                std::vector<uint8_t>* m_empty;
                bool n_empty;

            public:
                bool _is_null_empty() { empty(); return n_empty; };

            private:
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                std::vector<uint8_t>* empty() const { return m_empty; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            /**
            * Signed confirmation of a list of block hashes
            */

            class msg_confirm_ack_t : public kaitai::kstruct {

            public:

                msg_confirm_ack_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_confirm_ack_t();

            private:
                vote_common_t* m_common;
                vote_by_hash_t* m_votebyhash;
                nano_t* m__root;
                nano_t* m__parent;

            public:
                vote_common_t* common() const { return m_common; }
                vote_by_hash_t* votebyhash() const { return m_votebyhash; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * A node ID handshake request and/or response.
            */

            class msg_node_id_handshake_t : public kaitai::kstruct {

            public:

                msg_node_id_handshake_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_node_id_handshake_t();

            private:
                node_id_query_t* m_query;
                bool n_query;

            public:
                bool _is_null_query() { query(); return n_query; };

            private:
                node_id_response_t* m_response;
                bool n_response;

            public:
                bool _is_null_response() { response(); return n_response; };

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:
                node_id_query_t* query() const { return m_query; }
                node_id_response_t* response() const { return m_response; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * Request frontiers (account chain head blocks) from a remote node
            */

            class msg_frontier_req_t : public kaitai::kstruct {

            public:

                msg_frontier_req_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_frontier_req_t();

            private:
                std::string m_start;
                uint32_t m_age;
                uint32_t m_count;
                nano_t* m__root;
                nano_t* m__parent;

            public:

                /**
                * Public key of start account
                */
                std::string start() const { return m_start; }

                /**
                * Maximum age of included account. If 0xffffffff, turn off age filtering.
                */
                uint32_t age() const { return m_age; }

                /**
                * Maximum number of accounts to include. If 0xffffffff, turn off count limiting.
                */
                uint32_t count() const { return m_count; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * A bulk push is equivalent to an unsolicited bulk pull response.
            * If a node knows about an account a peer doesn't, the node sends
            * its local blocks for that account to the peer. The stream of
            * blocks ends with a sentinel block of type enum_blocktype::not_a_block.
            */

            class msg_bulk_push_t : public kaitai::kstruct {

            public:
                class bulk_push_entry_t;

                msg_bulk_push_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_bulk_push_t();

                class bulk_push_entry_t : public kaitai::kstruct {

                public:

                    bulk_push_entry_t(kaitai::kstream* p__io, nano_t::msg_bulk_push_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~bulk_push_entry_t();

                private:
                    uint8_t m_block_type;
                    block_selector_t* m_block;
                    nano_t* m__root;
                    nano_t::msg_bulk_push_t* m__parent;

                public:
                    uint8_t block_type() const { return m_block_type; }
                    block_selector_t* block() const { return m_block; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_bulk_push_t* _parent() const { return m__parent; }
                };

            private:
                std::vector<bulk_push_entry_t*>* m_entry;
                nano_t* m__root;
                nano_t* m__parent;

            public:
                std::vector<bulk_push_entry_t*>* entry() const { return m_entry; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            class node_id_response_t : public kaitai::kstruct {

            public:

                node_id_response_t(kaitai::kstream* p__io, nano_t::msg_node_id_handshake_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~node_id_response_t();

            private:
                std::string m_account;
                std::string m_signature;
                nano_t* m__root;
                nano_t::msg_node_id_handshake_t* m__parent;

            public:

                /**
                * Account (node id)
                */
                std::string account() const { return m_account; }

                /**
                * Signature
                */
                std::string signature() const { return m_signature; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_node_id_handshake_t* _parent() const { return m__parent; }
            };

            /**
            * ascending pull response
            */

            class msg_asc_pull_ack_t : public kaitai::kstruct {

            public:
                class account_info_ack_payload_t;
                class blocks_ack_payload_t;
                class asc_pull_ack_payload_t;

                msg_asc_pull_ack_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_asc_pull_ack_t();

                /**
                * ascend pull response account info payload
                */

                class account_info_ack_payload_t : public kaitai::kstruct {

                public:

                    account_info_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~account_info_ack_payload_t();

                private:
                    std::string m_account;
                    std::string m_account_open;
                    std::string m_account_head;
                    uint64_t m_block_count;
                    std::string m_conf_frontier;
                    uint64_t m_conf_height;
                    nano_t* m__root;
                    nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* m__parent;

                public:

                    /**
                    * account being pulled
                    */
                    std::string account() const { return m_account; }

                    /**
                    * opening block of account
                    */
                    std::string account_open() const { return m_account_open; }

                    /**
                    * block with highest height (can be confirmed or unconfirmed)
                    */
                    std::string account_head() const { return m_account_head; }

                    /**
                    * number of blocks in account (counts confirmed and unconfirmed blocks)
                    */
                    uint64_t block_count() const { return m_block_count; }

                    /**
                    * block hash of confirmation frontier, confirmed block with highest height
                    */
                    std::string conf_frontier() const { return m_conf_frontier; }

                    /**
                    * Height of highest confirmed block
                    */
                    uint64_t conf_height() const { return m_conf_height; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* _parent() const { return m__parent; }
                };

                /**
                * ascend pull response block payload
                */

                class blocks_ack_payload_t : public kaitai::kstruct {

                public:
                    class asc_pull_entry_t;

                    blocks_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~blocks_ack_payload_t();

                    class asc_pull_entry_t : public kaitai::kstruct {

                    public:

                        asc_pull_entry_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t* p__parent = 0, nano_t* p__root = 0);

                    private:
                        void _read();
                        void _clean_up();

                    public:
                        ~asc_pull_entry_t();

                    private:
                        uint8_t m_block_type;
                        block_selector_t* m_block;
                        nano_t* m__root;
                        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t* m__parent;

                    public:
                        uint8_t block_type() const { return m_block_type; }
                        block_selector_t* block() const { return m_block; }
                        nano_t* _root() const { return m__root; }
                        nano_t::msg_asc_pull_ack_t::blocks_ack_payload_t* _parent() const { return m__parent; }
                    };

                private:
                    std::vector<asc_pull_entry_t*>* m_entry;
                    nano_t* m__root;
                    nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* m__parent;

                public:
                    std::vector<asc_pull_entry_t*>* entry() const { return m_entry; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_ack_t::asc_pull_ack_payload_t* _parent() const { return m__parent; }
                };

                /**
                * payload of ascend pull responses
                */

                class asc_pull_ack_payload_t : public kaitai::kstruct {

                public:

                    asc_pull_ack_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_ack_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~asc_pull_ack_payload_t();

                private:
                    account_info_ack_payload_t* m_account_info;
                    bool n_account_info;

                public:
                    bool _is_null_account_info() { account_info(); return n_account_info; };

                private:
                    blocks_ack_payload_t* m_blocks;
                    bool n_blocks;

                public:
                    bool _is_null_blocks() { blocks(); return n_blocks; };

                private:
                    nano_t* m__root;
                    nano_t::msg_asc_pull_ack_t* m__parent;

                public:
                    account_info_ack_payload_t* account_info() const { return m_account_info; }
                    blocks_ack_payload_t* blocks() const { return m_blocks; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_ack_t* _parent() const { return m__parent; }
                };

            private:
                asc_pull_base_t* m_base;
                asc_pull_ack_payload_t* m_payload;
                nano_t* m__root;
                nano_t* m__parent;
                std::string m__raw_payload;
                kaitai::kstream* m__io__raw_payload;

            public:
                asc_pull_base_t* base() const { return m_base; }
                asc_pull_ack_payload_t* payload() const { return m_payload; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
                std::string _raw_payload() const { return m__raw_payload; }
                kaitai::kstream* _io__raw_payload() const { return m__io__raw_payload; }
            };

            /**
            * The msg_bulk_push request does not have a response.
            */

            class bulk_push_response_t : public kaitai::kstruct {

            public:

                bulk_push_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~bulk_push_response_t();

            private:
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            /**
            * Response of the msg_bulk_pull request.
            */

            class bulk_pull_response_t : public kaitai::kstruct {

            public:
                class bulk_pull_entry_t;

                bulk_pull_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~bulk_pull_response_t();

                class bulk_pull_entry_t : public kaitai::kstruct {

                public:

                    bulk_pull_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~bulk_pull_entry_t();

                private:
                    uint8_t m_block_type;
                    block_selector_t* m_block;
                    nano_t* m__root;
                    kaitai::kstruct* m__parent;

                public:
                    uint8_t block_type() const { return m_block_type; }
                    block_selector_t* block() const { return m_block; }
                    nano_t* _root() const { return m__root; }
                    kaitai::kstruct* _parent() const { return m__parent; }
                };

            private:
                std::vector<bulk_pull_entry_t*>* m_entry;
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                std::vector<bulk_pull_entry_t*>* entry() const { return m_entry; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            /**
            * Publish the given block
            */

            class msg_publish_t : public kaitai::kstruct {

            public:

                msg_publish_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_publish_t();

            private:
                block_selector_t* m_body;
                nano_t* m__root;
                nano_t* m__parent;

            public:
                block_selector_t* body() const { return m_body; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * State block
            */

            class block_state_t : public kaitai::kstruct {

            public:

                block_state_t(kaitai::kstream* p__io, nano_t::block_selector_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~block_state_t();

            private:
                std::string m_account;
                std::string m_previous;
                std::string m_representative;
                std::string m_balance;
                std::string m_link;
                std::string m_signature;
                uint64_t m_work;
                nano_t* m__root;
                nano_t::block_selector_t* m__parent;

            public:

                /**
                * Public key of account represented by this state block
                */
                std::string account() const { return m_account; }

                /**
                * Hash of previous block
                */
                std::string previous() const { return m_previous; }

                /**
                * Public key of the representative account
                */
                std::string representative() const { return m_representative; }

                /**
                * 128-bit big endian balance
                */
                std::string balance() const { return m_balance; }

                /**
                * Pairing send's block hash (open/receive), 0 (change) or destination public key (send)
                */
                std::string link() const { return m_link; }

                /**
                * ed25519-blake2b signature
                */
                std::string signature() const { return m_signature; }

                /**
                * Proof of work (big endian)
                */
                uint64_t work() const { return m_work; }
                nano_t* _root() const { return m__root; }
                nano_t::block_selector_t* _parent() const { return m__parent; }
            };

            /**
            * A general purpose pair of 32-byte hash values
            */

            class hash_pair_t : public kaitai::kstruct {

            public:

                hash_pair_t(kaitai::kstream* p__io, nano_t::confirm_request_by_hash_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~hash_pair_t();

            private:
                std::string m_first;
                std::string m_second;
                nano_t* m__root;
                nano_t::confirm_request_by_hash_t* m__parent;

            public:

                /**
                * First hash in pair
                */
                std::string first() const { return m_first; }

                /**
                * Second hash in pair
                */
                std::string second() const { return m_second; }
                nano_t* _root() const { return m__root; }
                nano_t::confirm_request_by_hash_t* _parent() const { return m__parent; }
            };

            /**
            * Response of the msg_bulk_pull_account message. The structure depends on the
            * flags that was passed to the query.
            */

            class bulk_pull_account_response_t : public kaitai::kstruct {

            public:
                class frontier_balance_entry_t;
                class bulk_pull_account_entry_t;

                bulk_pull_account_response_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~bulk_pull_account_response_t();

                class frontier_balance_entry_t : public kaitai::kstruct {

                public:

                    frontier_balance_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~frontier_balance_entry_t();

                private:
                    std::string m_frontier_hash;
                    std::string m_balance;
                    nano_t* m__root;
                    kaitai::kstruct* m__parent;

                public:

                    /**
                    * Hash of the head block of the account chain.
                    */
                    std::string frontier_hash() const { return m_frontier_hash; }

                    /**
                    * 128-bit big endian account balance.
                    */
                    std::string balance() const { return m_balance; }
                    nano_t* _root() const { return m__root; }
                    kaitai::kstruct* _parent() const { return m__parent; }
                };

                class bulk_pull_account_entry_t : public kaitai::kstruct {

                public:

                    bulk_pull_account_entry_t(uint8_t p_flags, kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~bulk_pull_account_entry_t();

                private:
                    bool f_pending_address_only;
                    bool m_pending_address_only;

                public:
                    bool pending_address_only();

                private:
                    bool f_pending_include_address;
                    bool m_pending_include_address;

                public:
                    bool pending_include_address();

                private:
                    std::string m_hash;
                    bool n_hash;

                public:
                    bool _is_null_hash() { hash(); return n_hash; };

                private:
                    std::string m_amount;
                    bool n_amount;

                public:
                    bool _is_null_amount() { amount(); return n_amount; };

                private:
                    std::string m_source;
                    bool n_source;

                public:
                    bool _is_null_source() { source(); return n_source; };

                private:
                    uint8_t m_flags;
                    nano_t* m__root;
                    kaitai::kstruct* m__parent;

                public:
                    std::string hash() const { return m_hash; }
                    std::string amount() const { return m_amount; }
                    std::string source() const { return m_source; }
                    uint8_t flags() const { return m_flags; }
                    nano_t* _root() const { return m__root; }
                    kaitai::kstruct* _parent() const { return m__parent; }
                };

            private:
                frontier_balance_entry_t* m_frontier_entry;
                std::vector<bulk_pull_account_entry_t*>* m_pending_entry;
                uint8_t m_flags;
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                frontier_balance_entry_t* frontier_entry() const { return m_frontier_entry; }
                std::vector<bulk_pull_account_entry_t*>* pending_entry() const { return m_pending_entry; }
                uint8_t flags() const { return m_flags; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            /**
            * A list of 8 peers, some of which may be all-zero.
            */

            class msg_keepalive_t : public kaitai::kstruct {

            public:

                msg_keepalive_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_keepalive_t();

            private:
                std::vector<peer_t*>* m_peers;
                bool n_peers;

            public:
                bool _is_null_peers() { peers(); return n_peers; };

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:
                std::vector<peer_t*>* peers() const { return m_peers; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * Requests confirmation of the given block or list of root/hash pairs
            */

            class msg_confirm_req_t : public kaitai::kstruct {

            public:

                msg_confirm_req_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_confirm_req_t();

            private:
                confirm_request_by_hash_t* m_reqbyhash;
                bool n_reqbyhash;

            public:
                bool _is_null_reqbyhash() { reqbyhash(); return n_reqbyhash; };

            private:
                block_selector_t* m_block;
                bool n_block;

            public:
                bool _is_null_block() { block(); return n_block; };

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:
                confirm_request_by_hash_t* reqbyhash() const { return m_reqbyhash; }
                block_selector_t* block() const { return m_block; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * Bulk pull account request.
            */

            class msg_bulk_pull_account_t : public kaitai::kstruct {

            public:

                msg_bulk_pull_account_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_bulk_pull_account_t();

            private:
                std::string m_account;
                std::string m_minimum_amount;
                enum_bulk_pull_account_t m_flags;
                nano_t* m__root;
                nano_t* m__parent;

            public:

                /**
                * Account public key.
                */
                std::string account() const { return m_account; }

                /**
                * 128-bit big endian minimum amount.
                */
                std::string minimum_amount() const { return m_minimum_amount; }
                enum_bulk_pull_account_t flags() const { return m_flags; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * A sequence of hashes, where count is read from header.
            */

            class vote_by_hash_t : public kaitai::kstruct {

            public:

                vote_by_hash_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~vote_by_hash_t();

            private:
                std::vector<std::string>* m_hashes;
                bool n_hashes;

            public:
                bool _is_null_hashes() { hashes(); return n_hashes; };

            private:
                nano_t* m__root;
                nano_t::msg_confirm_ack_t* m__parent;

            public:
                std::vector<std::string>* hashes() const { return m_hashes; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_confirm_ack_t* _parent() const { return m__parent; }
            };

            /**
            * Common data shared by votes
            */

            class vote_common_t : public kaitai::kstruct {

            public:

                vote_common_t(kaitai::kstream* p__io, nano_t::msg_confirm_ack_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~vote_common_t();

            private:
                bool f_timestamp;
                int32_t m_timestamp;

            public:

                /**
                * Number of seconds since the UTC epoch vote was generated at
                */
                int32_t timestamp();

            private:
                bool f_vote_duration_bits;
                int32_t m_vote_duration_bits;

            public:

                /**
                * Since V23.0 this is specified as 2^(duration + 4) in milliseconds
                */
                int32_t vote_duration_bits();

            private:
                std::string m_account;
                std::string m_signature;
                uint64_t m_timestamp_and_vote_duration;
                nano_t* m__root;
                nano_t::msg_confirm_ack_t* m__parent;

            public:
                std::string account() const { return m_account; }
                std::string signature() const { return m_signature; }
                uint64_t timestamp_and_vote_duration() const { return m_timestamp_and_vote_duration; }
                nano_t* _root() const { return m__root; }
                nano_t::msg_confirm_ack_t* _parent() const { return m__parent; }
            };

            class message_header_t : public kaitai::kstruct {

            public:

                message_header_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~message_header_t();

            private:
                bool f_bulk_pull_ascending_flag;
                int32_t m_bulk_pull_ascending_flag;

            public:

                /**
                * Since protocol version 19 (release 24).
                * May be set for "bulk_pull" messages.
                * If set, it reverses the order in which bulk_pull returns blocks. It returns the frontier block last.
                */
                int32_t bulk_pull_ascending_flag();

            private:
                bool f_response_flag;
                int32_t m_response_flag;

            public:

                /**
                * If set, this is a node_id_handshake response. This maybe be set at the
                * same time as the query_flag.
                */
                int32_t response_flag();

            private:
                bool f_block_type_int;
                int32_t m_block_type_int;

            public:
                int32_t block_type_int();

            private:
                bool f_telemetry_size;
                int32_t m_telemetry_size;

            public:

                /**
                * Since protocol version 18.
                * Must be set for "telemetry_ack" messages. Indicates size of payload.
                */
                int32_t telemetry_size();

            private:
                bool f_extended_params_present;
                int32_t m_extended_params_present;

            public:

                /**
                * Since protocol version 15.
                * May be set for "bulk_pull" messages.
                * If set, the bulk_pull message contain extended parameters.
                */
                int32_t extended_params_present();

            private:
                bool f_block_type;
                enum_blocktype_t m_block_type;

            public:

                /**
                * The block type determines what block is embedded in the message.
                * For some message types, block type is not relevant and the block type
                * is set to "invalid" or "not_a_block"
                */
                enum_blocktype_t block_type();

            private:
                bool f_item_count_int;
                int32_t m_item_count_int;

            public:

                /**
                * Since protocol v17. For confirm_ack vote-by-hash, this is the number of hashes
                * in the body. For confirm_req request-by-hash, this is the number of hash+root pairs.
                */
                int32_t item_count_int();

            private:
                bool f_query_flag;
                int32_t m_query_flag;

            public:

                /**
                * If set, this is a node_id_handshake query. This maybe be set at the
                * same time as the response_flag.
                */
                int32_t query_flag();

            private:
                bool f_confirmed_present;
                int32_t m_confirmed_present;

            public:

                /**
                * Since protocol version 18 (release 21.3).
                * May be set for "frontier_req" messages.
                * If set, the frontier_req response contains confirmed frontiers for each account.
                */
                int32_t confirmed_present();

            private:
                bool f_asc_pull_size;
                int32_t m_asc_pull_size;

            public:

                /**
                * Since protocol version 19 (release 24)
                * Must be set for "ascending pull messages" messages. Indicates size of payload of ascending pull message.
                */
                int32_t asc_pull_size();

            private:
                std::string m_magic;
                enum_network_t m_network_id;
                uint8_t m_version_max;
                uint8_t m_version_using;
                uint8_t m_version_min;
                enum_msgtype_t m_message_type;
                uint16_t m_extensions;
                nano_t* m__root;
                nano_t* m__parent;

            public:

                /**
                * Protocol identifier. Always 'R'.
                */
                std::string magic() const { return m_magic; }

                /**
                * Network ID 'A', 'B' or 'C' for test, beta or live network respectively.
                */
                enum_network_t network_id() const { return m_network_id; }

                /**
                * Maximum version supported by the sending node
                */
                uint8_t version_max() const { return m_version_max; }

                /**
                * Version used by the sending node
                */
                uint8_t version_using() const { return m_version_using; }

                /**
                * Minimum version supported by the sending node
                */
                uint8_t version_min() const { return m_version_min; }

                /**
                * Message type
                */
                enum_msgtype_t message_type() const { return m_message_type; }

                /**
                * Extensions bitfield
                */
                uint16_t extensions() const { return m_extensions; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

            /**
            * Response of the msg_frontier_req TCP request. An all-zero account and frontier_hash signifies the end of the result.
            */

            class frontier_response_t : public kaitai::kstruct {

            public:
                class frontier_entry_t;

                frontier_response_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~frontier_response_t();

                class frontier_entry_t : public kaitai::kstruct {

                public:

                    frontier_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~frontier_entry_t();

                private:
                    std::string m_account;
                    std::string m_frontier_hash;
                    nano_t* m__root;
                    kaitai::kstruct* m__parent;

                public:

                    /**
                    * Public key of account.
                    */
                    std::string account() const { return m_account; }

                    /**
                    * Hash of the head block of the account chain.
                    */
                    std::string frontier_hash() const { return m_frontier_hash; }
                    nano_t* _root() const { return m__root; }
                    kaitai::kstruct* _parent() const { return m__parent; }
                };

            private:
                std::vector<frontier_entry_t*>* m_entry;
                nano_t* m__root;
                kaitai::kstruct* m__parent;

            public:
                std::vector<frontier_entry_t*>* entry() const { return m_entry; }
                nano_t* _root() const { return m__root; }
                kaitai::kstruct* _parent() const { return m__parent; }
            };

            /**
            * ascending pull request
            */

            class msg_asc_pull_req_t : public kaitai::kstruct {

            public:
                class account_info_req_payload_t;
                class blocks_req_payload_t;
                class asc_pull_req_payload_t;

                msg_asc_pull_req_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_asc_pull_req_t();

                /**
                * ascend pull request acount info payload
                */

                class account_info_req_payload_t : public kaitai::kstruct {

                public:

                    account_info_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~account_info_req_payload_t();

                private:
                    std::string m_start;
                    enum_asc_hash_type_t m_start_type;
                    nano_t* m__root;
                    nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* m__parent;

                public:

                    /**
                    * block hash or account pubkey to pull from
                    */
                    std::string start() const { return m_start; }
                    enum_asc_hash_type_t start_type() const { return m_start_type; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* _parent() const { return m__parent; }
                };

                /**
                * ascend pull request block payload
                */

                class blocks_req_payload_t : public kaitai::kstruct {

                public:

                    blocks_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~blocks_req_payload_t();

                private:
                    std::string m_start;
                    uint8_t m_count;
                    enum_asc_hash_type_t m_start_type;
                    nano_t* m__root;
                    nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* m__parent;

                public:

                    /**
                    * block hash or account pubkey to pull from
                    */
                    std::string start() const { return m_start; }

                    /**
                    * max number of blocks to pull
                    */
                    uint8_t count() const { return m_count; }
                    enum_asc_hash_type_t start_type() const { return m_start_type; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_req_t::asc_pull_req_payload_t* _parent() const { return m__parent; }
                };

                /**
                * payload of ascend pull requests
                */

                class asc_pull_req_payload_t : public kaitai::kstruct {

                public:

                    asc_pull_req_payload_t(kaitai::kstream* p__io, nano_t::msg_asc_pull_req_t* p__parent = 0, nano_t* p__root = 0);

                private:
                    void _read();
                    void _clean_up();

                public:
                    ~asc_pull_req_payload_t();

                private:
                    account_info_req_payload_t* m_account_info;
                    bool n_account_info;

                public:
                    bool _is_null_account_info() { account_info(); return n_account_info; };

                private:
                    blocks_req_payload_t* m_blocks;
                    bool n_blocks;

                public:
                    bool _is_null_blocks() { blocks(); return n_blocks; };

                private:
                    nano_t* m__root;
                    nano_t::msg_asc_pull_req_t* m__parent;

                public:
                    account_info_req_payload_t* account_info() const { return m_account_info; }
                    blocks_req_payload_t* blocks() const { return m_blocks; }
                    nano_t* _root() const { return m__root; }
                    nano_t::msg_asc_pull_req_t* _parent() const { return m__parent; }
                };

            private:
                asc_pull_base_t* m_base;
                asc_pull_req_payload_t* m_payload;
                nano_t* m__root;
                nano_t* m__parent;
                std::string m__raw_payload;
                kaitai::kstream* m__io__raw_payload;

            public:
                asc_pull_base_t* base() const { return m_base; }
                asc_pull_req_payload_t* payload() const { return m_payload; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
                std::string _raw_payload() const { return m__raw_payload; }
                kaitai::kstream* _io__raw_payload() const { return m__io__raw_payload; }
            };

            /**
            * Signed telemetry response
            */

            class msg_telemetry_ack_t : public kaitai::kstruct {

            public:

                msg_telemetry_ack_t(kaitai::kstream* p__io, nano_t* p__parent = 0, nano_t* p__root = 0);

            private:
                void _read();
                void _clean_up();

            public:
                ~msg_telemetry_ack_t();

            private:
                std::string m_signature;
                std::string m_nodeid;
                uint64_t m_blockcount;
                uint64_t m_cementedcount;
                uint64_t m_uncheckedcount;
                uint64_t m_accountcount;
                uint64_t m_bandwidthcap;
                uint32_t m_peercount;
                uint8_t m_protocolversion;
                uint64_t m_uptime;
                std::string m_genesisblock;
                uint8_t m_majorversion;
                uint8_t m_minorversion;
                uint8_t m_patchversion;
                uint8_t m_prereleaseversion;
                uint8_t m_maker;
                uint64_t m_timestamp;
                uint64_t m_activedifficulty;
                std::vector<uint64_t>* m_unknown_data;
                bool n_unknown_data;

            public:
                bool _is_null_unknown_data() { unknown_data(); return n_unknown_data; };

            private:
                nano_t* m__root;
                nano_t* m__parent;

            public:

                /**
                * Signature (Big endian)
                */
                std::string signature() const { return m_signature; }

                /**
                * Public node id (Big endian)
                */
                std::string nodeid() const { return m_nodeid; }

                /**
                * Block count
                */
                uint64_t blockcount() const { return m_blockcount; }

                /**
                * Cemented block count
                */
                uint64_t cementedcount() const { return m_cementedcount; }

                /**
                * Unchecked block count
                */
                uint64_t uncheckedcount() const { return m_uncheckedcount; }

                /**
                * Account count
                */
                uint64_t accountcount() const { return m_accountcount; }

                /**
                * Bandwidth limit, 0 indiciates unlimited
                */
                uint64_t bandwidthcap() const { return m_bandwidthcap; }

                /**
                * Peer count
                */
                uint32_t peercount() const { return m_peercount; }

                /**
                * Protocol version
                */
                uint8_t protocolversion() const { return m_protocolversion; }

                /**
                * Length of time a peer has been running for (in seconds)
                */
                uint64_t uptime() const { return m_uptime; }

                /**
                * Genesis block hash (Big endian)
                */
                std::string genesisblock() const { return m_genesisblock; }

                /**
                * Major version
                */
                uint8_t majorversion() const { return m_majorversion; }

                /**
                * Minor version
                */
                uint8_t minorversion() const { return m_minorversion; }

                /**
                * Patch version
                */
                uint8_t patchversion() const { return m_patchversion; }

                /**
                * Pre-release version
                */
                uint8_t prereleaseversion() const { return m_prereleaseversion; }

                /**
                * Maker version. 0 indicates it is from the Nano Foundation, there is no standardised list yet for any others.
                */
                uint8_t maker() const { return m_maker; }

                /**
                * Number of milliseconds since the UTC epoch
                */
                uint64_t timestamp() const { return m_timestamp; }

                /**
                * The current network active difficulty.
                */
                uint64_t activedifficulty() const { return m_activedifficulty; }
                std::vector<uint64_t>* unknown_data() const { return m_unknown_data; }
                nano_t* _root() const { return m__root; }
                nano_t* _parent() const { return m__parent; }
            };

        private:
            bool f_const_block_zero;
            std::string m_const_block_zero;

        public:
            std::string const_block_zero();

        private:
            message_header_t* m_header;
            kaitai::kstruct* m_body;
            nano_t* m__root;
            kaitai::kstruct* m__parent;

        public:

            /**
            * Message header with message type, version information and message-specific extension bits.
            */
            message_header_t* header() const { return m_header; }

            /**
            * Message body whose content depends on block type in the header.
            */
            kaitai::kstruct* body() const { return m_body; }
            nano_t* _root() const { return m__root; }
            kaitai::kstruct* _parent() const { return m__parent; }
        };
    }
}

#endif  // NANO_H_
