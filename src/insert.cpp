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
	hls::stream<mwrite_t>& writeFifo,
	Node *hbm
) {
	static Tracer t;
	static enum {
		IDLE,
		TRAVERSE,
		BACKTRACK,
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
				parent.addr = INVALID;
				sibling.addr = INVALID;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step(readReqFifo, readRespFifo)) {
				state = NON_FULL;
			}
			break;
		case BACKTRACK:
			if (t.double_step_back(hbm)) {
				parent = t.get_node();
			} else {
				parent.addr = INVALID;
			}
			state = NON_FULL;
			break;
		case NON_FULL:
			leaf.addr = t.get_node().addr;
			leaf.node = mem_read_lock(t.get_node().addr, hbm);
			if (!is_full(&leaf.node)) {
				status = insert_nonfull(&leaf.node, pair.key, pair.value);
				mem_write_unlock(&leaf, hbm);
				output.write(status);
				state = IDLE;
			} else {
				state = SPLIT;
			}
			break;
		case SPLIT:
			parent = t.get_parent(hbm);
			// Try to split this node
			status = split_node(&root, &leaf, &parent, &sibling, hbm);
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
			status = insert_after_split(pair.key, pair.value, &leaf, &sibling, hbm);
			if (keep_splitting) {
				// Try this again on the parent
				pair.key = max(&sibling.node);
				rekey(&parent.node, pair.key, max(&leaf.node));
				pair.value.ptr = sibling.addr;
				leaf = parent;
				state = BACKTRACK;
			} else if (status != SUCCESS) {
				lock_v(&parent.node.lock);
				writeFifo.write(parent);
				output.write(status);
				state = IDLE;
			}
			break;
	}
}
