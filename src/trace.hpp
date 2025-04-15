#ifndef TRACE_HPP
#define TRACE_HPP


#include "types.hpp"
extern "C" {
#include "core/node.h"
};
#include "memgr.hpp"
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
		//! @brief Step the state machine once
		//! @return true if reached a leaf, false otherwise
		bool sm_step(
			//! [out] Stream of addresses to read from main memory
			hls::stream<mread_req_t>& readReqFifo,
			//! [in]  Stream of read results from main memory
			hls::stream<mread_resp_t>& readRespFifo
		);
		//! @brief Set a new key and reset internal state
		void reset(bptr_t root, bkey_t key);
		//! @brief Get the result of the most recent operation.
		bstatusval_t get_result() const;
		//! @brief Get a copy of the last node read from memory.
		AddrNode get_node() const;
		//! @brief Move the history buffer back by one
		//! @return `false` if there are insufficient entries in the history buffer, otherwise `true`
		AddrNode get_parent(Node *memory) const;
		//! @brief Move the history buffer back by two without loading the intermediate node
		//! @return `false` if there are insufficient entries in the history buffer, otherwise `true`
		bool double_step_back(Node *memory);
};


#endif
