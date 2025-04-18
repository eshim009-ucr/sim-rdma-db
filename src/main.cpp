#include "tests/tests.hpp"
#include "operations.hpp"
#include <iostream>


#define PARAM_LIST \
	s_axis_tx_meta, \
	s_axis_tx_data, \
	m_axis_tx_status, \
	s_axis_bram_write_cmd, \
	s_axis_bram_read_cmd, \
	s_axis_bram_write_data, \
	m_axis_bram_read_data, \
	m_axis_update, \
	myBoardNum, \
	RDMA_TYPE, \
	exec, \
	hbm, \
	req_buffer, \
	resp_buffer


int main() {
	int myBoardNum = 1, RDMA_TYPE = 4, exec = 1000000;
	uint8_t hbm[sizeof(Node) * MEM_SIZE];
	uint8_t req_buffer[sizeof(Request) * 0x100];
	uint8_t resp_buffer[sizeof(Response) * 0x100];
	uint passed = 0, failed = 0;
	hls::stream<pkt256> s_axis_tx_meta;
	hls::stream<pkt64> s_axis_tx_data;
	hls::stream<pkt64> m_axis_tx_status;
	hls::stream<pkt256> s_axis_bram_write_cmd;
	hls::stream<pkt256> s_axis_bram_read_cmd;
	hls::stream<pkt512> s_axis_bram_write_data;
	hls::stream<pkt512> m_axis_bram_read_data;
	hls::stream<pkt64> m_axis_update;

	std::cout << "\n\n=== Search Tests ===" << std::endl;
	std::cout << "--- Root is Leaf ---" << std::endl;
	if (root_is_leaf(PARAM_LIST)) {
		std::cout << "\nPassed!\n" << std::endl;
		passed++;
	} else {
		std::cerr << "\nFailed!\n" << std::endl;
		failed++;
	}
	std::cout << "--- One Internal ---" << std::endl;
	if (one_internal(PARAM_LIST)) {
		std::cout << "\nPassed!\n" << std::endl;
		passed++;
	} else {
		std::cerr << "\nFailed!\n" << std::endl;
		failed++;
	}

	std::cout << "\n\n=== Insert Tests ===" << std::endl;
	std::cout << "--- Leaf Node ---" << std::endl;
	if (leaf_node(PARAM_LIST)) {
		std::cout << "\nPassed!\n" << std::endl;
		passed++;
	} else {
		std::cerr << "\nFailed!\n" << std::endl;
		failed++;
	}
	std::cout << "--- Split Root ---" << std::endl;
	if (split_root(PARAM_LIST)) {
		std::cout << "\nPassed!\n" << std::endl;
		passed++;
	} else {
		std::cerr << "\nFailed!\n" << std::endl;
		failed++;
	}
	std::cout << "--- Insert Until It Breaks ---" << std::endl;
	if (until_it_breaks(PARAM_LIST)) {
		std::cout << "\nPassed!\n" << std::endl;
		passed++;
	} else {
		std::cerr << "\nFailed!\n" << std::endl;
		failed++;
	}

	std::cout << "\n" << passed << " tests passed, "
		<< failed << " tests failed." << std::endl;

	return 0;
}
