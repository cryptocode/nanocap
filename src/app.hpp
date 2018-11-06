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
		const std::string VERSION = PACKAGE_VERSION;

		app();
		~app();
		void launch(int argc, char** argv);

		inline nanocap::config& get_config() { return config; }
		inline nanocap::db& get_db() { return *db; }
		inline nanocap::packet_handler& get_handler() { return *handler; }
		inline nanocap::webserver& get_webserver() { return *webserver; }
		inline boost::program_options::variables_map& get_options() { return option_map; }

	private:
		nanocap::config config;
		std::unique_ptr<nanocap::db> db;
		std::unique_ptr<packet_handler> handler;
		std::unique_ptr<nanocap::webserver> webserver;
		boost::program_options::variables_map option_map;
	};
}
