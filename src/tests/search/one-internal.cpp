#include "one-internal.hpp"
#include "../../krnl.hpp"
#include "../../ramstream.hpp"
#include "../../operations.hpp"
#include "../test-helpers.hpp"
extern "C" {
#include "../../src/core/memory.h"
};
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

	Node *hbm,
	Request *req_buffer,
	Response *resp_buffer
) {
	bool pass = true;
	bptr_t root = MAX_LEAVES;
	hls::stream<search_in_t> input_log;
	uint_fast8_t ops_in, ops_out;
	search_in_t last_in;
	search_out_t last_out;
	Response last_resp;
	uint_fast64_t offset = 0;

	// Set up initial state
	mem_reset_all(hbm);
	reset_ramstream_offsets();
	// Root
	SET_IKV(root, 0, 5, 1)
	SET_IKV(root, 1, 11, 2)
	// Left Child
	SET_IKV(1, 0, 1, -1)
	SET_IKV(1, 1, 2, -2)
	SET_IKV(1, 2, 4, -4)
	SET_IKV(1, 3, 5, -5)
	// Right Child
	SET_IKV(2, 0, 7, -7)
	SET_IKV(2, 1, 8, -8)
	SET_IKV(2, 2, 10, -10)
	SET_IKV(2, 3, 11, -11)
	hbm_dump((uint8_t*) hbm, 0, sizeof(Node), 4);
	// Should fail
	INPUT_SEARCH(0)
	INPUT_SEARCH(3)
	INPUT_SEARCH(6)
	INPUT_SEARCH(9)
	INPUT_SEARCH(12)
	// Should succeed
	INPUT_SEARCH(1)
	INPUT_SEARCH(2)
	INPUT_SEARCH(4)
	INPUT_SEARCH(5)
	INPUT_SEARCH(7)
	INPUT_SEARCH(8)
	INPUT_SEARCH(10)
	INPUT_SEARCH(11)
	hbm_dump((uint8_t*) req_buffer, 0, sizeof(Request), 15);
	hbm_dump((uint8_t*) resp_buffer, 0, sizeof(Response), 15);

	// Perform Operations
	RUN_KERNEL

	// Evalue Results
	offset = 0;
	while (!input_log.empty()) {
		input_log.read(last_in);
		last_resp = resp_buffer[offset++];
		last_out = last_resp.search;
		#ifdef VERBOSE
		std::cout << "Search(" << last_in << "): ";
		if (last_out.status != SUCCESS) {
			std::cout << "Error: "
				<< ERROR_CODE_NAMES[last_out.status]
				<< '(' << (int) last_out.status << ')' << std::endl;
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
	hbm_dump((uint8_t*) req_buffer, 0, sizeof(Request), 15);
	hbm_dump((uint8_t*) resp_buffer, 0, sizeof(Response), 15);

	return pass;
}
