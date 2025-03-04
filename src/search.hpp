#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "node.hpp"
#include <hls_stream.h>

//! @brief State machine to execute search operations
void sm_search(
	//! [in] Root node of the tree to search
	bptr_t const& root,
	//! [in]  Keys to search for
	hls::stream<bkey_t> &input,
	//! [out] Results from searches
	hls::stream<bstatusval_t> &output,
	//! [out] Addresses to read
	hls::stream<bptr_t> &addrFifo,
	//! [in]  Results of address reads
	hls::stream<Node> &nodeFifo
);

#endif
