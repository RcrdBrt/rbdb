#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
	int port;
	std::string address, dir_path;

	Config();
	Config(const std::string& path);
	Config(const Config& orig);
	virtual ~Config();
};


#endif
