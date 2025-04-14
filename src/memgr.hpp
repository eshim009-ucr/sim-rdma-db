#ifndef MEMORY_HPP
#define MEMORY_HPP


extern "C" {
#include "core/node.h"
};
#include <hls_stream.h>


//! @brief Contains an address and whether or not to change the lock status in
//! main memory before returning a result
struct RwOp {
	//! Address to read/write from/to
	bptr_t addr;
	//! For reads, will grab the lock before returning.
	//! For write, will release the lock before returning.
	bool lock;
};


typedef RwOp mread_req_t;
typedef Node mread_resp_t;
typedef AddrNode mwrite_t;


//! Yes, it is correct per the docs to not mark array primitives as references
//! because they're already pointerse
void sm_memory(
	//! [out] Streams of addresses to read from main memory
	hls::stream<mread_req_t> readReqFifos[2],
	//! [in]  Streams of read results from main memory
	hls::stream<mread_resp_t> readRespFifos[2],
	//! [out] Streams of writes to main memory
	hls::stream<mwrite_t> writeFifos[1],
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
	hls::stream<mread_req_t>& readReqFifo,
	//! [in]  Stream of read results from main memory
	hls::stream<mread_resp_t>& readRespFifo,
	//! [out] Stream of writes to main memory
	hls::stream<mwrite_t>& writeFifo
);


#endif
