#include "insert.hpp"
extern "C" {
#include "core/insert.h"
};
#include <iostream>


void sm_insert(
	bptr_t& root,
	hls::stream<insert_in_t>& input,
	hls::stream<insert_out_t>& output,
	Node *hbm
) {
	KvPair pair;
	ErrorCode dummy = SUCCESS;

	if (!input.empty()) {
		input.read(pair);
		// output.write(insert(&root, pair.key, pair.value, hbm));
		output.write(dummy);
	}
}
