#include "test-helpers.hpp"
#include <cstring>


void reset_mem(uint8_t *hbm) {
	memset(hbm, INVALID, MEM_SIZE*sizeof(Node));
	for (uint_fast8_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&((Node*) hbm)[i].lock);
	}
}
