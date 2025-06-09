#include "test.hpp"
extern "C" {
#include "io.h"
};
#include "validate.hpp"
#include "cpp-ext.hpp"


void setup_insert_then_search(
	std::vector<Request, aligned_allocator<Request> >& insert_reqs,
	std::vector<Response, aligned_allocator<Response> >& insert_resps_expected,
	std::vector<Request, aligned_allocator<Request> >& search_reqs,
	std::vector<Response, aligned_allocator<Response> >& search_resps_expected,
	std::vector<Node, aligned_allocator<Node> >& memory,
	KeyPattern key_pattern,
	uint_fast64_t entry_count
) {
	Request tmp_insert_req = {.opcode = INSERT};
	Response tmp_insert_resp = {.opcode = INSERT, .insert = SUCCESS};
	Request tmp_search_req = {.opcode = SEARCH};
	Response tmp_search_resp = {.opcode = SEARCH, .search={.status = SUCCESS}};

	for (uint_fast64_t i = 1; i <= entry_count; i++) {
		// Insert
		switch (key_pattern) {
			case SEQUENTIAL:
				tmp_insert_req.insert.key = i;
				tmp_insert_req.insert.value.data = -i;
				break;
			case RANDOM:
				tmp_insert_req.insert.value.data = rand();
				tmp_insert_req.insert.value.data = rand();
				break;
		}
		insert_reqs.push_back(tmp_insert_req);
		insert_resps_expected.push_back(tmp_insert_resp);
		// Search
		tmp_search_req.search = i;
		tmp_search_resp.search.value.data = -i;
		search_reqs.push_back(tmp_search_req);
		search_resps_expected.push_back(tmp_search_resp);
	}

	memory.resize(MEM_SIZE);
	memset(memory.data(), INVALID, MEM_SIZE*sizeof(Node));
	for (uint_fast64_t i = 0; i < MEM_SIZE; ++i) {
		memory.at(i).lock = LOCK_INIT;
	}
}


int verify(
	std::vector<Response, aligned_allocator<Response> >& responses,
	std::vector<Response, aligned_allocator<Response> >& responses_expected,
	std::vector<Node, aligned_allocator<Node> >& memory
) {
	bool match = true;

	printf("Verifying Responses...");
	if (responses.size() != responses_expected.size()) {
		match = false;
	} else {
		for (size_t i = 0; i < responses_expected.size(); i++) {
			if (responses_expected[i] != responses[i]) {
				std::cout << "\nresponses[" << i <<"]:"
					<< "\n\texpected " << to_str(responses_expected[i])
					<< "\n\tgot " << to_str(responses[i]) << std::endl;
				match = false;
			}
		}
	}
	printf("Done!\n");
	printf("Verifying memory contents...\n");
	check_inserted_leaves(memory.data());
	printf("Done!\n");

	dump_node_list(stdout, memory.data());

	std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
	return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
