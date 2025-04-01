#include "operations.hpp"
#include "krnl.hpp"


void krnl(
	//Outgoing RDMA
	hls::stream<pkt256>& m_axis_tx_meta,
	hls::stream<pkt64>& m_axis_tx_data,
	hls::stream<pkt64>& s_axis_tx_status,

	//Local BRAM
	hls::stream<pkt256>& m_axis_bram_write_cmd,
	hls::stream<pkt256>& m_axis_bram_read_cmd,
	hls::stream<pkt512>& m_axis_bram_write_data,
	hls::stream<pkt512>& s_axis_bram_read_data,

	//Incoming
	hls::stream<pkt64>& s_axis_update,

	//Book keeping
	int myBoardNum,

	int RDMA_TYPE,
	int exec,

	uint8_t *hbm,
	bptr_t root,
	hls::stream<Request>& requests,
	hls::stream<Response>& responses
) {

	#pragma HLS INTERFACE axis port = m_axis_tx_meta
	#pragma HLS INTERFACE axis port = m_axis_tx_data
	#pragma HLS INTERFACE axis port = s_axis_tx_status

	#pragma HLS INTERFACE axis port = m_axis_bram_write_cmd
	#pragma HLS INTERFACE axis port = m_axis_bram_read_cmd
	#pragma HLS INTERFACE axis port = m_axis_bram_write_data
	#pragma HLS INTERFACE axis port = s_axis_bram_read_data

	#pragma HLS INTERFACE axis port = s_axis_update

	#pragma HLS INTERFACE s_axilite port = myBoardNum
	#pragma HLS INTERFACE s_axilite port = RDMA_TYPE
	#pragma HLS INTERFACE s_axilite port = exec
	#pragma HLS INTERFACE s_axilite port = root

	#pragma HLS INTERFACE m_axi port = hbm offset = slave bundle = gmem depth = 0x1000
	#pragma HLS INTERFACE axis port = requests
	#pragma HLS INTERFACE axis port = responses

	pkt64 status;
	if (!s_axis_tx_status.empty()) {
		s_axis_tx_status.read(status);
	}
	// Dummies to prevent [HLS 214-313] from inputs not being used
	(void) m_axis_tx_meta.empty();
	(void) m_axis_tx_data.empty();
	// (void) m_axis_tx_status.empty();
	(void) m_axis_bram_write_cmd.empty();
	(void) m_axis_bram_read_cmd.empty();
	(void) m_axis_bram_write_data.empty();
	(void) s_axis_bram_read_data.empty();

	// #pragma HLS DATAFLOW
	static hls::stream<search_in_t> searchInput;
	static hls::stream<insert_in_t> insertInput;
	static hls::stream<search_out_t> searchOutput;
	static hls::stream<insert_out_t> insertOutput;
	static hls::stream<mread_req_t> readReqFifos[2];
	static hls::stream<mread_resp_t> readRespFifos[2];
	static hls::stream<mwrite_t> writeFifos[1];
	#pragma HLS stream variable=searchInput type=fifo depth=0x100
	#pragma HLS stream variable=insertInput type=fifo depth=0x100
	#pragma HLS stream variable=searchOutput type=fifo depth=0x100
	#pragma HLS stream variable=insertOutput type=fifo depth=0x100
	#pragma HLS stream variable=readReqFifos type=fifo depth=0x100
	#pragma HLS stream variable=readRespFifos type=fifo depth=0x100
	#pragma HLS stream variable=writeFifos type=fifo depth=0x100

	uint_fast32_t stepCount = 0;

	while (stepCount++ < exec) {
		sm_search(
			root,
			searchInput, searchOutput,
			readReqFifos[1], readRespFifos[1]
		);
		sm_insert(
			root,
			insertInput, insertOutput,
			readReqFifos[0], readRespFifos[0],
			writeFifos[0]
		);
		sm_memory(
			readReqFifos, readRespFifos,
			writeFifos,
			(Node*) hbm
		);
		sm_decode(requests, searchInput, insertInput);
		sm_encode(responses, searchOutput, insertOutput);
	}

	return;
}
