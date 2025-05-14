#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP


#include "search.hpp"
#include "insert.hpp"
#ifndef __SYNTHESIS__
#include <sstream>
#endif

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
} __attribute__((packed));

struct Response {
	Opcode opcode;
	union {
		search_out_t search;
		insert_out_t insert;
	};
	#ifndef __SYNTHESIS__
	bool operator==(const Response& rhs) const {
		if (opcode != rhs.opcode) return false;
		switch (opcode) {
			case NOP: return true;
			case SEARCH: return
				search.status == rhs.search.status &&
				search.value.data == rhs.search.value.data;
			case INSERT: return (uint_fast8_t) insert == (uint_fast8_t) rhs.insert;
			default: return memcmp(
					this + sizeof(Opcode),
					&rhs + sizeof(Opcode),
					sizeof(Response)-sizeof(Opcode)
				) == 0;
		}
	}
	bool operator!=(const Response& rhs) const {
		return (*this == rhs) == false;
	}
	operator std::string() const {
		std::stringstream ss;
		switch (opcode) {
			case NOP:
				ss << "NOP Response";
				break;
			case SEARCH:
				ss << "Search Response ";
				if (search.status >= 0 && search.status <= 6) {
					ss << ERROR_CODE_NAMES[search.status];
				} else {
					ss << "UNKNOWN";
				}
				ss << '(' << (int) search.status << "), " << search.value.data;
				break;
			case INSERT:
				ss << "Insert Response ";
				if (search.status >= 0 && search.status <= 6) {
					ss << ERROR_CODE_NAMES[search.status];
				} else {
					ss << "UNKNOWN";
				}
				ss << '(' << (int) insert << ')';
				break;
			default:
				ss << "Response Opcode " << (int) opcode;
				break;
		}
		return ss.str();
	}
	#endif
} __attribute__((packed));


Request encode_search_req(search_in_t in);
Request encode_insert_req(insert_in_t in);
Response encode_search_resp(search_out_t out);
Response encode_insert_resp(insert_out_t out);

#ifdef HLS
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


#endif
