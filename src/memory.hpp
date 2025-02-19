#ifndef MEMORY_HPP
#define MEMORY_HPP


#include "node.hpp"
#include <hls_stream.h>


void sm_memory(
	hls::stream<bptr_t> &addrFifo1,
	hls::stream<Node> &nodeFifo1,
	Node *hbm
);

#endif
