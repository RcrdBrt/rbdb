#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include <boost/uuid/uuid_generators.hpp>

struct Database {
	Database();
	~Database();
	Database(const Database& orig);
	bool insert(const std::string& table_name, const std::string& payload);
	bool exists(const std::string& table_name) const;

	rocksdb::Options options;
	boost::uuids::random_generator uuid_gen;
};

#endif
