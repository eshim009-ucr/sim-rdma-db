#ifndef INSERT_HPP
#define INSERT_HPP


#include "memory.hpp"
#include "node.hpp"
#include <hls_stream.h>


typedef KvPair insert_in_t;
typedef ErrorCode insert_out_t;


struct InsertIO {
	//! Key/value pairs to insert
	hls::stream<insert_in_t> input;
	//! Status codes from inserts
	hls::stream<insert_out_t> output;
};


//! @brief State machine to execute insert operations
void sm_insert(
	//! [in]    Root node of the tree to insert into
	bptr_t& root,
	//! [inout] Input and output for search operations
	InsertIO& ops,
	//! [inout] Pairs of FIFOs for reading from main memory
	FifoPair &readFifos,
	//! [inout] Pairs of FIFOs for writing to main memory
	FifoPair &writeFifos
);


#endif
