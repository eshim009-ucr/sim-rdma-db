#ifndef SEARCH_HPP
#define SEARCH_HPP


#include "memory.hpp"
#include "node.hpp"
#include <hls_stream.h>


struct SearchIO {
	//! Keys to search for
	hls::stream<bkey_t> input;
	//! Results from searches
	hls::stream<bstatusval_t> output;
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
