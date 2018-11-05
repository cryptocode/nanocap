# Overview

**nanocap** is a protocol analyzer for the Nano network.

Raison d'être:

* Validate that the protocol specification is correct
* The Nano protocol is evolving quickly and nanocap will be kept up to date as new relases of the protocol is made.
* Provide an alternative to Wireshark and bettercap with query capabilities for diagnosing issues on the Nano network.


# Build

nanocap uses the Conan package manager. See https://docs.conan.io/en/latest/installation.html for information on how to install Conan.

The easy way is `pip install conan`

Add the following Conan repositories for Boost and pcap++:

```
$ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
$ conan remote add andreybronin https://api.bintray.com/conan/andreybronin/conan
```

To download and build everything needed:

```
$ conan install . --build missing
```

To generate the build file:

```
$ cmake . -G <generator>
```

### Optional build flags

The nanocap repository contains pre-generated protocol parsers.

If `NANOCAP_GENERATE_PARSER` is set in CMake, C++ and Javascript parsers are automatically regenerated.

For this to work, kaitai-struct-compiler must be installed. In addition, the Kaitai protocol definition file must be located under `protocol/reference`. This is best done by adding the nanocurrency/protocol repository as a submodule.


# Configuration

See `nanocap.config` for configuration. In particular, the IP used for capturing should be set. `nanocap --if` gives you a list of network interfaces and the associated IP.

The `block_details` adds hashes and block details to the database. This increases pcap import times.

# Importing pcap files

Run `nanocap --pcap my.pcap`

Progress is available in the web console. Live capture can be started once the import is finished.

# Live capture

Run `nanocap`

# Web console

During import or live capture, progress can be watched at http://localhost:7077, which also provides a query interface.

## Queries

The database table definition is provided under the *Schema* page.

Querying can be done via the sqlite3 client or the web interface. Examples:

Packet count by type

```
select count(*), msg_type from packet group by msg_type;
```

Handshake messages with the response flag set:

```
select * from packet where msg_type = 0x0a and dstip = '31.171.251.175' and srcport != 7075 and extensions & 2
```

How many vote-by-hash messages references a given block hash?

```
select count(*) from vote_by_hash where hashes like '%8EB3B7%'
```

What packets references the given open block? Note that these queries can be slow on large captures. Consider adding indices.

```
select P.* from block_open O
inner join packet P on P.id = O.packet_id
where O.hash = '09A295011083890BDDCE5C3C11E087F896F50F5AD0FDA02A2C12D6D5989AE281'
```

# REST API

Documentation in progress...

```
GET api/v1/status
```

```
GET api/v1/schema
```

```
GET api/v1/capture/query?query=<SQL>
```

```
GET api/v1/capture/start
```

```
GET api/v1/capture/stop
```

```
GET api/v1/capture/toggle
```

```
GET api/v1/capture/destroy
```

```
GET api/v1/count/packet
```

```
GET api/v1/count/per-type
```


