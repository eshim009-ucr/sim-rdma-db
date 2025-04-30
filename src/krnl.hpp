#pragma once


#include "operations.hpp"
#include "types.hpp"
extern "C" {
#include "core/node.h"
};
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
	int myBoardNum,
	int RDMA_TYPE,
	int exec,
	//! [inout] Pointer to on-chip high-bandwidth memory
	uint64_t *hbm,
	uint64_t *req_buffer,
	uint64_t *resp_buffer,
	//! [inout] Address within HBM that holds the root of the tree
	//!
	//! Can be modified by operation kernels
	bptr_t root
);
