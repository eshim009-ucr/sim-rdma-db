#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP


#include "../node.hpp"


#define VERBOSE
#define INPUT(op, x) opstream.op.input.write(x); input_log.write(x);
#define INPUT_SEARCH(x) opstream.search.input.write(x); input_log.write(x);
#define INPUT_INSERT(key_, value_) \
	last_in.key = key_; last_in.value.data = value_; \
	opstream.insert.input.write(last_in); input_log.write(last_in);
#define SET_IKV(addr, i, key_, value_) \
	hbm[addr].keys[i] = key_; hbm[addr].values[i].data = value_;


void reset_mem(Node *hbm);


#endif
