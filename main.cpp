#include <iostream>
#include "config.h"
#include "server.h"

int main() {
	Server(Config("conf.toml")).start();

	return 0;
}
