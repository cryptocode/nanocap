#include <webserver.hpp>
#include <app.hpp>
#include <db.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

nanocap::webserver::webserver(nanocap::app& app) : app (app)
{
	// TODO: use app.config
	server.config.address = "0.0.0.0";
	server.config.port = 7077;
	start();
}

void nanocap::webserver::start()
{
	server.resource["^/api/v1/count/packet$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		json j;
		j["count"] =  this->app.get_db().count_packets();
		response->write(j.dump(4));
	};
	
	server.resource["^/api/v1/schema$"]["GET"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		response->write(this->app.get_db().get_schema_json());
	};


	// GET /info
	// Responds with request-information
	server.resource["^/info$"]["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		std::stringstream stream;
		stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";
		
		stream << request->method << " " << request->path << " HTTP/" << request->http_version;
		
		stream << "<h2>Query Fields</h2>";
		auto query_fields = request->parse_query_string();
		for(auto &field : query_fields)
			stream << field.first << ": " << field.second << "<br>";
		
		stream << "<h2>Header Fields</h2>";
		for(auto &field : request->header)
			stream << field.first << ": " << field.second << "<br>";
		
		response->write(stream);
	};

	runner = std::thread([this]() {
		this->server.start();
	});
}

nanocap::webserver::~webserver()
{
	server.stop();
	if (runner.joinable())
	{
		runner.join();
	}
}
