#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP


extern "C" {
#include "../core/node.h"
};
#include <cstddef>


#define VERBOSE
#define INPUT_SEARCH(x) \
	req_buffer[offset++] = encode_search_req(x); \
	input_log.write(x);
#define INPUT_INSERT(key_, value_) \
	last_in.key = key_; last_in.value.data = value_; \
	req_buffer[offset++] = encode_insert_req(last_in); \
	input_log.write(last_in);
#define SET_IKV(addr, i, key_, value_) \
	hbm[addr].keys[i] = key_; hbm[addr].values[i].data = value_;
#define KERNEL_ARG_DECS \
	uint8_t *hbm, uint8_t *req_buffer, uint8_t *resp_buffer, bptr_t root, \
	int loop_max, int op_max, bool reset
#define KERNEL_ARG_VARS \
	hbm, req_buffer, resp_buffer, root, loop_max, op_max, reset


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
