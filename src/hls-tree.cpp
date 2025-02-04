extern "C" {
#include "b-link-tree/search.h"
};
#include "hls-tree.hpp"


void sm_search(
	Tree& tree,
	hls::stream<bkey_t> &input,
	hls::stream<bstatusval_t> &output,
	hls::stream<int> &search2mem,
	hls::stream<Node> &mem2search
) {
	static enum {
		INIT,
		READ,
		RESP
	} state = INIT;
	static bkey_t key;
	static bstatusval_t result;


	switch(state) {
		case INIT:
			if (!input.empty()) {
				state = READ;
			}
			break;
		case READ:
			input.read(key);
			state = RESP;
			break;
		case RESP:
			if (!output.full()) {
				result = search(&tree, key);
				output.write(result);
				state = INIT;
			}
			break;
	}
}
