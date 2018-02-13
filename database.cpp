#include "database.h"
#include "functions.h"
#include <string>
#include <rocksdb/db.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
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

bool Database::insert(const std::string& table_name,
		const std::string& payload) {
	rocksdb::DB* db;
	rocksdb::Status s = rocksdb::DB::Open(options, table_name, &db);
	if (s.ok()) {
		std::string incr_id_str, registry, uuid;
		uuid = boost::uuids::to_string(uuid_gen());
		s = db->Get(rocksdb::ReadOptions(), "incr_id", &incr_id_str);
		db->Get(rocksdb::ReadOptions(), "registry", &registry);
		json j = json::parse(registry);
		j[uuid] = payload;
		rocksdb::WriteBatch batch;
		if (s.ok()) { // db already existent (exists incr_id)
			long long incr_id = std::atoll(incr_id_str.c_str());
			incr_id_str = std::to_string(++incr_id);
			// starts the write
			batch.Put("incr_id", incr_id_str);
			batch.Put(incr_id_str, uuid);
		} else { // new db
			// starts the write
			batch.Put("incr_id", "1");
			batch.Put("1", uuid);
		}
		batch.Put(uuid, payload);
		batch.Put(utils::iso_time(), uuid);
		batch.Put("registry", j.dump());
		db->Write(rocksdb::WriteOptions(), &batch);
		delete db;
		return true;
	} else {
		std::cout << "Cannot create the database: " 
			<< s.ToString() << std::endl;
	}

	return false;
}
