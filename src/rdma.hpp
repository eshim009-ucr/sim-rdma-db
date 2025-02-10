#pragma once

#include <hls_stream.h>
#include <ap_int.h>
#include "types.hpp"


void rdma_read(
	int s_axi_lqpn,
	ap_uint<64> s_axi_laddr,
	ap_uint<64> s_axi_raddr,
	int s_axi_len,
	hls::stream<pkt256>& m_axis_tx_meta
);

void rdma_write(
	int s_axi_lqpn,
	ap_uint<64> s_axi_laddr,
	ap_uint<64> s_axi_raddr,
	int s_axi_len,
	ap_uint<64> write_value,
	hls::stream<pkt256>& m_axis_tx_meta,
	hls::stream<pkt64>& m_axis_tx_data
);

void rdma_write_through(
	int s_axi_lqpn,
	ap_uint<64> s_axi_laddr,
	ap_uint<64> s_axi_raddr,
	int s_axi_len,
	ap_uint<64> write_value,
	hls::stream<pkt256>& m_axis_tx_meta,
	hls::stream<pkt64>& m_axis_tx_data
);

void rdma_bram_read(
	int s_axi_lqpn,
	ap_uint<64> s_axi_laddr,
	ap_uint<64> s_axi_raddr,
	int s_axi_len,
	hls::stream<pkt256>& m_axis_tx_meta
);

void rdma_bram_write(
	int s_axi_lqpn,
	ap_uint<64> s_axi_laddr,
	ap_uint<64> s_axi_raddr,
	int s_axi_len,
	ap_uint<64>  write_value,
	hls::stream<pkt256>& m_axis_tx_meta,
	hls::stream<pkt64>& m_axis_tx_data
);
