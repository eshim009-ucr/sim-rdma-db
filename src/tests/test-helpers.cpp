#include "test-helpers.hpp"
#include <cstring>
#include <iostream>


void reset_mem(uint8_t *hbm) {
	memset(hbm, INVALID, MEM_SIZE*sizeof(Node));
	for (uint_fast8_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&((Node*) hbm)[i].lock);
	}
}


void hbm_dump(uint8_t* hbm, uint8_t length) {
	const uint8_t nodes_per_line = 80 / (sizeof(Node)*2);
	std::cout << " -  HBM Dump  - " << std::hex;
	for (uint_fast32_t n = 0; n < length; ++n) {
		if (n % nodes_per_line == 0) {
			std::cout << std::endl;
		}
		for (uint_fast32_t b = 0; b < sizeof(Node); ++b) {
			if (hbm[n*sizeof(Node) + b] < 0x10) {
				std::cout << '0';
			}
			std::cout << (int) hbm[n*sizeof(Node) + b];
		}
		std::cout << "  " << std::flush;
	}
	std::cout << std::dec << std::endl;
}
