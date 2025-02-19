#ifndef INSERT_HPP
#define INSERT_HPP


#include "node.hpp"
#include <hls_stream.h>


void sm_insert(
	//! [in] Root node of the tree to search
	bptr_t root,
	//! [in]  Keys to search for
	hls::stream<bkey_t> &input,
	//! [out] Results from searches
	hls::stream<ErrorCode> &output,
	//! [out] Addresses to read
	hls::stream<bptr_t> &addrFifo,
	//! [in]  Results of address reads
	hls::stream<Node> &nodeFifo
);


#endif
