#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP


#include "search.hpp"
#include "insert.hpp"


//! @brief Pairs of all I/O Fifos for all operations
struct IoPairs {
	SearchIO search;
	InsertIO insert;
};


#endif
