#ifndef SERVER_H
#define SERVER_H

#include "server_http.hpp"
#include "config.h"
#include <memory>
#include <string>
#include "database.h"
#include "nlohmann/json.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using Request = std::shared_ptr<HttpServer::Request>;
using Response = std::shared_ptr<HttpServer::Response>;
using json = nlohmann::json;
typedef std::shared_ptr<std::string> str_ptr;

class Server {
public:
	Server();
	Server(const Config& conf);
	~Server();
	void start();
private:
	HttpServer server;
};

const str_ptr res_str(const std::string& content_length,
		const std::string& content);
const str_ptr res_str(const std::size_t& content_length,
		const std::string& content);

#endif
