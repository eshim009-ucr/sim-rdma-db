#ifndef INSERT_HPP
#define INSERT_HPP

#include "memory.hpp"
#include "node.hpp"
#include <hls_stream.h>

//! @brief State machine to execute insert operations
void sm_insert(
	//! [in]    Root node of the tree to insert into
	bptr_t& root,
	//! [in]    Key/value pairs to insert
	hls::stream<KvPair> &input,
	//! [out]   Status codes from inserts
	hls::stream<ErrorCode> &output,
	//! [inout] Memory reads
	FifoPair &readFifos,
	//! [inout] Memory writes
	FifoPair &writeFifos
);

#endif
