#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP


#include "../node.hpp"


#define VERBOSE
#define INPUT_SEARCH(x) requests.write(encode_search_req(x)); input_log.write(x);
#define INPUT_INSERT(key_, value_) \
	last_in.key = key_; last_in.value.data = value_; \
	requests.write(encode_insert_req(last_in)); input_log.write(last_in);
#define SET_IKV(addr, i, key_, value_) \
	((Node*) hbm)[addr].keys[i] = key_; ((Node*) hbm)[addr].values[i].data = value_;
#define RUN_KERNEL \
	krnl( \
		myBoardNum, RDMA_TYPE, exec, \
		hbm, root, \
		requests, responses \
	);


void reset_mem(uint8_t *hbm);

//!@brief Print a hex dump of the first few nodes of HBM
void hbm_dump(uint8_t* hbm, uint8_t length);


#endif
