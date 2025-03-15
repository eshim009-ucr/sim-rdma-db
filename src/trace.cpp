#include "trace.hpp"


Tracer::Tracer(bptr_t root) {
	// Traversals always start from the root
	history[0] = root;
}


bool Tracer::sm_step(
	hls::stream<RwOp> &addrFifo,
	hls::stream<Node> &nodeFifo
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
			if (!addrFifo.full()) {
				addrFifo.write({.addr=node.addr, .lock=0});
				state = CHECK_NODE;
			}
			break;
		case CHECK_NODE:
			if (!nodeFifo.empty()) {
				nodeFifo.read(node);
				if (node.is_leaf()) {
					// Search for the exact key within the node
					result = node.find_value(key);
					state = DONE;
					return true;
				} else {
					// Try to traverse to the next node
					result = node.find_next(key);
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


void Tracer::reset(bkey_t key) {
	this->key = key;
	state = RESET;
}

bstatusval_t Tracer::get_result() const {
	return result;
}

AddrNode Tracer::get_node() const {
	return node;
}
