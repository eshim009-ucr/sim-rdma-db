#include "search.hpp"
extern "C" {
#include "core/search.h"
};


void sm_search(
	bptr_t const& root,
	hls::stream<search_in_t>& input,
	hls::stream<search_out_t>& output,
	Node const* hbm
) {
	bkey_t key;
	bstatusval_t dumy = {.status=SUCCESS, .value={.data=0}};
	if (!input.empty()) {
		input.read(key);
		// output.write(search(root, key, hbm));
		output.write(dumy);
	}
}
