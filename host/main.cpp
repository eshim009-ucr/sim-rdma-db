#include "host.hpp"
#include "test.hpp"
#include "run-tree.hpp"
#include "thread-runner.hpp"
#include <iostream>


Node memory[MEM_SIZE];


int main(int argc, char** argv) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

	return run_from_file(argc, argv);;
}
