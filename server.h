#ifndef SERVER_H
#define SERVER_H

#include "server_http.hpp"
#include "config.h"
#include <memory>

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using Request = std::shared_ptr<HttpServer::Request>;
using Response = std::shared_ptr<HttpServer::Response>;

class Server {
public:
	Server();
	Server(const Config& conf);
	~Server();
	void start();
private:
	HttpServer server;
};

#endif
