#include "trace.hpp"
#include "insert.hpp"
extern "C" {
#include "core/insert-helpers.h"
#include "core/split.h"
};

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
		NON_FULL,
		SPLIT
	} state = IDLE;
	static KvPair pair;
	static AddrNode leaf, sibling, parent;
	static ErrorCode status;
	static bool keep_splitting = false;
	static li_t i_leaf;


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
				state = NON_FULL;
			} else {
				parent = t.get_node();
			}
			break;
		case LOAD_PARENT;
		case NON_FULL:
			leaf = t.get_node();
			if (!is_full(&leaf.node)) {
				status = insert_nonfull(&leaf.node, pair.key, pair.value);
				lock_v(&leaf.node.lock);
				writeFifo.write(leaf);
				output.write(status);
				state = IDLE;
			} else {
				state = SPLIT;
			}
			break;
		case SPLIT:
			// Try to split this node
			status = split_node(&root, &leaf, &parent, &sibling, &readReqFifo, &readRespFifo, &writeFifo);
			keep_splitting = (status == PARENT_FULL);
			// Unrecoverable failure
			if (status != SUCCESS && status != PARENT_FULL) {
				lock_v(&leaf.node.lock); writeFifo.write(leaf);
				lock_v(&sibling.node.lock); writeFifo.write(sibling);
				lock_v(&parent.node.lock); writeFifo.write(parent);
				output.write(status);
				state = IDLE;
				break;
			}
			// Insert the new content and unlock leaf and its sibling
			status = insert_after_split(pair.key, pair.value, &leaf, &sibling, &writeFifo);
			if (keep_splitting) {
				// Try this again on the parent
				pair.key = max(&sibling.node);
				rekey(&parent.node, pair.key, max(&leaf.node));
				pair.value.ptr = sibling.addr;
				i_leaf--;
				leaf = parent;
			} else if (status != SUCCESS) {
				lock_v(&parent.node.lock);
				writeFifo.write(parent);
				output.write(status);
				state = IDLE;
			}
			break;
	}
}
