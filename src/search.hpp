#ifndef SEARCH_HPP
#define SEARCH_HPP


#include "memory.hpp"
#include "node.hpp"
#include <hls_stream.h>


typedef bkey_t search_in_t;
typedef bstatusval_t search_out_t;


struct SearchIO {
	//! Keys to search for
	hls::stream<search_in_t> input;
	//! Results from searches
	hls::stream<search_out_t> output;
};


//! @brief State machine to execute search operations
void sm_search(
	//! [in]    Root node of the tree to search
	bptr_t const& root,
	//! [inout] Input and output for search operations
	SearchIO& ops,
	//! [inout] Pairs of FIFOs for reading from main memory
	FifoPair& readFifos
);


#endif
