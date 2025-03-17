#include "test-helpers.hpp"
#include <cstring>


void reset_mem(Node *hbm) {
	memset(hbm, INVALID, MEM_SIZE*sizeof(Node));
	for (uint_fast8_t i = 0; i < MEM_SIZE; i++) {
		init_lock(&hbm[i].lock);
	}
}
