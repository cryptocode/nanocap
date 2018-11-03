#include "stdlib.h"
#include "UdpLayer.h"
#include "TcpLayer.h"
#include "IPv4Layer.h"
#include "IPv6Layer.h"
#include "PcapLiveDeviceList.h"
#include "PlatformSpecificUtils.h"
#include <nano.h>
#include <app.hpp>
#include <webserver.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/asio/signal_set.hpp>

static nanocap::app application;

nanocap::app::app()
{
	std::cout << "nanocap " << app::VERSION << std::endl;
	std::cout << "Supported protocol version: " << nano::protocol::nano_t::PROTOCOL_VERSION_VALUE << std::endl;
}

nanocap::app::~app()
{
}

void nanocap::app::launch(int argc, char** argv)
{
	try
	{
		boost::program_options::options_description desc("Options");
		desc.add_options()
		("help", "Print help messages")
		("if", "List interfaces and ip addresses");
		
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), option_map);
		
		if (option_map.count("help"))
		{
			std::cout << "nanocap" << std::endl << desc << std::endl;
			std::exit(0);
		}
		
		config = nanocap::load_config ("nanocap.config");
		config.record = false;
		nanocap::save_config ("nanocap.config", config);
		db = std::make_unique<nanocap::db>(*this);
		handler = std::make_unique<nanocap::packet_handler>(*this);
		
		// TODO: only create if configured
		webserver = std::make_unique<nanocap::webserver>(*this);
	}
	catch(boost::program_options::error& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void sig_handler(const boost::system::error_code& error, int signal_number)
{
	std::cout << "Stopping capture due to signal " << signal_number << ". Please wait..." << std::endl;
	application.get_handler().stop_capture();
	application.get_handler().print_stats(std::cout);
}

/**
 * main method of the application
 */
int main(int argc, char* argv[])
{
	boost::asio::io_context io_ctx;
	
	try
	{
		application.launch(argc, argv);
		auto& handler = application.get_handler();
		handler.start_capture();
		
		std::cout << "Press ctrl-c to stop" << std::endl;
	
		//PCAP_SLEEP(60);
		boost::asio::signal_set signals(io_ctx, SIGINT);
		signals.async_wait(sig_handler);
		io_ctx.run();
	}
	catch (std::runtime_error & err)
	{
		std::cerr << err.what() << std::endl;
	}
}
