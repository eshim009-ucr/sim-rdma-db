#include "memgr.hpp"


//! The HLS interface to HBM does not support atomic test-and-set operations.
//! Atomic test-and-set operations within the FPGA fabric are allowed.
//! Serializing test-and-set operations in memory with mutual exclusion ensures
//! that race conditions do not lead to unexpected concurrent modification.
//!
//! @todo Set up multiple locks for specific regions of memory, such as by
//! address ranges or hashes to allow higher write bandwidth.
static Node hbm_read_lock(Node* hbm, bptr_t addr) {
	static lock_t local_readlock = 0;
	Node tmp;
	lock_p(&local_readlock);
	// Read the given address from main memory until its lock is released
	do {
		tmp = hbm[addr];
	} while(lock_test(&tmp.lock));
	// After the lock is released, we can safely acquire it because no other
	// modules concurrently hold this function's lock
	lock_p(&tmp.lock);
	// Write back the locked value to main memory
	hbm[addr] = tmp;
	// Release the local lock for future writers
	lock_v(&local_readlock);
	return tmp;
}


static void mem_read(
	hls::stream<mread_req_t>& addrFifo,
	hls::stream<mread_resp_t>& nodeFifo,
	Node *hbm
) {
	RwOp read_op;
	Node tmp;
	if (!addrFifo.empty()) {
		// Pop the address to read
		addrFifo.read(read_op);
		assert(read_op.addr != INVALID);
		// Try to grab lock if requested
		//! @todo Prevent this from blocking other reads in the FIFO.
		//! Or at least ensure parallel for loop execution so it only backs
		//! up one FIFO.
		if (read_op.lock) {
			tmp = hbm_read_lock(hbm, read_op.addr);
		} else {
			tmp = hbm[read_op.addr];
		}
		// Read HBM value and push to the stack
		nodeFifo.write_nb(tmp);
	}
}


void sm_memory(
	hls::stream<mread_req_t> readReqFifos[1],
	hls::stream<mread_resp_t> readRespFifos[1],
	Node *hbm
) {
	mem_read(readReqFifos[0], readRespFifos[0], hbm);
}
