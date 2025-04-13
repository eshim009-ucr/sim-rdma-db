#ifndef SEARCH_HPP
#define SEARCH_HPP


#include "memgr.hpp"
extern "C" {
#include "core/node.h"
};
#include <hls_stream.h>


typedef bkey_t search_in_t;
typedef bstatusval_t search_out_t;


//! @brief State machine to execute search operations
void sm_search(
	//! [in]  Root node of the tree to search
	bptr_t const& root,
	//! [in]  Keys to search for
	hls::stream<search_in_t>& input,
	//! [out] Results from searches
	hls::stream<search_out_t>& output,
	//! [out] Stream of addresses to read from main memory
	hls::stream<mread_req_t>& readReqFifo,
	//! [in]  Stream of read results from main memory
	hls::stream<mread_resp_t>& readRespFifo
);


#endif
