AC_DEFUN([NANOCAP_FIND_BOOST], [
	dnl TODO: XXX: Improve this test
	AX_BOOST_BASE(1.60,, [
		AC_MSG_ERROR([Need Boost 1.60 or greater])
	])
	AX_BOOST_ASIO
	AX_BOOST_IOSTREAMS
	AX_BOOST_FILESYSTEM
	AX_BOOST_PROGRAM_OPTIONS
])

dnl app.cpp:#include <boost/asio.hpp>
dnl app.cpp:#include <boost/endian/conversion.hpp>
dnl app.cpp:#include <boost/asio/signal_set.hpp>
dnl app.cpp:                boost::program_options::options_description desc("Options");
dnl app.cpp:                ("pcap", boost::program_options::value(&pcap_src), "Populate database using this pcap file")
dnl app.cpp:                boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), option_map);
dnl app.cpp:        catch(boost::program_options::error& e)
dnl app.cpp:void sig_handler(const boost::system::error_code& error, int signal_number)
dnl app.cpp:        boost::asio::io_context io_ctx;
dnl app.cpp:                        boost::asio::signal_set signals(io_ctx, SIGINT);
dnl app.hpp:#include <boost/program_options.hpp>
dnl app.hpp:                inline boost::program_options::variables_map& get_options() { return option_map; }
dnl app.hpp:                boost::program_options::variables_map option_map;
dnl grep: blake2: Is a directory
dnl db.cpp:#include <boost/asio.hpp>
dnl db.cpp:#include <boost/endian/conversion.hpp>
dnl db.cpp:         // std::string#data() is contiguous, so we simply cast it to std::array (which boost expects) to save a copy.
dnl db.cpp:         boost::asio::ip::udp::endpoint endpoint (boost::asio::ip::address_v6 (*arr), peer->port ());
dnl grep: kaitai: Is a directory
dnl packet_handler.cpp:#include <boost/asio.hpp>
dnl packet_handler.cpp:#include <boost/endian/conversion.hpp>
dnl packet_handler.cpp:#include <boost/iostreams/stream.hpp>
dnl packet_handler.cpp:             using bufferstream = boost::iostreams::stream<boost::iostreams::array_source>;
dnl packet_handler.cpp:             info.dst_port = boost::endian::big_to_native(udp->getUdpHeader()->portDst);
dnl packet_handler.cpp:             info.src_port = boost::endian::big_to_native(udp->getUdpHeader()->portSrc);
dnl grep: sqlite: Is a directory
dnl grep: util: Is a directory
dnl grep: webserver: Is a directory
dnl webserver.cpp:#include <boost/filesystem.hpp>
dnl webserver.cpp:                  auto web_root_path = boost::filesystem::canonical("public");
dnl webserver.cpp:                  auto path = boost::filesystem::canonical(web_root_path / request->path);
dnl webserver.cpp:                  if(boost::filesystem::is_directory(path))
