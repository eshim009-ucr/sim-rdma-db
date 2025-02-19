#ifndef TRACE_HPP
#define TRACE_HPP


#include "types.hpp"
#include "node.hpp"
#include <hls_stream.h>


//! @brief Wrapper for state machine to hold a path throughout the tree
//!
//! Can't do it like normal because this code is reused by multiple other state
//! machines that must maintain their own states
struct Tracer {
	private:
		//! Key to search for
		bkey_t key;
		//! Internal temp to hold the result of the last operation
		bstatusval_t result;
		//! State machine state
		enum {
			//! Reset all values to prepare for a new search
			RESET,
			//! Not doing anything
			IDLE,
			//! Queue an address to be read from memory
			READ_NODE,
			//! Check the result of the last memory read
			CHECK_NODE,
			//! Execution complete, awaiting reset
			DONE
		} state = DONE;
		//! Buffer to hold history of traversed nodes
		bptr_t history[MAX_LEVELS*2];
		//! Pointer to current spot in history buffer
		uint8_t i;
		//! Current node in traversal
		AddrNode node;
	public:
		//! Addresses to read
		hls::stream<bptr_t> &addrFifo;
		//! Results of address reads
		hls::stream<Node> &nodeFifo;
		//! Outputs
		hls::stream<bstatusval_t> output;
		//! Initialize the root for the history buffer
		Tracer(
			bptr_t root,
			hls::stream<bptr_t> &addrFifo,
			hls::stream<Node> &nodeFifo
		);
		//! @brief Step the state machine once
		//! @return true if reached a leaf, false otherwise
		bool sm_step();
		//! @brief Set a new key and reset internal state
		void reset(bkey_t key);
};


#endif
