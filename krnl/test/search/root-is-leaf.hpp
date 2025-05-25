#ifndef TEST__ROOT_IS_LEAF_HPP
#define TEST__ROOT_IS_LEAF_HPP


extern "C" {
#include "../../core/node.h"
};
#include "../test-helpers.hpp"
#include "../../operations.hpp"
#include <hls_stream.h>


bool root_is_leaf(KERNEL_ARG_DECS);


#endif
