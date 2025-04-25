#include "insert.hpp"
extern "C" {
#include "core/insert.h"
};


bool sm_insert(
	bptr_t& root,
	hls::stream<insert_in_t>& input,
	hls::stream<insert_out_t>& output,
	Node *hbm
) {
	KvPair pair;

	if (!input.empty() && !output.full()) {
		input.read(pair);
		output.write(insert(&root, pair.key, pair.value, hbm));
	}

	return input.empty() && output.empty();
}
