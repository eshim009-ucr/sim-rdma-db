#include "one-internal.hpp"
#include "../../krnl.hpp"
#include "../test-helpers.hpp"
#include <iostream>
#include <cstdint>


bool one_internal(
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
	// Root
	SET_IKV(0, 0, 1, 1);
	SET_IKV(0, 1, 2, 2);
	// Left Child
	SET_IKV(1, 0, 1, -1);
	SET_IKV(1, 1, 2, -2);
	SET_IKV(1, 2, 4, -4);
	SET_IKV(1, 3, 5, -5);
	// Right Child
	SET_IKV(2, 0, 7, -7);
	SET_IKV(2, 1, 8, -8);
	SET_IKV(2, 2, 10, -10);
	SET_IKV(2, 3, 11, -11);
	// Should fail
	INPUT(0);
	INPUT(3);
	INPUT(6);
	INPUT(9);
	INPUT(12);
	// Should succeed
	INPUT(1);
	INPUT(2);
	INPUT(4);
	INPUT(5);
	INPUT(7);
	INPUT(8);
	INPUT(10);
	INPUT(11);

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
				std::cerr << "\tFor search input " << last_in
					<< ": Expected NOT_FOUND, got "
					<< ERROR_CODE_NAMES[last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
		} else {
			if (last_out.status != SUCCESS) {
				std::cerr << "\tFor search input " << last_in
					<< ": Expected SUCCESS, got "
					<< ERROR_CODE_NAMES[last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
			if (last_out.value.data != -last_in) {
				std::cerr << "\tFor search input " << last_in
					<< ": Expected " << (bdata_t) -last_in << ", got "
					<< last_out.value.data << std::endl;
				pass = false;
			}
		}
	}

	return pass;
}
