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

	Node *hbm,
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

	#pragma HLS INTERFACE m_axi port = hbm
	#pragma HLS INTERFACE axis port = requests
	#pragma HLS INTERFACE axis port = responses

	pkt64 status;
	if (!s_axis_tx_status.empty()) {
		s_axis_tx_status.read(status);
	}
	// Dummies to prevent [HLS 214-313] from inputs not being used
	if (!m_axis_tx_meta.empty()) {
	}
	if (!m_axis_tx_data.empty()) {
	}
	if (!m_axis_bram_write_cmd.empty()) {
	}
	if (!m_axis_bram_read_cmd.empty()) {
	}
	if (!m_axis_bram_write_data.empty()) {
	}
	if (!s_axis_bram_read_data.empty()) {
	}

	// #pragma HLS DATAFLOW
	static IoPairs opstream;
	static std::array<FifoPair,2> readFifoList;
	static std::array<FifoPair,1> writeFifoList;

	Node *n = &hbm[0];

	uint_fast32_t opsCount = requests.size();

	while (responses.size() < opsCount) {
		sm_search(
			root,
			opstream.search,
			readFifoList[1]
		);
		sm_insert(
			root,
			opstream.insert,
			readFifoList[0],
			writeFifoList[0]
		);
		sm_memory(
			readFifoList,
			writeFifoList,
			hbm
		);
		sm_decode(requests, opstream);
		sm_encode(responses, opstream);
	}

	return;
}
