#include "config.h"
#include <string>
#include "exceptions.h"

#include <iostream>
#include "toml/toml.h"

Config::Config() {}

Config::Config(const Config& orig) {
	port = orig.port;
	address = orig.address;
	dir_path = orig.dir_path;
}

Config::Config(const std::string& path) {
	std::ifstream ifs(path);
	toml::ParseResult pr = toml::parse(ifs);
	if (!pr.valid()) {
		std::cout << pr.errorReason << std::endl;
		throw config_error() << exc_info("Config is not a valid toml file");
	}
	const toml::Value& v = pr.value;
	const toml::Value* x;
	// server address from config file
	x = v.find("server.listen_addr");
	if (x && x->is<std::string>()) {
		address = x->as<std::string>();
	} else {
		throw config_error() << exc_info("Server addr error");
	}
	// server port from config file
	x = v.find("server.port");
	if (x && x->is<int>()) {
		port = x->as<int>();
	} else {
		throw config_error() << exc_info("Server port error");
	}
	// database path from config file
	x = v.find("database.path");
	if (x && x->is<std::string>()) {
		dir_path = x->as<std::string>();
	} else {
		throw config_error() << exc_info("Database path error");
	}
}

Config::~Config() {}
