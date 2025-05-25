#ifndef DEVICE_HPP
#define DEVICE_HPP


#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

// OCL_CHECK doesn't work if call has templatized function call
#define OCL_CHECK(error, call) \
	call; \
	if (error != CL_SUCCESS) { \
		printf("%s:%d Error calling " #call ", error code is: %d\n", \
			__FILE__, __LINE__, error); \
		exit(EXIT_FAILURE); \
	}

#include <vector>
#if __has_include("CL/opencl.hpp")
#include <CL/opencl.hpp>
#else
#include <CL/cl2.hpp>
#endif


std::vector<cl::Device> get_devices(const std::string& vendor_name);
std::vector<cl::Device> get_xil_devices();
std::vector<unsigned char> read_binary_file(const std::string& xclbin_file_name);


#endif
