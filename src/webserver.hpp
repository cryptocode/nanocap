#pragma once

#include <thread>
#include <webserver/client_http.hpp>
#include <webserver/server_http.hpp>

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

namespace nanocap
{
	class app;
	class webserver
	{
	public:
		webserver(nanocap::app& app);
		~webserver();
	private:
		nanocap::app& app;
		HttpServer server;
		std::thread runner;
		void start();
	};
}
