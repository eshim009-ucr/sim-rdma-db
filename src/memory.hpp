extern "C" {
#include "b-link-tree/memory.h"
}

#include <hls_stream.h>

void sm_memory(
	hls::stream<int> &search2mem,
	hls::stream<Node> &mem2search,
	hls::stream<Node> &cst2mem,
	hls::stream<int> &mem2cst,
	hls::stream<Node> &insert2mem,
	hls::stream<int> &mem2insert,
	hls::stream<int> &index2mem,
	hls::stream<Node> &overflow2mem,
	hls::stream<Node> &mem2overflow,
	hls::stream<Node> &overflow2reinsert,
	hls::stream<Node> &overflow2split,
	hls::stream<Node> &split2overflow,
	hls::stream<int> &removeInputLevel,
	hls::stream<int> &removeInputIndex,
	hls::stream<int> &removeLevel2mem,
	hls::stream<int> &removeIndex2mem,
	hls::stream<int> &mem2removeLevel,
	hls::stream<int> &mem2removeIndex,
	hls::stream<Node> &mem2node,
	//! Pointer to high-bandwidth memory
	Node *hbm
);
