#include "trace.hpp"


bool Tracer::sm_step(
	hls::stream<mread_req_t>& readReqFifo,
	hls::stream<mread_resp_t>& readRespFifo
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
			}
			break;
		case CHECK_NODE:
			if (!readRespFifo.empty()) {
				readRespFifo.read(node.node);
				if (is_leaf(node.addr)) {
					// Search for the exact key within the node
					result = find_value(&node.node, key);
					state = DONE;
					return true;
				} else {
					// Try to traverse to the next node
					result = find_next(&node.node, key);
					// If error, stop looking
					if (result.status != SUCCESS) {
						state = DONE;
						return true;
					} else {
						// Save new address to current and stack
						node.addr = result.value.ptr;
						history[++i] = node.addr;
						state = READ_NODE;
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
}

bstatusval_t Tracer::get_result() const {
	return result;
}

AddrNode Tracer::get_node() const {
	return node;
}


AddrNode Tracer::get_parent(Node *memory) const {
	AddrNode node;
	if (i >= 1) {
		node.addr = history[i-1];
		node.node = mem_read_lock(node.addr, memory);
	} else {
		node.addr = INVALID;
	}
	return node;
}

bool Tracer::double_step_back(Node *memory) {
	if (i >= 2) {
		i -= 2;
		node.addr = history[i];
		node.node = mem_read_lock(node.addr, memory);
		return true;
	} else {
		return false;
	}
}
