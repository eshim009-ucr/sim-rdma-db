#include "root-is-leaf.hpp"
#include "../../krnl.hpp"
#include <iostream>
#include <cstdint>


#define INPUT(x) input.write(x); input_log.write(x);


bool root_is_leaf(
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
	hls::stream<bkey_t> input;
	hls::stream<bkey_t> input_log;
	hls::stream<bstatusval_t> output;
	uint_fast8_t ops_in, ops_out;
	bkey_t last_in;
	bstatusval_t last_out;

	// Set up initial state
	hbm[0].keys[0] = 1; hbm[0].values[0].data = 10;
	hbm[0].keys[1] = 2; hbm[0].values[1].data = 20;
	hbm[0].keys[2] = 4; hbm[0].values[2].data = 40;
	hbm[0].keys[3] = 5; hbm[0].values[3].data = 50;
	// Should fail
	INPUT(0);
	INPUT(3);
	INPUT(6);
	// Should succeed
	INPUT(1);
	INPUT(2);
	INPUT(4);
	INPUT(5);

	// Perform Operations
	krnl(
		m_axis_tx_meta, m_axis_tx_data, s_axis_tx_status,
		m_axis_bram_write_cmd, m_axis_bram_read_cmd, m_axis_bram_write_data, s_axis_bram_read_data,
		s_axis_update,
		myBoardNum, RDMA_TYPE, exec,
		hbm,
		input, output
	);

	// Evalue Results
	while (!input_log.empty()) {
		input_log.read(last_in);
		output.read(last_out);
		#ifdef VERBOSE
		std::cout << "Search(" << last_in << "): ";
		if (last_out.status != SUCCESS) {
			std::cout << "Error: "
				<< ERROR_CODE_NAMES[last_out.status]
				<< '(' << (int) last_out.status << ')' << std::endl;
		} else if (last_out.value.data == INVALID) {
			std::cout << "Invalid Data" << std::endl;
		} else {
			std::cout << last_out.value.data<< "\t0x" << std::hex
				<< last_out.value.data << std::dec << std::endl;
		}
		#endif
		if (last_in % 3 == 0) {
			if (last_out.status != NOT_FOUND) {
				std::cerr << "For search input " << last_in
					<< ": Expected NOT_FOUND, got"
					<< ERROR_CODE_NAMES[last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
		} else {
			if (last_out.status != SUCCESS) {
				std::cerr << "For search input " << last_in
					<< ": Expected SUCCESS, got"
					<< ERROR_CODE_NAMES[last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
			if (last_out.value.data != 10*last_in) {
				std::cerr << "For search input " << last_in
					<< ": Expected " << 10*last_in << ", got"
					<< last_out.value.data << std::endl;
				pass = false;
			}
		}
	}

	return pass;
}
