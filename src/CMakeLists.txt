cmake_minimum_required(VERSION 3.0)

set (PROJECT_NAME krnl)
project(${PROJECT_NAME})

# Include custom Find<Module>.cmake scripts to enable searching for Vivado HLS
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)



set(FPGA_PART xcu280-fsvh2892-2L-e CACHE STRING "FPGA part")
set(CLOCK_PERIOD 300.000000MHz CACHE STRING "Target clock period in nanoseconds")
set(DEFS "-DHLS" CACHE STRING "Compile-time options")


# Installation directory
if (DEFINED ENV{IPREPO_DIR})
	set(IPREPO_DIR $ENV{IPREPO_DIR})
elseif(NOT IPREPO_DIR)
	set(IPREPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/iprepo/)
endif()


include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/make.tcl.in make.tcl)


set(EXAMPLE_HLS_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sm_insert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/krnl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/core/node.c
	${CMAKE_CURRENT_SOURCE_DIR}/core/insert.c
	${CMAKE_CURRENT_SOURCE_DIR}/core/insert-helpers.c
	${CMAKE_CURRENT_SOURCE_DIR}/core/memory.c
	${CMAKE_CURRENT_SOURCE_DIR}/core/split.c
	${CMAKE_CURRENT_SOURCE_DIR}/operations.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ramstream.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rdma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sm_search.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/test-helpers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/insert/leaf-node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/insert/split-root.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/insert/until-it-breaks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/search/one-internal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tests/search/root-is-leaf.cpp)


#Setup HLS custom targets
set(HLS_TARGETS synthesis csim ip installip)

foreach (target ${HLS_TARGETS})
	if (NOT TARGET ${target})
		add_custom_target(${target})
	endif()

	add_custom_target(${target}.${PROJECT_NAME}
		COMMAND ${VITIS_HLS_BINARY} -f make.tcl -tclargs ${target}
		DEPENDS ${EXAMPLE_HLS_DEPENDS})
	add_dependencies(${target} ${target}.${PROJECT_NAME})
endforeach()

#target dependencies
add_dependencies(ip.${PROJECT_NAME} synthesis.${PROJECT_NAME})
add_dependencies(installip.${PROJECT_NAME} ip.${PROJECT_NAME})
