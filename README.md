# Overview

**nanocap** is a protocol analyzer for the Nano network.
Features:

* SQL query capabilities to help analyze traffic patterns and diagnose issues on the Nano network.
* Live capture
* Import pcap files (for instance, captured with Wireshark)
* Web interface for querying, even while capturing. Any Sqlite client can also be used.
* Validates that the Nano protocol specification is correct by using its generated parser.
* Automatically performs TCP segment reassembly. This allows capturing of both live and bootstrap data.
* Tracks connection flow.

![](nanocapcast.gif)

# Getting started

For Linux, the easiest way to get started with nanocap is Docker. However, running nanocap in a container may affect packet capture performance. Hence, running an optimized build on the host is recommended to ensure good performance under heavy network load.

On macOS, Docker runs in virtualized environment where --net=host doesn't really work. For live traffic capture, building from source is thus recommended.

## Docker

```
docker pull cryptocode/nanocap
```

## Get IP

Get a list of interfaces that can be captured:

```
nanocap --if
```

or if on Docker:

```
docker run --net=host cryptocode/nanocap bin/nanocap --if
```

## Configure

Before running nanocap, create `nanocap.config` on the host.

If `device_ip` is empty/not present, nanocap tries to guess the interface (this is printed to console.) Leaving it blank is convenient if you frequently switch between multiple local networks.

You can replace `device_ip` with the IP from the above `nanocap --if` command.

```
{
    "capture": {
        "device_ip": "192.168.0.135",
        "enabled": false,
        "filter": "(tcp port 7075 or udp port 7075)",
        "max_capture_megabytes": 8192,
        "block_details": true,
        "bootstrap_details": false
    },
    "database": "capture.db",
    "web": {
        "bind": "0.0.0.0",
        "enabled": true,
        "port": 8077,
        "max_query_result": 5000
    }
}
```

If `bootstrap_details` is true, all bootstrap response data will be captured to tables such as `frontier_response`. This may
cause significant amounts of data to be stored (note that inbound bootstrap requests are captured as well). You may need
to increase `max_capture_megabytes` for long captures.

Likewise, if `block_details` is set, hashes and block details are added to the database for live messages.

Both settings may increase pcap import times.

## Capture live traffic

```
sudo bin/nanocap
```

If on Docker, map the config file and run nanocap like this:

```
docker run --net=host \
	-v `pwd`/nanocap.config:/nanocap/nanocap.config \
	cryptocode/nanocap bin/nanocap
```

Visit http://localhost:8077 to view/stop/restart captures and query packets.

## Import pcap files

```
sudo bin/nanocap --pcap my.pcap
```

If on Docker:

```
docker run -p 8077:8077 \
	-v `pwd`/nanocap.config:/nanocap/nanocap.config \
	-v `pwd`/pcap/my.pcap:/nanocap/my.pcap \
	cryptocode/nanocap bin/nanocap \
	--pcap my.pcap
```

Visit http://localhost:8077 to view import progress and query packets. Live capture can be started once the import is finished.


# Build from source

Get the repo and submodules:

```
git clone --recursive https://github.com/cryptocode/nanocap.git
```

nanocap uses the Conan package manager. See https://docs.conan.io/en/latest/installation.html for information on how to install Conan.

Add the following Conan repository for Boost and pcap++:

```
$ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

Install dependencies using conan. This may take a long time the first time:

```
$ conan install . --build missing
```

Finally, generate the build file:

```
$ mkdir build && cd build
$ cmake .. -G <generator>
```

If running from XCode, make sure you set the working directory to the project directory, and you may need to select Run as root.

### Optional build flags

The nanocap repository contains a pre-generated protocol parser.

If `NANOCAP_GENERATE_PARSER` is set in CMake, C++ and Javascript parsers are automatically regenerated.

For this to work, the latest `kaitai-struct-compiler` must be built and installed. In addition, the Kaitai protocol definition file must be located under `protocol/reference`. This is best done by adding the nanocurrency/protocol repository as a submodule.

# Web console

During import or live capture, progress can be watched at http://localhost:8077, which also provides a query interface.

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

Handshake packets with the response flag set (note that there are nodeid query and response tables as well):

```
select * from packet where msg_type = 0x0a and extensions & 2
```

How many vote-by-hash messages references a given block hash?

```
select count(*) from vote where hashes like '%8EB7%'
```

What packets references the given open block? Note that these queries can be slow on large captures. Consider adding indices.

```
select P.* from block_open O
inner join packet P on P.id = O.packet_id
where O.hash = '09A295011083890BDDCE5C3C11E087F896F50F5AD0FDA02A2C12D6D5989AE281'
```

Use telemetry to get ip/nodeid mappings, but filtering out the local ip

```
select telemetry.nodeid, packet.srcip from telemetry, packet
where telemetry.packet_id = packet.id and srcip not like '192.168.%'
limit 200
```

Connections closed after a certain time

```
select * from connection where closed and time >= '2020-08-22 21:30:00'
```

How may account/frontier hashes per frontier request? Note that both in- and outgoing frontier requests are captured.
Note that capturing bootstrap responses requires "bootstrap_details" to be true in the config.

```
select req.id, count(*) from frontier_request req, frontier_response res where res.frontier_request_id = req.id group by req.id
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


