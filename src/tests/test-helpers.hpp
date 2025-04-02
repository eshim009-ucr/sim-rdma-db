#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP


#include "../node.hpp"


#define VERBOSE
#define INPUT_SEARCH(x) \
	*((Request*) &hbm[offset]) = encode_search_req(x); \
	offset += sizeof(Request); \
	input_log.write(x);
#define INPUT_INSERT(key_, value_) \
	last_in.key = key_; last_in.value.data = value_; \
	*((Request*) &hbm[offset]) = encode_insert_req(last_in); \
	offset += sizeof(Request); \
	input_log.write(last_in);
#define SET_IKV(addr, i, key_, value_) \
	((Node*) hbm)[addr].keys[i] = key_; ((Node*) hbm)[addr].values[i].data = value_;
#define RUN_KERNEL \
	krnl( \
		myBoardNum, RDMA_TYPE, exec, \
		hbm, root \
	);


void reset_mem(uint8_t *hbm);

//!@brief Print a hex dump of a section of HBM grouped by object
void hbm_dump(
	//! Memory buffer to read from
	uint8_t* hbm,
	//! Offset at which to start the dump
	uint_fast64_t offset,
	//! Size in bytes of object to group by
	size_t size,
	//! Number of objects to print
	uint_fast64_t length
);


#endif
