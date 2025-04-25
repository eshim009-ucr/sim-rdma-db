#include "search.hpp"
extern "C" {
#include "core/search.h"
};


bool sm_search(
	bptr_t const& root,
	hls::stream<search_in_t>& input,
	hls::stream<search_out_t>& output,
	Node const* hbm
) {
	bkey_t key;
	if (!input.empty() && !output.full()) {
		input.read(key);
		output.write(search(root, key, hbm));
	}

	return input.empty() && output.empty();
}
