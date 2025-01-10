extern "C" {
#include "b-link-tree/tree.h"
};
#include <hls_stream.h>

#ifndef HLS_TREE
#define HLS_TREE

void sm_search(
    Tree& tree,
    hls::stream<bkey_t> &input,
    hls::stream<bstatusval_t> &output,
    hls::stream<int> &search2mem,
    hls::stream<Node> &mem2search
);

#endif
