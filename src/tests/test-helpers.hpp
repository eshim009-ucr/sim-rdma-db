#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP


#define VERBOSE
#define INPUT(x) input.write(x); input_log.write(x);
#define SET_IKV(addr, i, key_, value_) \
	hbm[addr].keys[i] = key_; hbm[addr].values[i].data = value_;


#endif
