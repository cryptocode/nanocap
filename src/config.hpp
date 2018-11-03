#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nanocap
{
	/** Config object serialized from nanocap.config */
	class config
	{
	public:
		/** IP of interface to capture traffic on */
		std::string ip {""};
		/** If non-zero, use as src/dst port filter. Note that a valid \filter overrides the port setting. */
		int port {0};
		/** sqlite3 database */
		std::string db {""};
		/** If true, capture live traffic. */
		bool record {false};
		/** If set, use a Berkeley Packet Filter expression instead of a filter based on the port config entry. */
		std::string filter {""};
	};

	inline void to_json(nlohmann::json& json, const config& conf)
	{
		json = nlohmann::json{{"ip", conf.ip}, {"port", conf.port}, {"database", conf.db}, {"record", conf.record}, {"filter", conf.filter}};
	}
	
	inline void from_json(const nlohmann::json& json, config& conf)
	{
		if (json.count("ip") > 0)
			json.at("ip").get_to(conf.ip);
		if (json.count("port") > 0)
			json.at("port").get_to(conf.port);
		if (json.count("database") > 0)
			json.at("database").get_to(conf.db);
		if (json.count("record") > 0)
			json.at("record").get_to(conf.record);
		if (json.count("filter") > 0)
			json.at("filter").get_to(conf.filter);
	}
	
	inline config load_config (std::string path)
	{
		nlohmann::json json;
		std::ifstream ifs(path);

		try
		{
			// There are issues with nlohmann::json >> stream operator, so we read via string
			std::string json_input(std::istreambuf_iterator<char>{ifs}, {});
			json = nlohmann::json::parse(json_input);
		}
		catch (nlohmann::detail::parse_error const & err)
		{
			std::cerr << "Couldn't parse config: " << err.what() << std::endl;
		}
		
		return static_cast<config> (json);
	}
	
	inline void save_config (std::string path, nanocap::config & config)
	{
		std::ofstream ofs(path);
		try
		{
			nlohmann::json json = config;
			ofs << json.dump(4) << std::endl;
		}
		catch (nlohmann::detail::type_error const & err)
		{
			std::cerr << "couldn't parse config: " << err.what() << std::endl;
		}
	}
}
