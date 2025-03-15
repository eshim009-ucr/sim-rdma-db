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

	Node *hbm
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

	static hls::stream<bkey_t> searchInput;
	static hls::stream<bstatusval_t> searchOutput;
	static hls::stream<KvPair> insertInput;
	static hls::stream<ErrorCode> insertOutput;
	static std::array<FifoPair,2> readFifoList;
	static std::array<FifoPair,1> writeFifoList;
	bstatusval_t searchResult;
	ErrorCode insertResult;

	static bptr_t root;

	Node *n = &hbm[0];
	bval_t result;

	n->keys[0] = 1; n->values[0].data = 10;
	n->keys[1] = 2; n->values[1].data = 20;
	n->keys[2] = 4; n->values[2].data = 40;
	n->keys[3] = 5; n->values[3].data = 50;

	searchInput.write(0);
	searchInput.write(3);
	searchInput.write(6);
	searchInput.write(1);
	searchInput.write(2);
	searchInput.write(4);
	searchInput.write(5);
	insertInput.write({.key=1, .value=11});


	uint_fast32_t opsCount = searchInput.size() + insertInput.size();
	uint_fast32_t opsIn = 0;
	uint_fast32_t opsOut = 0;

	while (opsOut < opsCount) {
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

		while (!searchOutput.empty()) {
			searchOutput.read(searchResult);
			std::cout << "Search Result: ";
			if (searchResult.status != SUCCESS) {
				std::cout << "ERROR " << ERROR_CODE_NAMES[searchResult.status] << std::endl;
			} else if (searchResult.value.data == INVALID) {
				std::cout << "Invalid" << std::endl;
			} else {
				std::cout << searchResult.value.data << "\t0x" << std::hex
						  << searchResult.value.data << std::dec << std::endl;
			}
			opsOut++;
		}
		while (!insertOutput.empty()) {
			insertOutput.read(insertResult);
			std::cout << "Insert Result: " << ERROR_CODE_NAMES[insertResult] << std::endl;
		}
	}

	return;
}
