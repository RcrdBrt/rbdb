#include <iostream>
#include "src/config.h"
#include "src/server.h"

int main() {
	Server(Config("conf.toml")).start();

	return 0;
}
