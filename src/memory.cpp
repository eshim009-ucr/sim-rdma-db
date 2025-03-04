#include "memory.hpp"


void sm_memory(
	FifoPairRefList& readFifos,
	FifoPairRefList& writeFifos,
	Node *hbm
) {
	AddrNode read_tmp, write_tmp;
	for(FifoPair& readPair : readFifos) {
		if (!readPair.addrFifo.empty()) {
			// Pop the address to read
			readPair.addrFifo.read(read_tmp.addr);
			// Read HBM value and push to the stack
			readPair.nodeFifo.write_nb(hbm[read_tmp.addr]);
		}
	}
	for(FifoPair& writePair : writeFifos) {
		if (!writePair.addrFifo.empty() && !writePair.nodeFifo.empty()) {
			// Pop the address to write to
			writePair.addrFifo.read(write_tmp.addr);
			// Pop the data to write
			writePair.nodeFifo.read(write_tmp);
			// Perform HBM write
			hbm[write_tmp.addr] = write_tmp;
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
		readFifos.addrFifo.write(sibling.addr);
		//! @todo Do this better
		// Wait for read to complete
		while (readFifos.nodeFifo.empty());
		readFifos.nodeFifo.write(sibling);
	} while(sibling.is_valid());

	// Lock node
	lock_p(&sibling.lock);
	writeFifos.addrFifo.write(sibling.addr);
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
