#include "operations.hpp"
#include "ramstream.hpp"
#include "krnl.hpp"


void krnl(
	uint8_t *hbm,
	uint8_t *req_buffer,
	uint8_t *resp_buffer,
	bptr_t root,
	int loop_max,
	int op_max,
	bool reset
) {
	#pragma HLS INTERFACE m_axi port=hbm bundle=gmem0
	#pragma HLS INTERFACE m_axi port=req_buffer bundle=gmem1
	#pragma HLS INTERFACE m_axi port=resp_buffer bundle=gmem2
	#pragma HLS INTERFACE s_axilite port=root
	#pragma HLS INTERFACE s_axilite port=loop_max
	#pragma HLS INTERFACE s_axilite port=op_max
	#pragma HLS INTERFACE s_axilite port=reset

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

	uint_fast32_t step_count = 0;
	uint_fast32_t ops_in = 0;
	uint_fast32_t ops_out = 0;
	if (reset) {
		step_count = 0;
		ops_in = 0;
		ops_out = 0;
	}

	while (ops_out < op_max && step_count++ < loop_max) {
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
		sm_ramstream_req(requests, req_buffer);
		sm_ramstream_resp(responses, resp_buffer);
		sm_decode(requests, searchInput, insertInput, ops_in);
		sm_encode(responses, searchOutput, insertOutput, ops_out);
	}

	return;
}
