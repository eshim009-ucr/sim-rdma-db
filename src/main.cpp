#include "krnl.hpp"
#include "search.hpp"
#include <iostream>


int main() {
	int myBoardNum = 1, RDMA_TYPE = 4, exec = 1000000;
	int network_ptr[MEM_SIZE * (sizeof(Node)/sizeof(int))];
	hls::stream<pkt256> s_axis_tx_meta;
	hls::stream<pkt64> s_axis_tx_data;
	hls::stream<pkt64> m_axis_tx_status;
	hls::stream<pkt256> s_axis_bram_write_cmd;
	hls::stream<pkt256> s_axis_bram_read_cmd;
	hls::stream<pkt512> s_axis_bram_write_data;
	hls::stream<pkt512> m_axis_bram_read_data;
	hls::stream<pkt64> m_axis_update;

	//TODO: Set up inital state

	std::cout << "Starting kernel..." << std::endl;

	//Run kernel
	krnl(
		s_axis_tx_meta,
		s_axis_tx_data,
		m_axis_tx_status,
		s_axis_bram_write_cmd,
		s_axis_bram_read_cmd,
		s_axis_bram_write_data,
		m_axis_bram_read_data,
		m_axis_update,
		myBoardNum,
		RDMA_TYPE,
		exec,
		network_ptr
	);

	//TODO: Check results
}
