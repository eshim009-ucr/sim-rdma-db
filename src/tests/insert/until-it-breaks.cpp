#include "until-it-breaks.hpp"
#include "../../krnl.hpp"
#include "../../ramstream.hpp"
#include "../../operations.hpp"
#include "../test-helpers.hpp"
extern "C" {
#include "../../src/core/memory.h"
};
#include <iostream>
#include <cstdint>


bool until_it_breaks(
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
	Request *req_buffer,
	Response *resp_buffer
) {
	bool pass = true;
	bptr_t root = 0;
	hls::stream<insert_in_t> input_log;
	uint_fast8_t ops_in, ops_out;
	insert_in_t last_in;
	insert_out_t last_out;
	Response last_resp;
	uint_fast64_t offset = 0;

	// Set up initial state
	mem_reset_all(hbm);
	reset_ramstream_offsets();
	// Should succeed
	for (uint_fast8_t i = 1; i <= (TREE_ORDER/2)*(MAX_LEAVES+1); ++i) {
		INPUT_INSERT(i, -i)
	}

	// Perform Operations
	RUN_KERNEL

	// Evalue Results
	offset = 0;
	while (!input_log.empty()) {
		input_log.read(last_in);
		last_resp = resp_buffer[offset++];
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
				<< ": Expected SUCCESS, got "
				<< ERROR_CODE_NAMES[last_out]
				<< '(' << (int) last_out << ')' << std::endl;
			pass = false;
		}
	}
	// Check for non-empty streams
	if (!input_log.empty()) {
		std::cerr << "Error: Response stream empty before input log stream" << std::endl;
		std::cerr << "Contains the data:";
		do {
			input_log.read(last_in);
			std::cerr << "\n\t k=" << last_in.key << " v=" << last_in.value.data;
		} while (!input_log.empty());
		std::cerr << std::endl;
		pass = false;
	}

	return pass;
}
