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
			Database db_instance;
			std::string payload = req->content.string();
			std::string table_name = req->path_match[1];
#ifdef DEBUG
			std::cout << "Payload: " << payload << std::endl;
			std::cout << "Table: " << table_name << std::endl;
#endif
			std::string content_type = 
				(req->header.find("Content-Type"))->second;
			if (content_type != "application/json") {
				std::cerr << "Payload isn't JSON!\n";
				res->write("HTTP/1.1 400 Bad Request\r\n");
				return;
			}
			json_ptr json_payload;
			try {
				json_payload =
					std::make_shared<json>(json::parse(payload));
			} catch (json::parse_error& e) {
				std::cerr << "Payload isn't JSON!:" << e.what();
				res->write("HTTP/1.1 400 Bad Request\r\n");
				return;
			}

			if (db_instance.insert(table_name, json_payload)) {
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
