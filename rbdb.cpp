#include <iostream>
#include "src/config.h"
#include "src/server.h"

int main() {
	Server(Config("/etc/rbdb_conf.toml")).start();

	return 0;
}
