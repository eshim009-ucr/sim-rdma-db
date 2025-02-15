#include "b-link-tree/lock.h"
#include "b-link-tree/memory.h"
#include "b-link-tree/tree.h"

Node mem_read(bptr_t address) {
	Node n;
	return n;
}

Node mem_read_lock(bptr_t address) {
	// lock_p(&memory[address].lock);
	Node n;
	return n;
}

void mem_write_unlock(bptr_t address, Node node) {
	// lock_v(&node.lock);
	// memory[address] = node;
}

void mem_unlock(bptr_t address) {
	// lock_v(&memory[address].lock);
}

void mem_reset_all() {
	// memset(memory, INVALID, MEM_SIZE*sizeof(Node));
	// for (uint_fast8_t i = 0; i < MEM_SIZE; i++) {
	// 	init_lock(&memory[i].lock);
	// }
}
