#ifndef RBDB_EXCEPTIONS_H
#define RBDB_EXCEPTIONS_H

#include <boost/exception/all.hpp>
#include <string>

typedef boost::error_info<struct tag_exc_info, std::string> exc_info;

struct config_error : virtual boost::exception, virtual std::exception {};

struct db_error : virtual boost::exception, virtual std::exception {};

#endif

