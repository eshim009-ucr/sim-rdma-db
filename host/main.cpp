#include "device.hpp"
#include "alloc.hpp"
#include "cpp-ext.hpp"
#include "validate.hpp"
#include <vector>
#include <iostream>
extern "C" {
#include "node.h"
#include "operations.h"
#include "io.h"
};


static void setup_ocl(
	const char* binaryFile,
	cl::Context& context,
	cl::Kernel& krnl1,
	cl::CommandQueue& q) {
	cl_int err;

	auto devices = get_xil_devices();
	auto fileBuf = read_binary_file(binaryFile);
	cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
	bool valid_device = false;

	for (size_t i = 0; i < devices.size(); i++) {
		auto device = devices[i];
		OCL_CHECK(err,
			context = cl::Context(device, nullptr, nullptr, nullptr, &err));
		OCL_CHECK(err, q = cl::CommandQueue(context, device, 0, &err));
		
		std::cout << "Trying to program device[" << i << "]: "
			<< device.getInfo<CL_DEVICE_NAME>() << std::endl;
		cl::Program program(context, {device}, bins, nullptr, &err);
		
		if (err != CL_SUCCESS) {
			std::cerr << "Failed to program device[" << i << "]"
				<< " with xclbin file '" << binaryFile << "'!\n";
		} else {
			std::cout << "Device[" << i << "]: program successful!\n";
			std::cout << "Setting CU(s) up..." << std::endl;
			OCL_CHECK(err, krnl1 = cl::Kernel(program, "krnl", &err));
			valid_device = true;
			break;
		}
	}
	if (!valid_device) {
		std::cerr << "Failed to program any device found, exit!\n";
		exit(EXIT_FAILURE);
	}
}


static void setup_data(
	std::vector<Request, aligned_allocator<Request> >& requests,
	std::vector<Response, aligned_allocator<Response> >& responses,
	std::vector<Response, aligned_allocator<Response> >& responses_expected,
	std::vector<Node, aligned_allocator<Node> >& memory
) {
	Request tmp_req = {.opcode = INSERT};
	Response tmp_resp = {.opcode = INSERT, .insert = SUCCESS};

	for (int i = 1; i <= 22; i++) {
		tmp_req.insert.key = i;
		tmp_req.insert.value.data = -i;
		requests.push_back(tmp_req);
		responses_expected.push_back(tmp_resp);
	}
	responses.resize(responses_expected.size());

	memory.resize(MEM_SIZE);
	memset(memory.data(), INVALID, MEM_SIZE*sizeof(Node));
	for (int i = 0; i < MEM_SIZE; ++i) {
		memory.at(i).lock = 0;
	}
}


static void run_kernel(
	cl::Context& context,
	cl::Kernel& krnl1,
	cl::CommandQueue& q,
	std::vector<Request, aligned_allocator<Request> >& requests,
	std::vector<Response, aligned_allocator<Response> >& responses,
	std::vector<Node, aligned_allocator<Node> >& memory
) {
	constexpr int FROM_HOST_FLAGS = 0;
	cl_int err;
	clock_t htod, dtoh, comp;

	// INPUT BUFFERS
	OCL_CHECK(err, cl::Buffer buffer_requests(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
		sizeof(Request)*requests.size(),
		requests.data(),
		&err
	));
	// OUTPUT BUFFERS
	OCL_CHECK(err, cl::Buffer buffer_responses(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
		sizeof(Response)*responses.size(),
		responses.data(),
		&err
	));
	OCL_CHECK(err, cl::Buffer buffer_memory(
		context,
		CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
		sizeof(Node)*memory.size(),
		memory.data(),
		&err
	));
	// SETTING INPUT PARAMETERS
	OCL_CHECK(err, err = krnl1.setArg(0, buffer_memory));
	OCL_CHECK(err, err = krnl1.setArg(1, buffer_requests));
	OCL_CHECK(err, err = krnl1.setArg(2, buffer_responses));
	OCL_CHECK(err, err = krnl1.setArg(3, (uint32_t) (6*requests.size())));
	OCL_CHECK(err, err = krnl1.setArg(4, (uint32_t) (1.25*requests.size())));
	OCL_CHECK(err, err = krnl1.setArg(5, true));

	// HOST -> DEVICE DATA TRANSFER
	std::cout << "HOST -> DEVICE" << std::endl;
	htod = clock();
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
		{buffer_requests}, FROM_HOST_FLAGS
	));
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
		{buffer_memory}, FROM_HOST_FLAGS
	));
	q.finish();
	htod = clock() - htod;
	// STARTING KERNEL(S)
	std::cout << "STARTING KERNEL(S)" << std::endl;
	comp = clock();
	OCL_CHECK(err, err = q.enqueueTask(krnl1));
	q.finish();
	comp = clock() - comp;
	std::cout << "KERNEL(S) FINISHED" << std::endl;
	// DEVICE -> HOST DATA TRANSFER
	std::cout << "HOST <- DEVICE" << std::endl;
	dtoh = clock();
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
		{buffer_memory}, CL_MIGRATE_MEM_OBJECT_HOST
	));
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
		{buffer_responses}, CL_MIGRATE_MEM_OBJECT_HOST
	));
	q.finish();
	dtoh = clock() - dtoh;

	printf("Host -> Device : %lf ms\n", 1000.0 * htod/CLOCKS_PER_SEC);
	printf("Device -> Host : %lf ms\n", 1000.0 * dtoh/CLOCKS_PER_SEC);
	printf("Computation : %lf ms\n",  1000.0 * comp/CLOCKS_PER_SEC);
}


static int verify(
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


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

	cl::Kernel krnl;
	cl::Context context;
	cl::CommandQueue q;
	setup_ocl(argv[1], context, krnl, q);
	
	std::vector<Request, aligned_allocator<Request> > requests;
	std::vector<Response, aligned_allocator<Response> >
		responses, responses_expected;
	std::vector<Node, aligned_allocator<Node> > memory(MEM_SIZE);
	setup_data(requests, responses, responses_expected, memory);
	run_kernel(context, krnl, q, requests, responses, memory);
	
	return verify(responses, responses_expected, memory);
}
