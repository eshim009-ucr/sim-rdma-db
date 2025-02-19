#include "search.hpp"
#include "trace.hpp"


void sm_search(
	//! [in] Root node of the tree to search
	bptr_t root,
	//! [in]  Keys to search for
	hls::stream<bkey_t> &input,
	//! [out] Results from searches
	hls::stream<bstatusval_t> &output,
	//! [out] Addresses to read
	hls::stream<bptr_t> &addrFifo,
	//! [in]  Results of address reads
	hls::stream<Node> &nodeFifo
) {
	static Tracer t(root, addrFifo, nodeFifo);
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
				t.reset(key);
				state = TRAVERSE;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step()) {
				t.output.read(result);
				output.write(result);
				state = IDLE;
			}
			break;
	}
}
