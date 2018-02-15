#include "database.h"
#include "functions.h"
#include <string>
#include <rocksdb/db.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

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
			delete db;
			std::cerr << s.ToString() << std::endl;
			return false;
	}
	batch.Put(uuid, payload_str);
	batch.Put(utils::iso_time(), uuid);
	db->Write(rocksdb::WriteOptions(), &batch);
	update_registry(uuid, payload);
#ifdef DEBUG
	std::cout << "JSON insert: ";
	json debug_print;
	debug_print["uuid"] = uuid;
	debug_print["id"] = incr_id_str;
	debug_print["payload"] = payload_str;
	std::cout << debug_print.dump(4) << std::endl << std::endl;
	std::cout << "-----------------------------" << std::endl << std::endl;
#endif
	delete db;
	return true;
}

bool Database::update_registry(const std::string& uuid, const json_ptr payload) {
	rocksdb::DB* db;
	rocksdb::WriteBatch batch;
	rocksdb::Status s;
	json contains_key_json;
	std::vector<std::string> keys_vector;
	std::string contains_key_string;
	if (!payload->is_object()) return false;
	rocksdb::DB::Open(options, "registry", &db);
	for (json::iterator it = payload->begin(); it != payload->end(); ++it) {
		keys_vector.push_back(it.key());
		s = db->Get(rocksdb::ReadOptions(), it.key(), &contains_key_string);
		if (s.IsNotFound()) {
			contains_key_json.push_back(uuid);
		} else {
			if (s.ok()) {
				contains_key_json = json::parse(contains_key_string);
				contains_key_json.push_back(uuid);
			} else {
				//invalid status
				delete db;
				return false;
			}
		}
		db->Put(rocksdb::WriteOptions(), it.key(), contains_key_json.dump());
#ifdef DEBUG
		std::cout << "Contains_key_json (" + it.key() + "): ";
		std::cout << contains_key_json.dump(4) << std::endl;
#endif
		contains_key_json.clear();
	}
	json keys_json(keys_vector);
	batch.Put(uuid, keys_json.dump());
	db->Write(rocksdb::WriteOptions(), &batch);
#ifdef DEBUG
	std::cout << "Registry entry (keys list): ";
	std::cout << keys_json.dump(4) << std::endl;
#endif
	delete db;
	return true;
}

const std::string Database::get(const std::string& table_name, const std::string& key) const {
	rocksdb::DB* db;
	rocksdb::DB* registry_db;
	rocksdb::Status s;
	std::string contains_key_string;
	json refined_result, result_tmp;
	s = rocksdb::DB::Open(options, table_name, &db);
	if (!s.ok()) {
		delete db;
		return nullptr;
	}
	s = rocksdb::DB::Open(options, "registry", &registry_db);
	if (!s.ok()) {
		delete db;
		delete registry_db;
		return nullptr;
	}
	registry_db->Get(rocksdb::ReadOptions(), key, &contains_key_string);
	result_tmp = json::parse(contains_key_string);
	if (!result_tmp.is_array()) {
		delete db, registry_db;
		return nullptr;
	}
	for (auto& i : result_tmp) {
		db->Get(rocksdb::ReadOptions(), i.dump(), &contains_key_string); // contains_key_string is a container
		refined_result.push_back(contains_key_string);
	}
	delete db;
	delete registry_db;

	return refined_result.dump();
}
