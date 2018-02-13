#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "rocksdb/db.h"
#include "nlohmann/json.hpp"

namespace utils {

	std::size_t dir_size(const std::string& path);

	bool enough_fs_space(const std::string& path,
			const std::size_t space_required);

	bool move_to_tmp(const std::string& path);

	void create_dir_and_go(const std::string& path);

	struct iso_time_comp {
		bool operator()(const std::string& s1, const std::string& s2) const;
	};

	std::string iso_time();

	void up_a_dir();
}

#endif

