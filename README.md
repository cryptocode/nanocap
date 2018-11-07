# Overview

**nanocap** is a protocol analyzer for the Nano network. It supports live capture and *pcap* file import. Packets are stored in an sqlite database which can be queried using the web interface or any sqlite client.

## Why not just use Wireshark or bettercap?

The purpose of nanocap is to:

* Validate that the protocol specification is correct.
* Provide advanced query capabilities for diagnosing issues on the Nano network.

# Docker

For Linux, the easiest way to get started with nanocap is Docker. Running nanocap in a container may affect packet capture performance. Hence, running an optimized build on the host is recommended to ensure good performance under load.

On macOS, docker runs in virtualized environment where --net=host doesn't really work. For live traffic capture, building from source is thus recommended.

## Install

```
docker pull cryptocode/nanocap
```

## Get IP

Get a list of interfaces that can be captured:

```
docker run --net=host cryptocode/nanocap bin/nanocap --if
```

## Configure

Before running nanocap, create `nanocap.config` on the host.

Replace `device_ip` with the IP from the above `nanocap --if` command.

```
{
    "capture": {
        "device_ip": "192.168.0.135",
        "enabled": false,
        "filter": "(tcp port 7075 or udp port 7075)",
        "max_capture_megabytes": 8192,
        "block_details": true
    },
    "database": "capture.db",
    "web": {
        "bind": "0.0.0.0",
        "enabled": true,
        "port": 7077,
        "max_query_result": 5000
    }
}
```

## Capture

Map the config file and run nanocap:

```
docker run --net=host \
	-v `pwd`/nanocap.config:/nanocap/nanocap.config \
	cryptocode/nanocap bin/nanocap
```

Visit http://localhost:7077 to view/stop/restart capture and query packets.

## Import pcap files

```
docker run -p 7077:7077 \
	-v `pwd`/nanocap.config:/nanocap/nanocap.config \
	-v `pwd`/pcap/rep.pcap:/nanocap/rep.pcap \
	cryptocode/nanocap bin/nanocap \
	--pcap rep.pcap
```

Visit http://localhost:7077 to view import progress and query packets.


# Build from source

nanocap uses the Conan package manager. See https://docs.conan.io/en/latest/installation.html for information on how to install Conan.

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

For this to work, the latest `kaitai-struct-compiler` must be built and installed. In addition, the Kaitai protocol definition file must be located under `protocol/reference`. This is best done by adding the nanocurrency/protocol repository as a submodule.


# Configuration

See `nanocap.config` for configuration. In particular, the IP used for capturing should be set. `nanocap --if` gives you a list of network interfaces and the associated IP.

If `block_details` is set, hashes and block details are added to the database. This increases pcap import times.

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
select count(*), msg_type from packet group by msg_type
```

Vote-by-hash histogram:

```
select vote_count, count(*) from vote where vbh group by vote_count order by vote_count
```

Handshake messages with the response flag set:

```
select * from packet where msg_type = 0x0a and dstip = '31.171.251.175' and srcport != 7075 and extensions & 2
```

How many vote-by-hash messages references a given block hash?

```
select count(*) from vote where hashes like '%8EB3B7%'
```

What packets references the given open block? Note that these queries can be slow on large captures. Consider adding indices.

```
select P.* from block_open O
inner join packet P on P.id = O.packet_id
where O.hash = '09A295011083890BDDCE5C3C11E087F896F50F5AD0FDA02A2C12D6D5989AE281'
```

# REST API

Documentation in progress.

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


