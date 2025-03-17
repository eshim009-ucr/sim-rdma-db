#include "leaf-node.hpp"
#include "../../krnl.hpp"
#include "../../operations.hpp"
#include "../test-helpers.hpp"
#include <iostream>
#include <cstdint>


bool leaf_node(
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
	bool pass = true;
	bptr_t root = 0;
	IoPairs opstream;
	hls::stream<KvPair> input_log;
	uint_fast8_t ops_in, ops_out;
	KvPair last_in;
	ErrorCode last_out;

	// Set up initial state
	reset_mem(hbm);
	// Should succeed
	last_in.key = 0; last_in.value.data = 2;
	INPUT(insert, last_in);
	last_in.key = 5; last_in.value.data = 3;
	INPUT(insert, last_in);
	last_in.key = 3; last_in.value.data = 1;
	INPUT(insert, last_in);

	// Perform Operations
	krnl(
		m_axis_tx_meta, m_axis_tx_data, s_axis_tx_status,
		m_axis_bram_write_cmd, m_axis_bram_read_cmd, m_axis_bram_write_data, s_axis_bram_read_data,
		s_axis_update,
		myBoardNum, RDMA_TYPE, exec,
		hbm, root,
		opstream
	);

	// Evalue Results
	while (!input_log.empty()) {
		input_log.read(last_in);
		opstream.insert.output.read(last_out);
		#ifdef VERBOSE
		std::cout << "Insert(k=" << last_in.key
			<< ", v=" << last_in.value.data << "): ";
		if (last_out != SUCCESS) {
			std::cout << "Error: "
				<< ERROR_CODE_NAMES[last_out]
				<< '(' << (int) last_out << ')' << std::endl;
		} else {
			std::cout << "SUCCESS" << std::endl;
		}
		#endif
		if (last_out != SUCCESS) {
			std::cerr << "For inesrt input k=" << last_in.key
				<< ", v=" << last_in.value.data
				<< ": Expected SUCCESS, got"
				<< ERROR_CODE_NAMES[last_out]
				<< '(' << (int) last_out << ')' << std::endl;
			pass = false;
		}
	}

	return pass;
}
