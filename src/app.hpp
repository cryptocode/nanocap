#pragma once

#include <config.hpp>
#include <boost/program_options.hpp>

namespace nanocap
{
	class packet_handler;
	class webserver;
	class db;

	/** Application singleton */
	class app
	{
	public:
		const std::string VERSION = "2.0";

		app();
		~app();
		void launch(int argc, char** argv);

		nanocap::config& get_config() { return config; }
		nanocap::db& get_db() { return *db; }
		nanocap::packet_handler& get_handler() { return *handler; }
		nanocap::webserver& get_webserver() { return *webserver; }
		boost::program_options::variables_map& get_options() { return option_map; }

	private:
		nanocap::config config;
		std::unique_ptr<nanocap::db> db;
		std::unique_ptr<packet_handler> handler;
		std::unique_ptr<nanocap::webserver> webserver;
		boost::program_options::variables_map option_map;
	};
}
