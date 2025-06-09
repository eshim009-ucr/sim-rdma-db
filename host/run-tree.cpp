#include "run-tree.hpp"
extern "C" {
#include "../krnl/core/memory.h"
#include "../krnl/core/node.h"
};


TreeInput::TreeInput() {
	uint_fast64_t i;
	const char* UNITS[] = {"B", "kB", "MB", "GB", "TB"};
	const uint64_t BYTES_TOTAL = sizeof(Node) * MEM_SIZE;
	uint64_t short_size = BYTES_TOTAL;

	for (i = 0; i < 5; ++i) {
		if (short_size > 1000) {
			short_size /= 1000;
		} else {
			break;
		}
	}

	std::cout << "Allocating "
		<< short_size << ' ' << UNITS[i] << "..." << std::flush;

	memory.resize(MEM_SIZE);
	std::cout << "Done!\nResetting memory..." << std::flush;
	mem_reset_all(memory.data());
	std::cout << "Done!" << std::endl;
}

TreeOutput::TreeOutput() {
	memory.resize(MEM_SIZE);
}
