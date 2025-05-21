#ifndef TEST__ONE_INTERNAL_HPP
#define TEST__ONE_INTERNAL_HPP


#include "../../types.hpp"
extern "C" {
#include "../../core/node.h"
};
#include "../test-helpers.hpp"
#include "../../operations.hpp"
#include <hls_stream.h>


bool one_internal(KERNEL_ARG_DECS);


#endif
