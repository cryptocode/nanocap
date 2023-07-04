project(nanocap)
cmake_minimum_required(VERSION 2.8)

set(NANOCAP_GENERATE_PARSER OFF CACHE BOOL "If set, nanocurrency/protocol must be a submodule and Kaitai installed and on the path.")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# Find Boost
find_package(Boost 1.66 REQUIRED COMPONENTS system filesystem program_options)

# Do not build Kaitai runtime with iconv
add_definitions(-DKS_STR_ENCODING_NONE)

# Include directories
include_directories (${CMAKE_SOURCE_DIR}/src)
include_directories("/usr/local/include/pcapplusplus")
#include_directories (${Boost_INCLUDE_DIRS})

#Link directories
link_directories("/usr/local/lib")

# Assuming other libraries are installed and findable
find_package(JSONForModernCPP REQUIRED)
find_package(Threads REQUIRED)

# Regenerate parser. The nanocurrency/protocol repository must be
# added as a submodule when this flag is active.
if (NANOCAP_GENERATE_PARSER)

	execute_process (
		COMMAND
		"kaitai-struct-compiler" "${CMAKE_SOURCE_DIR}/protocol/reference/protocol.ksy" "--cpp-namespace" "nano::protocol" "--outdir" "${CMAKE_SOURCE_DIR}/src" "-t" "cpp_stl"
		ERROR_FILE "${CMAKE_SOURCE_DIR}/gen.err"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

	execute_process (
		COMMAND
		"kaitai-struct-compiler" "${CMAKE_SOURCE_DIR}/protocol/reference/protocol.ksy" "--outdir" "${CMAKE_SOURCE_DIR}/public" "-t" "javascript"
		ERROR_FILE "${CMAKE_SOURCE_DIR}/gen.err"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)
endif ()

# Blake2
file(GLOB_RECURSE blake2_sources
	"src/blake2/ref/blake2*.h"
	"src/blake2/ref/blake2*.c")
include_directories (src/blake2/ref)

add_library(blake2b ${blake2_sources})

include_directories (src/sqlite)
add_definitions(-DSQLITE_ENABLE_JSON1 -DSQLITE_THREADSAFE=0)
add_library(sqlite src/sqlite/sqlite3.h src/sqlite/sqlite3ext.h src/sqlite/sqlite3.c)
add_library(sqlitecpp
	src/SQLiteCpp/Assertion.h src/SQLiteCpp/Backup.h src/SQLiteCpp/Backup.cpp
	src/SQLiteCpp/Column.h src/SQLiteCpp/Column.cpp src/SQLiteCpp/Database.h src/SQLiteCpp/Database.cpp
	src/SQLiteCpp/Exception.h src/SQLiteCpp/Exception.cpp src/SQLiteCpp/SQLiteCpp.h
	src/SQLiteCpp/Statement.h src/SQLiteCpp/Statement.cpp src/SQLiteCpp/Transaction.h src/SQLiteCpp/Transaction.cpp src/SQLiteCpp/Utils.h src/SQLiteCpp/VariadicBind.h)

add_executable(nanocap
	src/shared.hpp src/util/hex.hpp src/util/nanoutil.hpp src/util/nanoutil.cpp
	src/db.hpp src/db.cpp src/packet_handler.hpp src/packet_handler.cpp src/config.hpp
	src/app.hpp src/app.cpp
	src/webserver.hpp src/webserver.cpp
	src/nano.cpp src/nano.h src/kaitai/kaitaistream.cpp
	src/util/ctpl_stl.h src/util/termcolor.hpp
	)

target_link_libraries(nanocap blake2b sqlite sqlitecpp Boost::system Boost::filesystem Boost::program_options JSONForModernCPP /usr/local/lib/libPacket++.a /usr/local/lib/libPcap++.a /usr/local/lib/libCommon++.a pcap ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})


if (APPLE)
	target_link_libraries(nanocap "-framework CoreFoundation")
	target_link_libraries(nanocap "-framework SystemConfiguration")
endif ()
