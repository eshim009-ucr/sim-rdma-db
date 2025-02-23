#include "memory.hpp"


void sm_memory(FifoPairRefList& rwFifos, Node *hbm) {
	AddrNode n;
	for(FifoPair& readPair : rwFifos) {
		if (!readPair.addrFifo.empty()) {
			readPair.addrFifo.read(n.addr);
			readPair.nodeFifo.write_nb(hbm[n.addr]);
		}
	}
}
