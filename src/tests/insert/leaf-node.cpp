#include "leaf-node.hpp"
#include "../../krnl.hpp"
#include "../../ramstream.hpp"
#include "../test-helpers.hpp"
extern "C" {
#include "../../core/io.h"
#include "../../core/memory.h"
};
#include <iostream>
#include <cstdint>


bool leaf_node(KERNEL_ARG_DECS) {
	bool pass = true;
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
	INPUT_INSERT(0, 2)
	INPUT_INSERT(5, 3)
	INPUT_INSERT(3, 1)

	// Perform Operations
	krnl(KERNEL_ARG_VARS);
	hbm_dump((uint8_t*) hbm, 0, sizeof(Node), 4);

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
	dump_node_list(stdout, hbm);

	return pass;
}
