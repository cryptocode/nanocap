project(nanocap)
cmake_minimum_required(VERSION 2.8)

set(NANOCAP_GENERATE_PARSER OFF CACHE BOOL "If set, nanocurrency/protocol must be a submodule and Kaitai installed and on the path.")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Find Boost
set(CMAKE_MODULE_PATH ${CONAN_CMAKE_MODULE_PATH} ${CMAKE_MODULE_PATH})
find_package(Boost 1.66 EXACT)

# Do not build Kaitai runtime with iconv
add_definitions(-DKS_STR_ENCODING_NONE)

# Include directories
include_directories (${CMAKE_BINARY_DIR}/src)
#include_directories (${Boost_INCLUDE_DIRS})

# Regenerate parser. The nanocurrency/protocol repository must be
# added as a submodule when this flag is active.
if (NANOCAP_GENERATE_PARSER)

	execute_process (
		COMMAND
		"kaitai-struct-compiler" "${CMAKE_BINARY_DIR}/protocol/reference/protocol.ksy" "--cpp-namespace" "nano::protocol" "--outdir" "${CMAKE_BINARY_DIR}/src" "-t" "cpp_stl"
		ERROR_FILE "${CMAKE_BINARY_DIR}/gen.err"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	)

	execute_process (
		COMMAND
		"kaitai-struct-compiler" "${CMAKE_BINARY_DIR}/protocol/reference/protocol.ksy" "--outdir" "${CMAKE_BINARY_DIR}/public" "-t" "javascript"
		ERROR_FILE "${CMAKE_BINARY_DIR}/gen.err"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	)
endif ()

#add_executable(reassembly src/reassembly/main.cpp)
#target_link_libraries(reassembly ${CONAN_LIBS} ${Boost_LIBRARIES})

# Blake2
if(SSE STREQUAL "None")
	file(GLOB_RECURSE blake2_sources
		"src/blake2/ref/blake2*.h"
		"src/blake2/ref/blake2*.c")
	include_directories (src/blake2/ref)
else()
	file(GLOB_RECURSE blake2_sources
		"src/blake2/sse/blake2*.h"
		"src/blake2/sse/blake2*.c")
	include_directories (src/blake2/sse)
endif()
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

target_link_libraries(nanocap blake2b sqlite sqlitecpp CONAN_PKG::boost_system CONAN_PKG::boost_filesystem CONAN_PKG::boost_program_options CONAN_PKG::boost_asio CONAN_PKG::boost_multiprecision CONAN_PKG::jsonformoderncpp CONAN_PKG::PcapPlusPlus ${CMAKE_DL_LIBS})

if (APPLE)
	target_link_libraries(nanocap "-framework CoreFoundation")
	target_link_libraries(nanocap "-framework SystemConfiguration")

	#target_link_libraries(reassembly "-framework CoreFoundation")
	#target_link_libraries(reassembly "-framework SystemConfiguration")	
endif ()
