#include "functions.h"
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <numeric>

namespace io = boost::filesystem;

namespace utils {

	std::size_t dir_size(const std::string& path) {
		// basic alg for the dir size (in bytes), found on stackoverflow.com
		std::size_t size;
		for (io::recursive_directory_iterator it(path);
				it != io::recursive_directory_iterator();
				++it) {
			if (!io::is_directory(*it))
				size += io::file_size(*it);
		}

		return size;
	}

	bool enough_fs_space(const std::string& path,
			const std::size_t space_required) {
		if (io::space(path).available > space_required)
			return true;

		return false;
	}

	bool move_to_tmp(const std::string& path) {
		std::string tmp_path = "/tmp";
		if (enough_fs_space(tmp_path, dir_size(path))) {
			io::copy(path, tmp_path);
			return true;
		}

		return false;
	}

	void create_dir_and_go(const std::string& path) {
		io::create_directories(path);
		io::current_path(path);
	}

	// functor that compares 2 ISO 8601 timestamps up to the minutes
	bool iso_time_comp::operator()(const std::string& s1,
			const std::string& s2) const {
		return s1.compare(0, 16, s2, 0, 16) < 0;
	}

	std::string iso_time() {
		using namespace boost::posix_time;
		ptime t = microsec_clock::universal_time();

		return to_iso_extended_string(t);
	}

	void up_a_dir() {
		io::current_path("..");
	}

	std::string json_str_to_str(const json& j) {
		std::string res;
		if (j.is_string()) {
			res = j.dump();
			return res.substr(1, res.size()-2);
		}
		std::cout << "Error, passed json is not a string: " << j.dump(4) << std::endl;

		return "Error, passed json is not a string";
	}

	namespace db {
		bool exists(rocksdb::DB* db, json j) {

		}
	}
}
