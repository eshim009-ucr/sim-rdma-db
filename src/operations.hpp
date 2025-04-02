#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP


#include "search.hpp"
#include "insert.hpp"
#include <sstream>

enum Opcode {
	NOP = 0,
	SEARCH = 1,
	INSERT = 2
};

struct Request {
	Opcode opcode;
	union {
		search_in_t search;
		insert_in_t insert;
	};
	operator std::string() const {
		std::stringstream ss;
		switch (opcode) {
			case SEARCH:
				ss << "Search Request " << search;
				break;
			case INSERT:
				ss << "Insert Request k=" << insert.key << ", v=" << insert.value.data;
				break;
		}
		return ss.str();
	}
} __attribute__((packed));

struct Response {
	Opcode opcode;
	union {
		search_out_t search;
		insert_out_t insert;
	};
	operator std::string() const {
		std::stringstream ss;
		switch (opcode) {
			case SEARCH:
				ss << "Search Response " << ERROR_CODE_NAMES[-search.status] << ", " << search.value.data;
				break;
			case INSERT:
				ss << "Insert Response " << ERROR_CODE_NAMES[-insert];
				break;
		}
		return ss.str();
	}
} __attribute__((packed));


Request encode_search_req(search_in_t in);
Request encode_insert_req(insert_in_t in);
Response encode_search_resp(search_out_t out);
Response encode_insert_resp(insert_out_t out);

//! @brief State machine to decode and redirect incoming instructions
void sm_decode(
	//! [in]  Incoming instructions
	hls::stream<Request>& requests,
	//! [out] Decoded search instructions
	hls::stream<search_in_t>& searchInput,
	//! [out] Decoded insert instructions
	hls::stream<insert_in_t>& insertInput
);

//! @brief State machine to encode and queue responses for send
void sm_encode(
	//! [out]  Incoming instructions
	hls::stream<Response>& responses,
	//! [in] Search results
	hls::stream<search_out_t>& searchOutput,
	//! [in] Insert results
	hls::stream<insert_out_t>& insertOutput
);


#endif
