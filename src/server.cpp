#include "server.h"
#include <string>
#include "config.h"
#include <iostream>
#include <thread>
#include <memory>
#include "database.h"
#include "functions.h"

Server::Server() {}

Server::Server(const Config& conf) {
	server.config.address = conf.address;
	server.config.port = conf.port;
	utils::create_dir_and_go(conf.dir_path);

	// root
	server.resource["^/rbdb(/{1})?$"]["GET"] =
		[](Response res, Request req) {
			res->write("Hello World!\n");
			*res << "HTTP/1.1 200 OK\r\n";
		};

	// create
	server.resource["^/rbdb/([a-zA-Z_]+)(/{1})?$"]["POST"] =
		[](Response res, Request req) {
			Database db_instance;
			std::string payload = req->content.string();
			std::string table_name = req->path_match[1];
#ifdef DEBUG
			std::cout << "Payload: " << payload << std::endl;
			std::cout << "Table: " << table_name << std::endl;
#endif
			if (db_instance.insert(table_name, payload)) {
				res->write("HTTP/1.1 200 OK\r\n");
			} else {
				res->write("HTTP/1.1 500 Internal Server Error\r\n");
			}
		};
}

Server::~Server() {}

void Server::start() {
	server.start();
}
