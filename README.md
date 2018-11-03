# Overview

**nanocap** is a protocol analyzer for the Nano network.

Raison d'être:

* Validate that the protocol specification is correct
* The Nano protocol is evolving quickly and nanocap will be kept up to date as new relases of the protocol is made.
* Provide an alternative to Wireshark and bettercap with query capabilities helpfor for diagnosing issues on the Nano network.


# Build

nanocap uses the Conan package manager. See docs.conan.io/en/latest/installation.html for information on how to install Conan.

Add the following Conan repositories for Boost and pcap++:

```
$ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
$ conan remote add andreybronin https://api.bintray.com/conan/andreybronin/conan
```

Download and build everything needed:

```
$ conan install . --build missing
```

Generate build file

```
$ cmake . -G <generator>
```
