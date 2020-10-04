#include <nano.h>
#include <app.hpp>
#include <webserver.hpp>
#include <db.hpp>
#include <packet_handler.hpp>
#include <util/termcolor.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/asio/signal_set.hpp>

static nanocap::app application;

nanocap::app::app()
{
	std::cout << termcolor::bold << "nanocap " << app::VERSION << termcolor::reset << std::endl;
	std::cout << "Supported protocol version: " << nano::protocol::nano_t::PROTOCOL_VERSION_VALUE << std::endl;
}

nanocap::app::~app()
{
}

void nanocap::app::launch(int argc, char** argv)
{
	try
	{
		std::string pcap_src = "";
		boost::program_options::options_description desc("Options");
		desc.add_options()
		("help", "Print help messages")
		("version", "Print version")
		("pcap", boost::program_options::value(&pcap_src), "Populate database using this pcap file")
		("if", "List interfaces and ip addresses");
		
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), option_map);
		
		if (option_map.count("help"))
		{
			std::cout << "nanocap" << std::endl << desc << std::endl;
			std::exit(0);
		}
		else if (option_map.count("version"))
		{
			// The constructor always prints version info, so all we need to do is quit
			std::exit(0);
		}
		
		config = nanocap::load_config ("nanocap.config");
		db = std::make_unique<nanocap::db>(*this);
		handler = std::make_unique<nanocap::packet_handler>(*this);
		
		if (config.web.enabled)
		{
			webserver = std::make_unique<nanocap::webserver>(*this);
		}
	}
	catch(boost::program_options::error& e)
	{
		std::cerr << e.what() << std::endl;
		std::exit(0);
	}
}

namespace
{
	boost::asio::io_context io_ctx;
}

void sig_handler(const boost::system::error_code& error, int signal_number)
{
	std::cout << "Stopping due to signal " << signal_number << ". Please wait..." << std::endl;
	application.get_handler().stop_packet_processing_thread();
	std::cout << "Stopping capture" << std::endl;
	application.get_handler().stop_capture();
	std::cout << "Stopping I/O" << std::endl;
	io_ctx.stop();
	std::cout << "Exiting" << std::endl;
}

/**
 * main method of the application
 */
int main(int argc, char* argv[])
{
	try
	{
		application.launch(argc, argv);
		auto& handler = application.get_handler();
		
		if (application.get_config().web.enabled)
		{
			std::cout << termcolor::bold << "Console available at:" << termcolor::reset << std::endl;
			std::cout << "    " << application.get_webserver().get_static_url() << std::endl;
			std::cout << termcolor::bold << "API available at:" << termcolor::reset << std::endl;
			std::cout << "    " << application.get_webserver().get_api_url() << std::endl;
		}

		if (application.get_options().count("pcap"))
		{
			std::string pcap_file = application.get_options()["pcap"].as<std::string>();
			handler.analyze_pcap_file(pcap_file);
		}
		else if (application.get_options().count("if"))
		{
			handler.print_devices();
			std::exit(0);
		}
		else if (application.get_config().capture.enabled)
		{
			handler.start_capture();
		}

		if (application.get_config().web.enabled || application.get_config().capture.enabled)
		{
			std::cout << termcolor::bold << "Press ctrl-c to stop" << termcolor::reset << std::endl;

			boost::asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
			signals.async_wait(sig_handler);
			io_ctx.run();
		}
	}
	catch (std::runtime_error & err)
	{
		std::cerr << err.what() << std::endl;
	}
}
