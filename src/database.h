#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include <boost/uuid/uuid_generators.hpp>
#include "nlohmann/json.hpp"
#include <memory>
#include <rocksdb/db.h>

using json = nlohmann::json;
typedef std::shared_ptr<json> json_ptr;

struct Database {
	Database();
	~Database();
	Database(const Database& orig);
	bool insert(const std::string& table_name, const json_ptr payload);
	bool exists(const std::string& table_name) const;
	bool update_registry(const std::string& uuid, const json_ptr payload);
	const std::string get(const std::string& table_name, const std::string& key) const;

	rocksdb::Options options;
	boost::uuids::random_generator uuid_gen;
};

#endif
