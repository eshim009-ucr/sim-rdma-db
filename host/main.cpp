#include "host.hpp"
#include "test.hpp"
#include "run-tree.hpp"
#include <iostream>
#include <fstream>


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

	std::ofstream fout("data.csv");
	KeyPattern key_pattern = SEQUENTIAL;

	TreeInput input_insert, input_search;
	TreeOutput output_insert, output_search;
	std::vector<Response, aligned_allocator<Response> >
		insert_expected, search_expected;

	fout << "Operation,Key Pattern,Entry Count,Duration,Throughput,Verified" << std::endl;
	for (uint_fast64_t i = 0; i < 2; ++i) {
		for (uint_fast64_t entry_count = 1000; entry_count <= 1000000; entry_count *= 10) {
			insert_expected.clear();
			search_expected.clear();
			setup_insert_then_search(
				input_insert.requests, insert_expected,
				input_search.requests, search_expected,
				input_insert.memory,
				RANDOM, entry_count
			);
			output_insert = run_fpga_tree(input_insert, argv[1]);
			fout << "INSERT,"
				<< ((key_pattern==RANDOM) ? "RANDOM" : "SEQUENTIAL") << ','
				<< entry_count << ','
				<< output_insert.duration_us << ','
				<< entry_count/(output_insert.duration_us/1000000) << ','
				<< (verify(
					output_insert.responses,
					insert_expected,
					output_insert.memory
				) ? "PASS" : "FAIL") << std::endl;
			// input_search.memory = output_insert.memory;
			input_search.memory.assign(output_insert.memory.begin(), output_insert.memory.end());
			output_search = run_fpga_tree(input_search, argv[1]);
			fout << "SEARCH,"
				<< ((key_pattern==RANDOM) ? "RANDOM" : "SEQUENTIAL") << ','
				<< entry_count << ','
				<< output_search.duration_us << ','
				<< entry_count/(output_search.duration_us/1000000) << ','
				<< (verify(
					output_search.responses,
					search_expected,
					output_search.memory
				) ? "PASS" : "FAIL") << std::endl;
		}
		key_pattern = RANDOM;
	}
	fout.close();

	return 0;
}
