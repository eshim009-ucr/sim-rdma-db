#include "krnl.hpp"
#include "search.hpp"
#include "insert.hpp"
#include "memory.hpp"


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
	bptr_t& root,

	hls::stream<bkey_t>& searchInput,
	hls::stream<bstatusval_t>& searchOutput
) {

	#pragma HLS INTERFACE axis port = m_axis_tx_meta
	#pragma HLS INTERFACE axis port = m_axis_tx_data
	#pragma HLS INTERFACE axis port = s_axis_tx_status

	#pragma HLS INTERFACE axis port = m_axis_bram_write_cmd
	#pragma HLS INTERFACE axis port = m_axis_bram_read_cmd
	#pragma HLS INTERFACE axis port = m_axis_bram_write_data
	#pragma HLS INTERFACE axis port = s_axis_bram_read_data

	#pragma HLS INTERFACE axis port = s_axis_update

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

	static hls::stream<KvPair> insertInput;
	static hls::stream<ErrorCode> insertOutput;
	static std::array<FifoPair,2> readFifoList;
	static std::array<FifoPair,1> writeFifoList;

	Node *n = &hbm[0];

	uint_fast32_t opsCount = searchInput.size() + insertInput.size();

	while (searchOutput.size() + insertOutput.size() < opsCount) {
		sm_search(
			root,
			searchInput,
			searchOutput,
			readFifoList[1].addrFifo,
			readFifoList[1].nodeFifo
		);
		sm_insert(
			root,
			insertInput,
			insertOutput,
			readFifoList[0],
			writeFifoList[0]
		);
		sm_memory(
			readFifoList,
			writeFifoList,
			hbm
		);
	}

	return;
}
