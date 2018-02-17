#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include <boost/uuid/uuid_generators.hpp>
#include "nlohmann/json.hpp"
#include <memory>
#include <rocksdb/db.h>

using json = nlohmann::json;

class Database {
private:
	
public:
	Database();
	~Database();
	Database(const Database& orig);
	bool insert(const std::string& table_name, const json payload);
	bool exists(const std::string& table_name) const;
	bool update_registry(const std::string& uuid, const json payload);
	std::string get(const std::string& table_name, const std::string& key) const;
	bool exists(rocksdb::DB* db, json j) const;

	rocksdb::Options options;
	boost::uuids::random_generator uuid_gen;
};

#endif
