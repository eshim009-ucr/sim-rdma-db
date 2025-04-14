extern "C" {
#include "core/memory.h"
#include "core/node.h"
};
#include "memgr.hpp"
#include <hls_stream.h>


Node mem_read(bptr_t address, mread_req_stream_t *req_stream, mread_resp_stream_t *resp_stream) {
	Node node;
	RwOp read_op = {.addr = address, .lock = false};
	hls::stream<mread_req_t>& addrFifo = *((hls::stream<mread_req_t>*) req_stream);
	hls::stream<mread_resp_t>& nodeFifo = *((hls::stream<mread_resp_t>*) resp_stream);

	addrFifo.write(read_op);
	while (nodeFifo.empty());
	nodeFifo.read(node);

	return node;
}


Node mem_read_lock(bptr_t address, mread_req_stream_t *req_stream, mread_resp_stream_t *resp_stream) {
	Node node;
	RwOp read_op = {.addr = address, .lock = true};
	hls::stream<mread_req_t>& addrFifo = *((hls::stream<mread_req_t>*) req_stream);
	hls::stream<mread_resp_t>& nodeFifo = *((hls::stream<mread_resp_t>*) resp_stream);

	addrFifo.write(read_op);
	while (nodeFifo.empty());
	nodeFifo.read(node);

	return node;
}


//! @brief Write a node to memory and unlock it
void mem_write_unlock(AddrNode *node, mwrite_stream_t *req_stream) {
	hls::stream<mwrite_t>& writeFifo = *((hls::stream<mwrite_t>*) req_stream);
	writeFifo.write(*node);
}
