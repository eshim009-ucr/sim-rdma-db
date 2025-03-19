#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP


#include "search.hpp"
#include "insert.hpp"


//! @brief Pairs of all I/O Fifos for all operations
struct IoPairs {
	SearchIO search;
	InsertIO insert;
};

enum Opcode {
	SEARCH = 1,
	INSERT = 2
};

typedef uint64_t req_bits_t;
typedef uint64_t resp_bits_t;
union Request {
	struct RequestFields {
		Opcode opcode;
		union {
			search_in_t search;
			insert_in_t insert;
		};
	} fields;
	req_bits_t bits;
};
union Response {
	struct {
		Opcode opcode;
		union {
			search_out_t search;
			insert_out_t insert;
		};
	} fields;
	resp_bits_t bits;
};


Request encode_search_req(search_in_t in);
Request encode_insert_req(insert_in_t in);
Response encode_search_resp(search_out_t out);
Response encode_insert_resp(insert_out_t out);

//! @brief State machine to decode and redirect incoming instructions
void sm_decode(
	//! [in]  Incoming instructions
	hls::stream<req_bits_t>& requests,
	//! [out] Bundle of FIFOs for decoded instructions
	IoPairs& opstream
);

//! @brief State machine to encode and queue responses for send
void sm_encode(
	//! [out]  Incoming instructions
	hls::stream<resp_bits_t>& responses,
	//! [in] Bundle of FIFOs for decoded instructions
	IoPairs& opstream
);


#endif
