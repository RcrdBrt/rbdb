#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "rocksdb/db.h"
#include <boost/uuid/uuid_generators.hpp>
#include "nlohmann/json.hpp"
#include <memory>

using json = nlohmann::json;
typedef std::shared_ptr<json> json_ptr;

struct Database {
	Database();
	~Database();
	Database(const Database& orig);
	bool insert(const std::string& table_name, const json_ptr payload);
	bool exists(const std::string& table_name) const;

	rocksdb::Options options;
	boost::uuids::random_generator uuid_gen;
};

#endif
