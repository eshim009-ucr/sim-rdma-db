#ifndef MEMORY_HPP
#define MEMORY_HPP


#include "node.hpp"
#include <hls_stream.h>
#include <list>
#include <functional>


//! @brief Contains an address and whether or not to change the lock status in
//! main memory before returning a result
struct RwOp {
	//! Address to read/write from/to
	bptr_t addr;
	//! For reads, will grab the lock before returning.
	//! For write, will release the lock before returning.
	bool lock;
};

//! @brief Pair of address and data streams to use for memory lookups
//!
//! Can be used for reads or writes
struct FifoPair {
	//! @brief Stream of addresses to read/write from
	hls::stream<RwOp> addrFifo;
	//! @brief Stream of nodes to read/write to
	hls::stream<Node> nodeFifo;
};


void sm_memory(
	//! List of FIFOs used when reading from memory
	std::array<FifoPair, 2>& readFifos,
	//! List of FIFOs used when writing to memory
	std::array<FifoPair, 1>& writeFifos,
	//! Pointer to high bandwidth memory
	Node *hbm
);


//! @brief Allocate a new sibling node in an empty slot in main mameory
//!
//! Acquires a lock on the sibling node
ErrorCode alloc_sibling(
	//! [in] Node to create a sibling to
	//!
	//! Not used in all allocation algorithms
	AddrNode& old_node,
	//! [out] Newly allocated sibling
	//!
	//! Not used in all allocation algorithms
	AddrNode& sibling,
	//! Pair of FIFOs used when reading from memory
	FifoPair& readFifos,
	//! Pair of FIFOs used when writing to memory
	FifoPair& writeFifos
);


#endif
