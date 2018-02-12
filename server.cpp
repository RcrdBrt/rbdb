#include "server.h"
#include <string>
#include "config.h"
#include <iostream>

Server::Server() {}

Server::Server(const Config& conf) {
	server.config.address = conf.address;
	server.config.port = conf.port;

	server.resource["^/$"]["GET"] =
		[](Response res, Request req) {
			res->write("Hello World!\n");
		};
}

Server::~Server() {}

void Server::start() {
	server.start();
}
