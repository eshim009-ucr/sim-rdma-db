#include "host.h"
#include "src/operations.hpp"
#include <vector>


bool check_inserted_leaves(Node const *memory) {
	bool match = true;
	uint_fast8_t next_val = 1;
	AddrNode node = {.addr = 0};
	int i = 0;

	while (node.addr != INVALID) {
		node.node = memory[node.addr];
		for (li_t j = 0; j < TREE_ORDER; ++j) {
			if (node.node.keys[j] == INVALID) {
				if (i == 0 && j == 0) {
					std::cout << "Fail, memory is empty" << std::endl;
					match = false;
				}
				break;
			} else {
				if (node.node.keys[j] != next_val) {
					std::cout << "mem[" << node.addr << "].keys[" << j <<"]:"
						<< "\n\texpected " << node.node.keys[j]
						<< "\n\tgot " << next_val << std::endl;
					match = false;
				}
				if (node.node.values[j].data == -next_val) {
					std::cout << "mem[" << node.addr << "].values[" << j <<"]:"
						<< "\n\texpected " << node.node.values[j].data
						<< "\n\tgot " << -next_val << std::endl;
					match = false;
				}
				next_val++;
			}
		}
		i++;
		node.addr = node.node.next;
	}
	if (match) {
		std::cout << "Verified " << i << " leaves" << std::endl;
	}
	return match;
}


int main(int argc, char** argv) {

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}

	clock_t htod, dtoh, comp;

	/*====================================================CL===============================================================*/

	std::string binaryFile = argv[1];
	cl_int err;
	cl::Context context;
	cl::Kernel krnl1;
	cl::CommandQueue q;

	auto devices = get_xil_devices();
	auto fileBuf = read_binary_file(binaryFile);
	cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
	bool valid_device = false;
	for (size_t i = 0; i < devices.size(); i++) {
		auto device = devices[i];
		OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
		OCL_CHECK(err, q = cl::CommandQueue(context, device, 0, &err));
		std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
		cl::Program program(context, {device}, bins, nullptr, &err);
		if (err != CL_SUCCESS) {
			std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
		} else {
			std::cout << "Device[" << i << "]: program successful!\n";
			std::cout << "Setting CU(s) up..." << std::endl;
			OCL_CHECK(err, krnl1 = cl::Kernel(program, "krnl", &err));
			valid_device = true;
			break; // we break because we found a valid device
		}
	}
	if (!valid_device) {
		std::cout << "Failed to program any device found, exit!\n";
		exit(EXIT_FAILURE);
	}

	/*====================================================INIT INPUT/OUTPUT VECTORS===============================================================*/

	std::vector<Request, aligned_allocator<Request> > requests;
	std::vector<Response, aligned_allocator<Response> >
		responses, responses_expected;
	std::vector<Node, aligned_allocator<Node> > memory(MEM_SIZE);
	Request tmp_req = {.opcode = INSERT};
	Response tmp_resp = {.opcode = INSERT, .insert = SUCCESS};

	for (int i = 1; i <= 22; i++) {
		tmp_req.insert.key = i;
		tmp_req.insert.value.data = -i;
		requests.push_back(tmp_req);
		responses_expected.push_back(tmp_resp);
	}
	responses.resize(responses_expected.size());

	memset(memory.data(), INVALID, MEM_SIZE*sizeof(Node));
	for (int i = 0; i < MEM_SIZE; ++i) {
		memory.at(i).lock = 0;
	}

	/*====================================================Setting up kernel I/O===============================================================*/

	/* INPUT BUFFERS */
	OCL_CHECK(err, cl::Buffer buffer_requests(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(Request)*requests.size(), requests.data(), &err));

	/* OUTPUT BUFFERS */
	OCL_CHECK(err, cl::Buffer buffer_responses(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(Response)*responses.size(), responses.data(), &err));
	OCL_CHECK(err, cl::Buffer buffer_memory(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(Node)*memory.size(), memory.data(), &err));

	/* SETTING INPUT PARAMETERS */
	OCL_CHECK(err, err = krnl1.setArg(0, 0));
	OCL_CHECK(err, err = krnl1.setArg(1, 0));
	OCL_CHECK(err, err = krnl1.setArg(2, 0x1000));
	OCL_CHECK(err, err = krnl1.setArg(3, buffer_memory));
	OCL_CHECK(err, err = krnl1.setArg(4, buffer_requests));
	OCL_CHECK(err, err = krnl1.setArg(5, buffer_responses));
	OCL_CHECK(err, err = krnl1.setArg(6, 0));

	/*====================================================KERNEL===============================================================*/
	/* HOST -> DEVICE DATA TRANSFER*/
	std::cout << "HOST -> DEVICE" << std::endl;
	htod = clock();
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_requests}, 0 /* 0 means from host*/));
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_memory}, 0 /* 0 means from host*/));
	q.finish();
	htod = clock() - htod;

	/*STARTING KERNEL(S)*/
	std::cout << "STARTING KERNEL(S)" << std::endl;
	comp = clock();
	OCL_CHECK(err, err = q.enqueueTask(krnl1));
	q.finish();
	comp = clock() - comp;
	std::cout << "KERNEL(S) FINISHED" << std::endl;

	/*DEVICE -> HOST DATA TRANSFER*/
	std::cout << "HOST <- DEVICE" << std::endl;
	dtoh = clock();
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_memory}, CL_MIGRATE_MEM_OBJECT_HOST));
	OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_responses}, CL_MIGRATE_MEM_OBJECT_HOST));
	q.finish();
	dtoh = clock() - dtoh;

	/*====================================================VERIFICATION & TIMING===============================================================*/

	printf("Host -> Device : %lf ms\n", 1000.0 * htod/CLOCKS_PER_SEC);
	printf("Device -> Host : %lf ms\n", 1000.0 * dtoh/CLOCKS_PER_SEC);
	printf("Computation : %lf ms\n",  1000.0 * comp/CLOCKS_PER_SEC);

	bool match = true;
	printf("Verifying Responses...\n");
	if (responses.size() != responses_expected.size()) {
		match = false;
	} else {
		for (size_t i = 0; i < responses_expected.size(); i++) {
			if (responses_expected[i] != responses[i]) {
				std::cout << "responses[" << i <<"]:"
					<< "\n\texpected " << static_cast<std::string>(responses_expected[i])
					<< "\n\tgot " << static_cast<std::string>(responses[i]) << std::endl;
				match = false;
			}
		}
	}
	printf("Verifying memory contents...\n");
	check_inserted_leaves(memory.data());
	printf("Done!\n");

	std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

	return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
