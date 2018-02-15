#include <iostream>
#include "src/config.h"
#include "src/server.h"
#include "src/exceptions.h"

int main() {
	try {
		Server(Config("/etc/rbdb_conf.toml")).start();
	} catch(config_error& e) {
		std::cerr << *boost::get_error_info<exc_info>(e) << std::endl;
	}

	return 0;
}
