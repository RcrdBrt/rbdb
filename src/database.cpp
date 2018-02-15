#include "database.h"
#include "functions.h"
#include <string>
#include <rocksdb/db.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

Database::Database() {
	options.create_if_missing = true;
}

Database::~Database() {}

Database::Database(const Database& orig) : 
	options(orig.options),
	uuid_gen(orig.uuid_gen) {}

bool Database::exists(const std::string& table_name) const {
	return boost::filesystem::exists(table_name);
}

bool Database::insert(const std::string& table_name,
		const std::string& payload) {
	rocksdb::DB* db;
	rocksdb::Status s = rocksdb::DB::Open(options, table_name, &db);
	if (s.ok()) {
		std::string incr_id_str, registry, uuid;
		uuid = boost::uuids::to_string(uuid_gen());
		std::cout << "UUID: " << uuid << std::endl;
		s = db->Get(rocksdb::ReadOptions(), "incr_id", &incr_id_str);
		std::cout << "Incr_id_str: " << incr_id_str << std::endl;
		db->Get(rocksdb::ReadOptions(), "registry", &registry);
		std::cout << "Registry: " << registry << std::endl;
		json j;
		rocksdb::WriteBatch batch;
		if (s.ok()) { // db already existent (exists incr_id)
			j = json::parse(registry);
			long long incr_id = std::atoll(incr_id_str.c_str());
			incr_id_str = std::to_string(++incr_id);
			batch.Put("incr_id", incr_id_str);
			batch.Put(incr_id_str, uuid);
		} else { // new db
			j = json::parse("{}");
			batch.Put("incr_id", "1");
			batch.Put("1", uuid);
		}
		j[uuid] = payload;
		batch.Put(uuid, payload);
		batch.Put(utils::iso_time(), uuid);
		batch.Put("registry", j.dump());
		db->Write(rocksdb::WriteOptions(), &batch);
#ifdef DEBUG
		json debug_print;
		debug_print["uuid"] = uuid;
		debug_print["incr_id_str"] = incr_id_str;
		debug_print["payload"] = payload;
		debug_print["s.ok()?"] = s.ok();
		debug_print["registry"] = j.dump();
		std::cout << debug_print.dump(2) << std::endl;
#endif
		delete db;
		return true;
	} else {
		std::cout << "Cannot access the database: " 
			<< s.ToString() << std::endl;
	}

	return false;
}
