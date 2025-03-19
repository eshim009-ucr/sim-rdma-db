#ifndef TEST__SPLIT_ROOT_HPP
#define TEST__SPLIT_ROOT_HPP


#include "../../node.hpp"
#include <hls_stream.h>


bool split_root(
	hls::stream<pkt256>& m_axis_tx_meta,
	hls::stream<pkt64>& m_axis_tx_data,
	hls::stream<pkt64>& s_axis_tx_status,
	hls::stream<pkt256>& m_axis_bram_write_cmd,
	hls::stream<pkt256>& m_axis_bram_read_cmd,
	hls::stream<pkt512>& m_axis_bram_write_data,
	hls::stream<pkt512>& s_axis_bram_read_data,
	hls::stream<pkt64>& s_axis_update,
	int myBoardNum,
	int RDMA_TYPE,
	int exec,
	uint8_t *hbm
);


#endif
