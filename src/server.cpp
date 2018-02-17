#include "server.h"
#include <string>
#include "config.h"
#include <iostream>
#include <thread>
#include <memory>
#include "database.h"
#include "functions.h"
#include "nlohmann/json.hpp"

typedef std::shared_ptr<json> json_ptr;

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

	// insert
	server.resource["^/rbdb/([a-zA-Z_]+)(/{1})?$"]["POST"] =
		[](Response res, Request req) {
			Database db;
			std::string payload = req->content.string();
			std::string table_name = req->path_match[1];
			json payload_json;
			try {
				payload_json = json::parse(payload);
			} catch (json::parse_error& e) {
				std::cerr << e.what() << std::endl;
				res->write("HTTP/1.1 400 Bad Request\r\n");
				return;
			}

			if (db.insert(table_name, payload_json)) {
				res->write("HTTP/1.1 200 OK\r\n");
			} else {
				res->write("HTTP/1.1 500 Internal Server Error\r\n");
			}
		};

	server.resource["^/rbdb/([a-zA-Z_]+)(/{1})?$"]["GET"] =
		[](Response res, Request req) {
			json j;
			Database db;
			j["table"] = req->path_match[1];
			j["exists"] = db.exists(j["table"]);
			std::string content = j.dump(4);
			*res << *(res_str(content.length(), content));
		};
	
	server.resource["^/rbdb/([a-zA-Z_]+)/([a-zA-Z0-9_]+)(/{1})?$"]["GET"] =
		[](Response res, Request req) {
			Database db;
			std::string content = db.get(req->path_match[1],
					req->path_match[2]);
#ifdef DEBUG
			std::cout << "Get all request: ";
			json debug_print;
			debug_print["table_name"] = req->path_match[1];
			debug_print["key"] = req->path_match[2];
			std::cout << debug_print.dump(4) << std::endl;
#endif
			if (content.empty()) {
				*res << "HTTP/1.1 404 Not Found\r\n";
				return;
			}
			*res << *(res_str(content.length(), content));
		};
}

const str_ptr res_str(const std::string& content_length,
		const std::string& content) {
	const str_ptr p = std::make_shared<std::string>(
			"HTTP/1.1 200 OK\r\nContent-Length: "
		+ content_length + "\r\n\r\n" + content);
	
	return p;
}

const str_ptr res_str(const std::size_t& content_length,
		const std::string& content) {
	
	return res_str(std::to_string(content_length), content);
}

Server::~Server() {}

void Server::start() {
	server.start();
}
