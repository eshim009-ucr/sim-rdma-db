#include "trace.hpp"
#include "insert.hpp"


//! @brief Divide by 2 and take the ceiling using only integer arithmetic
//! @param[in] x  The value to operate on
//! @return ceil(x/2)
#define DIV2CEIL(x) (((x) & 1) ? (((x)/2) + 1) : ((x)/2))


void sm_insert(
	bptr_t& root,
	hls::stream<insert_in_t>& input,
	hls::stream<insert_out_t>& output,
	hls::stream<mread_req_t>& readReqFifo,
	hls::stream<mread_resp_t>& readRespFifo,
	hls::stream<mwrite_t>& writeFifo
) {
	static Tracer t;
	static enum {
		IDLE,
		TRAVERSE,
		INSERT,
		SPLIT,
		SPLIT_ROOT,
		SPLIT_NON_ROOT
	} state = IDLE;
	static KvPair pair;
	static AddrNode node, sibling, parent;
	static ErrorCode status;
	// Iterators
	li_t i, i_insert;


	switch(state) {
		case IDLE:
			if (!input.empty()) {
				input.read(pair);
				t.reset(root, pair.key);
				state = TRAVERSE;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step(readReqFifo, readRespFifo)) {
				state = INSERT;
			} else {
				parent = t.get_node();
			}
			break;
		case INSERT:
			node = t.get_node();
			for (i = 0; i < TREE_ORDER; ++i) {
				// Found an empty slot
				// Will be the last slot
				if (node.keys[i] == INVALID) {
					// Scoot nodes if necessary to maintain ordering
					// Iterate right to left from the last node to the insertion point
					for (; i_insert < i; i--) {
						node.keys[i] = node.keys[i-1];
						node.values[i] = node.values[i-1];
					}
					// Do the actual insertion
					node.keys[i_insert] = pair.key;
					node.values[i_insert] = pair.value;
					writeFifo.write(node);
					// Done
					output.write(SUCCESS);
					state = IDLE;
					return; // Need to double-break
				} else if (node.keys[i] == pair.key) {
					output.write(KEY_EXISTS);
					state = IDLE;
					return; // Need to double-break
				} else if (node.keys[i] < pair.key) {
					i_insert++;
				}
			}
			// Didn't find a slot, need to split
			state = SPLIT;
			break;
		case SPLIT:
			status = alloc_sibling(node, sibling, readReqFifo, readRespFifo, writeFifo);
			if (status != SUCCESS) {
				state = IDLE;
				break;
			}
			if (node.addr == root) {
				state = SPLIT_ROOT;
			} else {
				state = SPLIT_NON_ROOT;
			}
			break;
		case SPLIT_ROOT:
			// If this is the only node
			// We need to create the first inner node
			if (is_leaf(node)) {
				// Make a new root node
				root = MAX_LEAVES;
			} else {
				root = root + MAX_NODES_PER_LEVEL;
				if (root >= MEM_SIZE) {
					status = NOT_IMPLEMENTED;
					state = IDLE;
					break;
				}
			}
			parent.addr = root;
			lock_p(&parent.lock);
			writeFifo.write(parent);
			clear(parent);
			parent.keys[0] = node.keys[DIV2CEIL(TREE_ORDER)-1];
			parent.values[0].ptr = node.addr;
			parent.keys[1] = sibling.keys[(TREE_ORDER/2)-1];
			parent.values[1].ptr = sibling.addr;
			status = SUCCESS;
			state = IDLE;
			break;
		case SPLIT_NON_ROOT:
			if (is_full(parent)) {
				status = NOT_IMPLEMENTED;
				state = IDLE;
				break;
			} else {
				for (li_t i = 0; i < TREE_ORDER; ++i) {
					// Update key of old node
					if (parent.values[i].ptr == node.addr) {
						parent.keys[i] = node.keys[DIV2CEIL(TREE_ORDER)-1];
						// Scoot over other nodes to fit in new node
						for (li_t j = TREE_ORDER-1; j > i; --j) {
							parent.keys[j] = parent.keys[j-1];
							parent.values[j] = parent.values[j-1];
						}
						// Insert new node
						parent.keys[i+1] = sibling.keys[(TREE_ORDER/2)-1];
						parent.values[i+1].ptr = sibling.addr;
						status = SUCCESS;
						state = IDLE;
						break;
					}
				}
				status = NOT_IMPLEMENTED;
				state = IDLE;
				break;
			}
			break;
	}
}
