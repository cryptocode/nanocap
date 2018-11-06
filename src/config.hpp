#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann-json/json.hpp>

using json = nlohmann::json;

namespace nanocap
{
	/** Packlet capture configuration */
	class config_capture
	{
	public:
		bool enabled {true};
		/** IP address of interface to capture*/
		std::string device_ip {""};
		/** If set, use a Berkeley Packet Filter expression instead of a filter based on the port config entry. */
		std::string filter {""};
		/** Maximum estimated database size in MB, by default 4GB. Live capture or pcap import is aborted if necessary. */
		int64_t max_capture_megabytes {4096};
		/** If true, save block details. If false, save only block type. */
		bool block_details {false};
	};

	/** Web/REST server configuration */
	class config_web
	{
	public:
		bool enabled {true};
		std::string bind {"0.0.0.0"};
		int port {7077};
		int max_query_result {5000};
	};

	/** Config object serialized from nanocap.config */
	class config
	{
	public:
		/** sqlite3 database (path or memory) */
		std::string db {""};
		config_capture capture;
		config_web web;
	};
	
	inline void to_json(nlohmann::json& json, const config_capture& capture)
	{
		json = nlohmann::json
		{
			{"enabled", capture.enabled},
			{"device_ip", capture.device_ip},
			{"filter", capture.filter},
			{"max_capture_megabytes", capture.max_capture_megabytes},
			{"block_details", capture.block_details}
		};
	}
	
	inline void from_json(const nlohmann::json& json, config_capture& capture)
	{
		if (json.count("enabled"))
			json.at("enabled").get_to(capture.enabled);
		if (json.count("device_ip"))
			json.at("device_ip").get_to(capture.device_ip);
		if (json.count("filter") > 0)
			json.at("filter").get_to(capture.filter);
		if (json.count("max_capture_megabytes"))
			json.at("max_capture_megabytes").get_to(capture.max_capture_megabytes);
		if (json.count("block_details"))
			json.at("block_details").get_to(capture.block_details);
	}
	
	inline void to_json(nlohmann::json& json, const config_web& web)
	{
		json = nlohmann::json
		{
			{"enabled", web.enabled},
			{"bind", web.bind},
			{"port", web.port},
			{"max_query_result", web.max_query_result}
		};
	}
	
	inline void from_json(const nlohmann::json& json, config_web& web)
	{
		if (json.count("enabled"))
			json.at("enabled").get_to(web.enabled);
		if (json.count("bind"))
			json.at("bind").get_to(web.bind);
		if (json.count("port"))
			json.at("port").get_to(web.port);
		if (json.count("max_query_result"))
			json.at("max_query_result").get_to(web.max_query_result);
	}
	
	inline void to_json(nlohmann::json& json, const config& conf)
	{
		json = nlohmann::json
		{
			{"database", conf.db},
			{"capture", conf.capture},
			{"web", conf.web}
		};
	}
	
	inline void from_json(const nlohmann::json& json, config& conf)
	{
		if (json.count("database") > 0)
			json.at("database").get_to(conf.db);
		if (json.count("web") > 0)
			from_json(static_cast<const nlohmann::json&>(json.at("web")), static_cast<config_web&>(conf.web));
		if (json.count("capture") > 0)
			from_json(static_cast<const nlohmann::json&>(json.at("capture")), static_cast<config_capture&>(conf.capture));
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
