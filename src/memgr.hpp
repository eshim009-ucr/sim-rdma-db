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
	//! Pointer to high bandwidth memory
	Node *hbm
);


#endif
