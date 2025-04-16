#include "trace.hpp"
#include "insert.hpp"
extern "C" {
#include "core/insert.h"
};
#include <iostream>


void sm_insert(
	bptr_t& root,
	hls::stream<insert_in_t>& input,
	hls::stream<insert_out_t>& output,
	hls::stream<mread_req_t>& readReqFifo,
	hls::stream<mread_resp_t>& readRespFifo,
	hls::stream<mwrite_t>& writeFifo,
	Node *hbm
) {
	KvPair pair;

	if (!input.empty()) {
		input.read(pair);
		output.write(insert(&root, pair.key, pair.value, hbm));
	}
}
