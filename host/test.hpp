#ifndef HOST_TEST_HPP
#define HOST_TEST_HPP



extern "C" {
#include "node.h"
#include "operations.h"
};
#include "alloc.hpp"
#include <vector>


enum KeyPattern {
	SEQUENTIAL,
	RANDOM
};


void setup_insert_then_search(
	std::vector<Request, aligned_allocator<Request> >& insert_reqs,
	std::vector<Response, aligned_allocator<Response> >& insert_resps_expected,
	std::vector<Request, aligned_allocator<Request> >& search_reqs,
	std::vector<Response, aligned_allocator<Response> >& search_resps_expected,
	std::vector<Node, aligned_allocator<Node> >& memory,
	KeyPattern key_pattern,
	uint_fast64_t entry_count
);

int verify(
	std::vector<Response, aligned_allocator<Response> >& responses,
	std::vector<Response, aligned_allocator<Response> >& responses_expected,
	std::vector<Node, aligned_allocator<Node> >& memory
);


#endif
