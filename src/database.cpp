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
}

Database::~Database() {}

Database::Database(const Database& orig) : 
	options(orig.options),
	uuid_gen(orig.uuid_gen) {}

bool Database::exists(const std::string& table_name) const {
	return boost::filesystem::exists(table_name);
}

bool Database::insert(const std::string& table_name,
		const json_ptr payload) {
	rocksdb::DB* db;
	rocksdb::Status s = rocksdb::DB::Open(options, table_name, &db);
	std::string incr_id_str, registry, uuid, payload_str;
	payload_str = payload->dump();
	uuid = boost::uuids::to_string(uuid_gen());
	s = db->Get(rocksdb::ReadOptions(), "incr_id", &incr_id_str);
	if (!s.ok()) {
		std::cerr << s.ToString() << std::endl;
		return false;
	}
	db->Get(rocksdb::ReadOptions(), "registry", &registry);
#ifdef DEBUG
	std::cout << "UUID: " << uuid << std::endl;
	std::cout << "Incr_id_str: " << incr_id_str << std::endl;
	std::cout << "Registry: " << registry << std::endl;
#endif
	json reg;
	rocksdb::WriteBatch batch;
	if (s.ok()) { // db already existent (exists incr_id)
		reg = json::parse(registry);
		long long incr_id = std::atoll(incr_id_str.c_str());
		incr_id_str = std::to_string(++incr_id);
		batch.Put("incr_id", incr_id_str);
		batch.Put(incr_id_str, uuid);
	} else { // new db
		reg = {};
		batch.Put("incr_id", "1");
		batch.Put("1", uuid);
	}
	reg[uuid] = payload_str;
	batch.Put(uuid, payload_str);
	batch.Put(utils::iso_time(), uuid);
	batch.Put("registry", reg.dump());
	db->Write(rocksdb::WriteOptions(), &batch);
#ifdef DEBUG
	json debug_print;
	debug_print["uuid"] = uuid;
	debug_print["incr_id_str"] = incr_id_str;
	debug_print["payload"] = payload_str;
	debug_print["s.ok()?"] = s.ok();
	debug_print["registry"] = reg.dump();
	std::cout << debug_print.dump(4) << std::endl;
#endif
	delete db;
	return true;
}
