#pragma once


#include "operations.hpp"
#include "node.hpp"
#include <hls_stream.h>
#include <iostream>
#include <stdlib.h>
#include <ap_int.h>
#include <ap_fixed.h>


template <int K>
struct htLookupReq
{
	ap_uint<K>  key;
	ap_uint<1>  source;
	htLookupReq<K>() {}
	htLookupReq<K>(ap_uint<K> key, ap_uint<1> source)
		:key(key), source(source) {}
};

template <int K, int V>
struct htLookupResp
{
	ap_uint<K>  key;
	ap_uint<V>  value;
	bool        hit;
	ap_uint<1>  source;
};

typedef enum {KV_INSERT, KV_DELETE} kvOperation;

template <int K, int V>
struct htUpdateReq
{
	kvOperation op;
	ap_uint<K>  key;
	ap_uint<V>  value;
	ap_uint<1>  source;
	htUpdateReq<K,V>() {}
	htUpdateReq<K,V>(kvOperation op, ap_uint<K> key, ap_uint<V> value, ap_uint<1> source)
		:op(op), key(key), value(value), source(source) {}
};

template <int K, int V>
struct htUpdateResp
{
	kvOperation op;
	ap_uint<K>  key;
	ap_uint<V>  value;
	bool        success;
	ap_uint<1>  source;
};


void krnl(
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
	//! [inout] Pointer to on-chip high-bandwidth memory
	uint8_t *hbm,
	//! [inout] Address within HBM that holds the root of the tree
	//!
	//! Can be modified by operation kernels
	bptr_t root,
	hls::stream<Request>& requests,
	hls::stream<Response>& responses
);
