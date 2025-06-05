#include "host.hpp"
#include "test.hpp"
#include "run-tree.hpp"
#include "thread-runner.hpp"
#include <iostream>


Node memory[MEM_SIZE];


int main(int argc, char** argv) {
	if (argc < 4 {
		std::cout << "Usage: " << argv[0]
			<< "exe <XCLBIN File> <Request.BIN Files>" << std::endl;
		return EXIT_FAILURE;
	}

	return run_from_file(argc, argv);
}
