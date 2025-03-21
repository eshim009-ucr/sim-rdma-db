#include "trace.hpp"
#include <iostream>


bool Tracer::sm_step(
	MemReadReqStream& readReqFifo,
	MemReadRespStream& readRespFifo
) {
	switch(state) {
		case RESET:
			i = 0;
			state = IDLE;
			break;
		case IDLE:
			// (Re)start search at the root
			node.addr = history[0];
			state = READ_NODE;
			break;
		case READ_NODE:
			if (!readReqFifo.full()) {
				readReqFifo.write({.addr=node.addr, .lock=0});
				state = CHECK_NODE;
				std::cout << "\t\t[trace.cpp] Read node at 0x"
					<< std::hex << node.addr << std::dec << std::endl;
			}
			break;
		case CHECK_NODE:
			if (!readRespFifo.empty()) {
				readRespFifo.read(node);
				if (is_leaf(node)) {
					// Search for the exact key within the node
					result = find_value(node, key);
					state = DONE;
					std::cout << "\t\t[trace.cpp] Node at 0x"
						<< std::hex << node.addr << std::dec
						<< " is a leaf with k/v pair "
						<< key << ", " << result.value.data << std::endl;
					return true;
				} else {
					std::cout << "\t\t[trace.cpp] Node at 0x"
						<< std::hex << node.addr << std::dec
						<< " is internal" << std::endl;
					// Try to traverse to the next node
					result = find_next(node, key);
					// If error, stop looking
					if (result.status != SUCCESS) {
						state = DONE;
						std::cout << "\t\t[trace.cpp] Failed to find next node" << std::endl;
						return true;
					} else {
						// Save new address to current and stack
						node.addr = result.value.ptr;
						history[++i] = node.addr;
						state = READ_NODE;
						std::cout << "\t\t[trace.cpp] Found next node at 0x"
							<< std::hex << result.value.ptr << std::dec << std::endl;
					}
				}
			}
			break;
		case DONE:
			return true;
	}
	return false;
}


void Tracer::reset(bptr_t root, bkey_t key) {
	history[0] = root;
	this->key = key;
	state = RESET;
	std::cout << "\n\t\t[trace.cpp] Resetting tracer with root 0x"
		<< std::hex << root << std::dec << " key " << key << std::dec << std::endl;
}

bstatusval_t Tracer::get_result() const {
	return result;
}

AddrNode Tracer::get_node() const {
	return node;
}
