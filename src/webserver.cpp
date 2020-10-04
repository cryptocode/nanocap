#include <algorithm>
#include <fstream>
#include <webserver.hpp>
#include <packet_handler.hpp>
#include <app.hpp>
#include <db.hpp>
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

using json = nlohmann::json;

nanocap::webserver::webserver(nanocap::app& app) : app (app)
{
	server.config.address = app.get_config().web.bind;
	server.config.port = app.get_config().web.port;
	start();
}

nanocap::webserver::~webserver()
{
	server.stop();
	if (runner.joinable())
	{
		runner.join();
	}
}

std::string nanocap::webserver::get_static_url()
{
	auto host = server.config.address == "0.0.0.0" ? "localhost" : server.config.address;
	return "http://" + host + ":" + std::to_string(server.config.port);
}

std::string nanocap::webserver::get_api_url()
{
	auto host = server.config.address == "0.0.0.0" ? "localhost" : server.config.address;
	return "http://" + host + ":" + std::to_string(server.config.port) + "/api/v1";
}

void nanocap::webserver::start()
{
	server.resource["^/api/v1/status$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		json status;
		status["capturing"] = app.get_handler().is_live_capturing();
		status["importing"] = app.get_handler().is_file_importing();
		status["max_db_size_reached"] = app.get_db().max_reached.load();
		status["version"] = app.VERSION;
		status["packet_queue_size"] = app.get_handler().packet_queue_size();
		status["packet_parse_error_count"] = app.get_handler().packet_parse_error_count.load();
		response->write(status.dump(4));
	};

	server.resource["^/api/v1/capture/query$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		auto params = request->parse_query_string();
		if (params.find("query") != params.end())
		{
			response->write(app.get_db().query(params.find("query")->second).dump(4));
		}
		else
		{
			response->write("{\error\": \"Missing query\"}");
		}
	};

	server.resource["^/api/v1/capture/start$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		app.get_handler().start_capture();
		response->write("{}");
	};

	server.resource["^/api/v1/capture/stop$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		app.get_handler().stop_capture();
		response->write("{}");
	};

	server.resource["^/api/v1/capture/toggle$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		if (app.get_handler().is_file_importing())
		{
			response->write("{\"error\": \"Import in progress\"}");
		}
		else
		{
			if (app.get_handler().is_live_capturing())
			{
				app.get_handler().stop_capture();
			}
			else
			{
				app.get_handler().start_capture();
			}
			response->write("{}");
		}
	};

	server.resource["^/api/v1/capture/destroy$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		bool is_capturing = app.get_handler().is_live_capturing();
		if (is_capturing)
		{
			app.get_handler().stop_capture();
		}

		app.get_db().destroy_capture_data();

		if (is_capturing)
		{
			app.get_handler().start_capture();
		}
		response->write("{}");
	};

	server.resource["^/api/v1/count/packet$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		json j;
		j["count"] =  this->app.get_db().count_packets();
		response->write(j.dump(4));
	};

	server.resource["^/api/v1/schema$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		response->write(this->app.get_db().get_schema_json());
	};

	server.resource["^/api/v1/count/per-type$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		response->write(this->app.get_db().count_packets_per_type());
	};

	// Serve static resources
	server.default_resource["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
	{
		auto web_root_path = boost::filesystem::canonical("public");
		auto path = web_root_path / request->path;

		try
		{
			// We're an SPA so all unknown paths should load index.html
			if (!boost::filesystem::exists(path))
			{
				path = boost::filesystem::canonical(web_root_path);
			}

			// Check if path is within web_root_path
			if(std::distance(web_root_path.begin(), web_root_path.end()) > std::distance(path.begin(), path.end()) ||
			   !std::equal(web_root_path.begin(), web_root_path.end(), path.begin()))
				throw std::invalid_argument("path must be within root path");
			if(boost::filesystem::is_directory(path))
				path /= "index.html";

			SimpleWeb::CaseInsensitiveMultimap header;

			auto ifs = std::make_shared<std::ifstream>();
			ifs->open(path.string(), std::ifstream::in | std::ios::binary | std::ios::ate);

			if(*ifs)
			{
				auto length = ifs->tellg();
				ifs->seekg(0, std::ios::beg);

				if (path.extension().string()  == ".html")
				{
					header.emplace("Content-Type", "text/html");
				}
				else if (path.extension().string()  == ".css")
				{
					header.emplace("Content-Type", "text/css");
				}
				else if (path.extension().string()  == ".js")
				{
					header.emplace("Content-Type", "application/javascript");
				}
				else if (path.extension().string()  == ".png")
				{
					header.emplace("Content-Type", "image/png");
				}

				header.emplace("Content-Length", to_string(length));
				response->write(header);

				// Serve files; set mime type for scripts
				class FileServer
				{
				public:
					static void read_and_send(const std::shared_ptr<HttpServer::Response> &response, const std::shared_ptr<std::ifstream> &ifs)
					{
						// Read and send 128 KB at a time
						static std::vector<char> buffer(131072);
						std::streamsize read_length;
						if((read_length = ifs->read(&buffer[0], static_cast<std::streamsize>(buffer.size())).gcount()) > 0)
						{
							response->write(&buffer[0], read_length);
							if(read_length == static_cast<std::streamsize>(buffer.size()))
							{
								response->send([response, ifs](const SimpleWeb::error_code &ec)
								{
									if(!ec)
									{
										read_and_send(response, ifs);
									}
									else
									{
										std::cerr << "Connection interrupted" << std::endl;
									}
								});
							}
						}
					}
				};
				FileServer::read_and_send(response, ifs);
			}
			else
				throw std::invalid_argument("Could not read file");
		}
		catch(const std::exception &e)
		{
			response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + path.string() + ": " + e.what());
		}
	};

	// Run webserver
	runner = std::thread([this]()
	{
		this->server.start();
	});
}
