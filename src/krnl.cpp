#include "krnl.hpp"
#include "search.hpp"

const int NUM_NODES = 2;


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

	//Local HBM
	int *network_ptr
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

	#pragma HLS DATAFLOW


	static hls::stream<bkey_t> searchInput;
	static hls::stream<bstatusval_t> searchOutput;
	static hls::stream<bptr_t> searchAddrFifo;
	static hls::stream<Node> searchNodeFifo;
	bstatusval_t searchResult;

	Node *root = (Node *) &network_ptr[0];
	bval_t result;

	root->keys[0] = 1; root->values[0].data = 10;
	root->keys[1] = 2; root->values[1].data = 20;
	root->keys[2] = 4; root->values[2].data = 40;
	root->keys[3] = 5; root->values[3].data = 50;

	searchInput.write(0);
	searchInput.write(3);
	searchInput.write(6);
	searchInput.write(1);
	searchInput.write(2);
	searchInput.write(4);
	searchInput.write(5);


	uint_fast32_t opsCount = searchInput.size();
	uint_fast32_t opsIn = 0;
	uint_fast32_t opsOut = 0;

	while (opsOut < opsCount) {
		sm_search(
			0,
			searchInput,
			searchOutput,
			searchAddrFifo,
			searchNodeFifo
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
	}

	return;
}
