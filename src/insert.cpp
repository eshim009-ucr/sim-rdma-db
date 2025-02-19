#include "include.hpp"


void sm_insert(
	//! [in] Root node of the tree to search
	bptr_t root,
	//! [in]  Keys to search for
	hls::stream<bkey_t> &input,
	//! [out] Results from searches
	hls::stream<ErrorCode> &output,
	//! [out] Addresses to read
	hls::stream<bptr_t> &rAddrFifo,
	//! [out] Addresses to write
	hls::stream<AddrNode> &wAddrFifo,
	//! [in]  Results of address reads
	hls::stream<Node> &nodeFifo
) {
	static Tracer t(root, rAddrFifo, nodeFifo);
	static enum {
		IDLE,
		TRAVERSE,
		RESPOND
	} state = IDLE;
	static bkey_t key;
	static bstatusval_t result;

	switch(state) {
		case IDLE:
			if (!input.empty()) {
				input.read(key);
				state = TRAVERSE;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step()) {
				t.output.read(result);
				if (result.status == SUCCESS) {
					wAddrFifo.write(result.value.ptr);
				}
				state = IDLE;
			}
			break;
	}
}
