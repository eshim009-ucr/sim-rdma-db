#include "memory.hpp"

void sm_memory(
	hls::stream<bptr_t> &addrFifo1,
	hls::stream<Node> &nodeFifo1,
	Node *hbm
) {
	AddrNode n;
	if (!addrFifo1.empty()) {
		addrFifo1.read(n.addr);
		nodeFifo1.write_nb(hbm[n.addr]);
	}
}
