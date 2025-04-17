#ifndef INSERT_HPP
#define INSERT_HPP


#include "memgr.hpp"
#include "types.hpp"
extern "C" {
#include "core/node.h"
};
#include <hls_stream.h>


typedef KvPair insert_in_t;
typedef ErrorCode insert_out_t;


//! @brief State machine to execute insert operations
void sm_insert(
	//! [in]  Root node of the tree to insert into
	bptr_t& root,
	//! [in]  Key/value pairs to insert
	hls::stream<insert_in_t>& input,
	//! [out] Status codes from inserts
	hls::stream<insert_out_t>& output,
	Node *hbm
);


#endif
