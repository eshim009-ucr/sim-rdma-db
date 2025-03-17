#include "search.hpp"
#include "trace.hpp"


void sm_search(
	//! [in] Root node of the tree to search
	bptr_t const& root,
	//! [in]  Keys to search for
	hls::stream<bkey_t> &input,
	//! [out] Results from searches
	hls::stream<bstatusval_t> &output,
	//! [out] Addresses to read
	hls::stream<RwOp> &addrFifo,
	//! [in]  Results of address reads
	hls::stream<Node> &nodeFifo
) {
	static Tracer t;
	static enum {
		IDLE,
		TRAVERSE
	} state = IDLE;
	static bkey_t key;
	static bstatusval_t result;

	switch(state) {
		case IDLE:
			if (!input.empty()) {
				input.read(key);
				t.reset(root, key);
				state = TRAVERSE;
			}
			break;
		case TRAVERSE:
			//! If reached a leaf
			if (t.sm_step(addrFifo, nodeFifo)) {
				result = t.get_result();
				output.write(result);
				state = IDLE;
			}
			break;
	}
}
