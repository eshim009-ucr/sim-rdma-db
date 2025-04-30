#include "operations.hpp"
#include "ramstream.hpp"
#include "krnl.hpp"


void krnl(
	int myBoardNum,
	int RDMA_TYPE,
	int exec,
	uint64_t *hbm,
	uint64_t *req_buffer,
	uint64_t *resp_buffer,
	bptr_t root
) {

	#pragma HLS INTERFACE s_axilite port = myBoardNum
	#pragma HLS INTERFACE s_axilite port = RDMA_TYPE
	#pragma HLS INTERFACE s_axilite port = exec
	#pragma HLS INTERFACE s_axilite port = root

	#pragma HLS INTERFACE m_axi port = hbm bundle = gmem0
	#pragma HLS INTERFACE m_axi port = req_buffer bundle = gmem1
	#pragma HLS INTERFACE m_axi port = resp_buffer bundle = gmem2

	static hls::stream<Request> requests;
	static hls::stream<Response> responses;
	static hls::stream<search_in_t> searchInput;
	static hls::stream<insert_in_t> insertInput;
	static hls::stream<search_out_t> searchOutput;
	static hls::stream<insert_out_t> insertOutput;
	#pragma HLS stream variable=requests type=fifo depth=0x100
	#pragma HLS stream variable=requests type=fifo depth=0x100
	#pragma HLS stream variable=searchInput type=fifo depth=0x100
	#pragma HLS stream variable=insertInput type=fifo depth=0x100
	#pragma HLS stream variable=searchOutput type=fifo depth=0x100
	#pragma HLS stream variable=insertOutput type=fifo depth=0x100

	uint_fast32_t stepCount = 0;

	while (stepCount++ < exec) {
		sm_search(
			root,
			searchInput, searchOutput,
			(Node*) hbm
		);
		sm_insert(
			root,
			insertInput, insertOutput,
			(Node*) hbm
		);
		sm_ramstream_req(requests, (Request*) req_buffer);
		sm_ramstream_resp(responses, (Response*) resp_buffer);
		sm_decode(requests, searchInput, insertInput);
		sm_encode(responses, searchOutput, insertOutput);
	}

	return;
}
