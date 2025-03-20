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


typedef hls::stream<RwOp> MemReadReqStream;
typedef hls::stream<Node> MemReadRespStream;
typedef hls::stream<AddrNode> MemWriteStream;


//! Yes, it is correct per the docs to not mark array primitives as references
//! because they're already pointerse
void sm_memory(
	//! [out] Streams of addresses to read from main memory
	MemReadReqStream readReqFifos[2],
	//! [in]  Streams of read results from main memory
	MemReadRespStream readRespFifos[2],
	//! [out] Streams of writes to main memory
	MemWriteStream writeFifos[1],
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
	//! [out] Stream of addresses to read from main memory
	MemReadReqStream& readReqFifo,
	//! [in]  Stream of read results from main memory
	MemReadRespStream& readRespFifo,
	//! [out] Stream of writes to main memory
	MemWriteStream& writeFifo
);


#endif
