#include "search.hpp"
#include "trace.hpp"


void sm_search(
	bptr_t const& root,
	hls::stream<search_in_t>& input,
	hls::stream<search_out_t>& output,
	hls::stream<mread_req_t>& readReqFifo,
	hls::stream<mread_resp_t>& readRespFifo
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
			if (t.sm_step(readReqFifo, readRespFifo)) {
				result = t.get_result();
				output.write(result);
				state = IDLE;
			}
			break;
	}
}
