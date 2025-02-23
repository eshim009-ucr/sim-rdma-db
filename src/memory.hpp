#ifndef MEMORY_HPP
#define MEMORY_HPP


#include "node.hpp"
#include <hls_stream.h>
#include <list>
#include <functional>


//! @brief Pair of address and data streams to use for memory lookups
//!
//! Can be used for reads or writes
typedef struct {
	//! @brief Stream of addresses to read/write from
	hls::stream<bptr_t> addrFifo;
	//! @brief Stream of вфеф to read/write to
	hls::stream<Node> nodeFifo;
} FifoPair;

typedef std::list<std::reference_wrapper<FifoPair>> FifoPairRefList;


void sm_memory(FifoPairRefList& rwFifos, Node *hbm);


#endif
