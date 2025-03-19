#include "split-root.hpp"
#include "../../krnl.hpp"
#include "../../operations.hpp"
#include "../test-helpers.hpp"
#include <iostream>
#include <cstdint>


bool split_root(
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

	uint8_t *hbm
) {
	bool pass = true;
	bptr_t root = 0;
	hls::stream<Request> requests;
	hls::stream<Response> responses;
	hls::stream<insert_in_t> input_log;
	uint_fast8_t ops_in, ops_out;
	insert_in_t last_in;
	insert_out_t last_out;
	Response last_resp;

	// Set up initial state
	reset_mem(hbm);
	// Should succeed
	INPUT_INSERT(0, 0)
	INPUT_INSERT(5, -5)
	INPUT_INSERT(3, -3)
	INPUT_INSERT(1, -1)
	INPUT_INSERT(4, -4)

	// Perform Operations
	RUN_KERNEL

	// Evalue Results
	while (!input_log.empty()) {
		input_log.read(last_in);
		responses.read(last_resp);
		last_out = last_resp.insert;
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
			std::cerr << "For insert input k=" << last_in.key
				<< ", v=" << last_in.value.data
				<< ": Expected SUCCESS, got"
				<< ERROR_CODE_NAMES[last_out]
				<< '(' << (int) last_out << ')' << std::endl;
			pass = false;
		}
	}

	return pass;
}
