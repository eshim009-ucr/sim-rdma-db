#include "memory.hpp"


static void mem_read(
	MemReadReqStream& addrFifo,
	MemReadRespStream& nodeFifo,
	Node *hbm
) {
	RwOp read_op;
	if (!addrFifo.empty()) {
		// Pop the address to read
		addrFifo.read(read_op);
		assert(read_op.addr != INVALID);
		// Read HBM value and push to the stack
		nodeFifo.write_nb(hbm[read_op.addr]);
	}
}


void sm_memory(
	MemReadReqStream readReqFifos[2],
	MemReadRespStream readRespFifos[2],
	MemWriteStream writeFifos[1],
	Node *hbm
) {
	mem_read(readReqFifos[0], readRespFifos[0], hbm);
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
