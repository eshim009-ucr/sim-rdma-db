#ifndef HOST_HPP
#define HOST_HPP


#include "run-tree.hpp"


void run_fpga_tree(
	std::vector<Request, aligned_allocator<Request> >& requests,
	std::vector<Response, aligned_allocator<Response> >& responses,
	std::vector<Node, aligned_allocator<Node> >& memory,
	std::string const& binaryFile
);


#endif
