#include "memory.hpp"
#include <iostream>


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
	std::cout << "\t\t[memory.cpp] About to grab lock for HBM..." << std::endl;
	lock_p(&local_readlock);
	std::cout << "\t\t[memory.cpp] Got HBM lock!" << std::endl;
	// Read the given address from main memory until its lock is released
	do {
		tmp = hbm[addr];
	} while(lock_test(&tmp.lock));
	// After the lock is released, we can safely acquire it because no other
	// modules concurrently hold this function's lock
	std::cout << "\t\t[memory.cpp] About to grab lock for hbm[0x"
	<< std::hex << addr << std::dec << "]..." << std::endl;
	lock_p(&tmp.lock);
	std::cout << "\t\t[memory.cpp] Got hbm[0x"
		<< std::hex << addr << std::dec << "]'s lock!" << std::endl;
	// Write back the locked value to main memory
	hbm[addr] = tmp;
	// Release the local lock for future writers
	lock_v(&local_readlock);
	return tmp;
}


static void mem_read(
	MemReadReqStream& addrFifo,
	MemReadRespStream& nodeFifo,
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
		std::cout << "\t\t[memory.cpp] hbm[0x"
			<< std::hex << read_op.addr << std::dec << "] has " << std::flush;
		for (li_t i = 0; i < TREE_ORDER; ++i) {
			if (tmp.keys[i] == INVALID) {
				std:: cout << "EMPTY" << std::flush;
			} else {
				std::cout << "{"
					<< "k=" << tmp.keys[i] << "}"
					<< ", v=" << tmp.values[i].data << "}"
					<< std::flush;
			}
			if (i < TREE_ORDER-1) {
				std::cout << ", " << std::flush;
			}
		}
		std::cout << std::endl;
		// Read HBM value and push to the stack
		nodeFifo.write_nb(tmp);
	}
}


static void mem_write(
	MemWriteStream& writeFifo,
	Node *hbm
) {
	AddrNode node;
	if (!writeFifo.empty()) {
		// Pop the address & data to write to
		writeFifo.read(node);
		// Check address validity
		assert(node.addr < MEM_SIZE);
		assert(node.addr != INVALID);
		// Perform write
		hbm[node.addr] = node;
	}
}


void sm_memory(
	MemReadReqStream readReqFifos[2],
	MemReadRespStream readRespFifos[2],
	MemWriteStream writeFifos[1],
	Node *hbm
) {
	mem_read(readReqFifos[0], readRespFifos[0], hbm);
	mem_write(writeFifos[0], hbm);
	mem_read(readReqFifos[1], readRespFifos[1], hbm);
}


ErrorCode alloc_sibling(
	AddrNode& old_node,
	AddrNode& sibling,
	MemReadReqStream& readReqFifo,
	MemReadRespStream& readRespFifo,
	MemWriteStream& writeFifo
) {
	static bptr_t stack_ptr = 0;
	sibling.addr = stack_ptr - sizeof(Node);
	// Ensure stack top is actually empty
	// Can eliminate outside of tests
	do {
		sibling.addr += sizeof(Node);
		if (sibling.addr > MEM_SIZE) {
			return OUT_OF_MEMORY;
		}
		readReqFifo.write({.addr=sibling.addr, .lock=0});
		//! @todo Do this better
		// Wait for read to complete
		while (readRespFifo.empty());
		readRespFifo.read(sibling);
	} while(is_valid(sibling));

	// Lock node
	lock_p(&sibling.lock);
	writeFifo.write(sibling);

	// Adjust next node pointers
	sibling.next = old_node.next;
	old_node.next = sibling.addr;

	// Move half of old node's contents to new node
	for (li_t i = 0; i < TREE_ORDER/2; ++i) {
		sibling.keys[i] = old_node.keys[i + (TREE_ORDER/2)];
		sibling.values[i] = old_node.values[i + (TREE_ORDER/2)];
		old_node.keys[i + (TREE_ORDER/2)] = INVALID;
	}

	return SUCCESS;
}
