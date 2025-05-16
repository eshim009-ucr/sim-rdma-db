#ifndef TYPES_HPP
#define TYPES_HPP


extern "C" {
#include "../core/types.h"
};
#ifdef HLS
#include <ap_axi_sdata.h>


typedef ap_axiu<512, 0, 0, 0> pkt512;
typedef ap_axiu<256, 0, 0, 0> pkt256;
typedef ap_axiu<128, 0, 0, 0> pkt128;
typedef ap_axiu<64, 0, 0, 0> pkt64;
typedef ap_axiu<32, 0, 0, 0> pkt32;
typedef ap_axiu<16, 0, 0, 0> pkt16;
typedef ap_axiu<8, 0, 0, 0> pkt8;
#endif

//! @brief Key/value pair
struct KvPair {
	bkey_t key;
	bval_t value;
};

//! @brief Contains an address and whether or not to change the lock status in
//! main memory before returning a result
struct RwOp {
	//! Address to read/write from/to
	bptr_t addr;
	//! For reads, will grab the lock before returning.
	//! For write, will release the lock before returning.
	bool lock;
};

#endif
