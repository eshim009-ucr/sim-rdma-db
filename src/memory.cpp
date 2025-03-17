#include "memory.hpp"


void sm_memory(
	std::array<FifoPair, 2>& readFifos,
	std::array<FifoPair, 1>& writeFifos,
	Node *hbm
) {
	// #pragma HLS dataflow
	RwOp read_op, write_op;
	Node write_node;
	for(FifoPair& readPair : readFifos) {
		if (!readPair.addrFifo.empty()) {
			// Pop the address to read
			readPair.addrFifo.read(read_op);
			assert(read_op.addr != INVALID);
			// Try to grab lock if requested
			//! @todo Prevent this from blocking other reads in the FIFO.
			//! Or at least ensure parallel for loop execution so it only backs
			//! up one FIFO.
			if (read_op.lock) {
				lock_p(&hbm[read_op.addr].lock);
			}
			// Read HBM value and push to the stack
			readPair.nodeFifo.write_nb(hbm[read_op.addr]);
		}
	}
	for(FifoPair& writePair : writeFifos) {
		if (!writePair.addrFifo.empty() && !writePair.nodeFifo.empty()) {
			// Pop the address to write to
			writePair.addrFifo.read(write_op);
			assert(write_op.addr != INVALID);
			// Pop the data to write
			writePair.nodeFifo.read(write_node);
			//! Unlock if requested
			lock_v(&hbm[write_op.addr].lock);
			// Perform HBM write
			hbm[write_op.addr] = write_node;
		}
	}
}


ErrorCode alloc_sibling(
	AddrNode& old_node,
	AddrNode& sibling,
	FifoPair& readFifos,
	FifoPair& writeFifos
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
		readFifos.addrFifo.write({.addr=sibling.addr, .lock=0});
		//! @todo Do this better
		// Wait for read to complete
		while (readFifos.nodeFifo.empty());
		readFifos.nodeFifo.write(sibling);
	} while(sibling.is_valid());

	// Lock node
	writeFifos.addrFifo.write({.addr=sibling.addr, .lock=1});
	writeFifos.nodeFifo.write(sibling);

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
