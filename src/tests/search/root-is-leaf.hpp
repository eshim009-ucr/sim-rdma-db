#ifndef TEST__ROOT_IS_LEAF_HPP
#define TEST__ROOT_IS_LEAF_HPP


#include "../../node.hpp"
#include <hls_stream.h>


bool root_is_leaf(
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
