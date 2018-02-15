#include "database.h"
#include "functions.h"
#include <string>
#include <rocksdb/db.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>

Database::Database() {
	options.create_if_missing = true;
	options.max_background_compactions = 4;
	options.max_background_flushes = 2;
	options.bytes_per_sync = 1048576;
}

Database::~Database() {}

Database::Database(const Database& orig) : uuid_gen(orig.uuid_gen) {}



bool Database::exists(const std::string& table_name) const {
	return boost::filesystem::exists(table_name);
}

bool Database::insert(const std::string& table_name, const json_ptr payload) {
	rocksdb::DB* db;
	rocksdb::Status s;
	long long incr_id;
	s = rocksdb::DB::Open(options, table_name, &db);
	if (!s.ok()) {
		delete db;
		std::cerr << s.ToString() << std::endl;
		return false;
	}
	std::string incr_id_str, uuid, payload_str;
	uuid = boost::uuids::to_string(uuid_gen());
	auto registry_thread = std::thread([=]{
			update_registry(uuid, payload);
			});
	payload_str = payload->dump();
	s = db->Get(rocksdb::ReadOptions(), "incr_id", &incr_id_str);
	rocksdb::WriteBatch batch;
	switch (s.code()) {
		case 0:
			// db already existent
			incr_id = std::atoll(incr_id_str.c_str());
			incr_id_str = std::to_string(++incr_id);
			batch.Put("incr_id", incr_id_str);
			batch.Put(incr_id_str, uuid);
			break;
		case 1:
			// new database
			batch.Put("incr_id", "1");
			batch.Put("1", uuid);
			break;
		default:
			registry_thread.join();
			delete db;
			std::cerr << s.ToString() << std::endl;
			return false;
	}
	batch.Put(uuid, payload_str);
	batch.Put(utils::iso_time(), uuid);
	db->Write(rocksdb::WriteOptions(), &batch);
#ifdef DEBUG
	std::cout << "JSON insert: ";
	json debug_print;
	debug_print["uuid"] = uuid;
	debug_print["id"] = incr_id_str;
	debug_print["payload"] = payload_str;
	std::cout << debug_print.dump(4) << std::endl;
#endif
	registry_thread.join();
	delete db;
	return true;
}

bool Database::update_registry(const std::string& uuid, const json_ptr payload) {
	rocksdb::DB* db;
	if (!payload->is_object()) return false;
	std::vector<std::string> keys_vector;
	for (json::iterator it = payload->begin(); it != payload->end(); ++it) {
		keys_vector.push_back(it.key());
	}
	rocksdb::DB::Open(options, "registry", &db);
	json keys_json(keys_vector);
	db->Put(rocksdb::WriteOptions(), uuid, keys_json.dump());
#ifdef DEBUG
	std::cout << "Registry entry: ";
	std::cout << keys_json.dump(4) << std::endl;
#endif
	delete db;
	return true;
}
