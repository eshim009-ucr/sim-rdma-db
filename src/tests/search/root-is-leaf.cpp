#include "root-is-leaf.hpp"
#include "../../krnl.hpp"
#include "../../ramstream.hpp"
#include "../../operations.hpp"
#include "../test-helpers.hpp"
#include <iostream>
#include <cstdint>


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

	uint8_t *hbm
) {
	bool pass = true;
	bptr_t root = 0;
	hls::stream<search_in_t> input_log;
	uint_fast8_t ops_in, ops_out;
	search_in_t last_in;
	search_out_t last_out;
	Response last_resp;
	uint_fast64_t offset = REQUEST_OFFSET;

	// Set up initial state
	reset_mem(hbm);
	SET_IKV(root, 0, 1, 10)
	SET_IKV(root, 1, 2, 20)
	SET_IKV(root, 2, 4, 40)
	SET_IKV(root, 3, 5, 50)
	hbm_dump(hbm, 2);
	// Should fail
	INPUT_SEARCH(0)
	INPUT_SEARCH(3)
	INPUT_SEARCH(6)
	// Should succeed
	INPUT_SEARCH(1)
	INPUT_SEARCH(2)
	INPUT_SEARCH(4)
	INPUT_SEARCH(5)

	// Perform Operations
	RUN_KERNEL

	// Evalue Results
	offset = RESPONSE_OFFSET;
	while (!input_log.empty()) {
		input_log.read(last_in);
		last_resp = *((Response*) &hbm[offset]);
		last_out = last_resp.search;
		#ifdef VERBOSE
		std::cout << "Search(" << last_in << "): ";
		if (last_out.status != SUCCESS) {
			std::cout << "Error: "
				<< ERROR_CODE_NAMES[-last_out.status]
				<< '(' << (int) last_out.status << ')' << std::endl;
		} else {
			std::cout << last_out.value.data<< "\t0x" << std::hex
				<< last_out.value.data << std::dec << std::endl;
		}
		#endif
		if (last_in % 3 == 0) {
			if (last_out.status != NOT_FOUND) {
				std::cerr << "For search input " << last_in
					<< ": Expected NOT_FOUND, got "
					<< ERROR_CODE_NAMES[-last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
		} else {
			if (last_out.status != SUCCESS) {
				std::cerr << "For search input " << last_in
					<< ": Expected SUCCESS, got "
					<< ERROR_CODE_NAMES[-last_out.status]
					<< '(' << (int) last_out.status << ')' << std::endl;
				pass = false;
			}
			if (last_out.value.data != 10*last_in) {
				std::cerr << "For search input " << last_in
					<< ": Expected " << 10*last_in << ", got "
					<< last_out.value.data << std::endl;
				pass = false;
			}
		}
	}
	// Check for non-empty streams
	if (!input_log.empty()) {
		std::cerr << "Error: Response stream empty before input log stream" << std::endl;
		std::cerr << "Contains the data:";
		do {
			input_log.read(last_in);
			std::cerr << "\n\t" << last_in;
		} while (!input_log.empty());
		std::cerr << std::endl;
		pass = false;
	}

	return pass;
}
