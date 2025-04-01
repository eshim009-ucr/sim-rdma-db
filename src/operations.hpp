#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP


#include "search.hpp"
#include "insert.hpp"
#include <sstream>
#include <ap_axi_sdata.h>

enum Opcode {
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
	operator std::string() const {
		std::stringstream ss;
		switch (opcode) {
			case SEARCH:
				ss << "Search Response " << ERROR_CODE_NAMES[-search.status] << ", " << search.value.data;
			case INSERT:
				ss << "Insert Response " << ERROR_CODE_NAMES[-insert];
		}
		return ss.str();
	}
} __attribute__((packed));

typedef ap_axiu<sizeof(Request)*8, 0, 0, 0> pkt_req;
typedef ap_axiu<sizeof(Response)*8, 0, 0, 0> pkt_resp;

union req_union {
	Request data;
	ap_uint<sizeof(Request)*8> bytes;
	req_union() { bytes = 0; };
};
union resp_union {
	Response data;
	ap_uint<sizeof(Request)*8> bytes;
	resp_union() { bytes = 0; };
};

pkt_req encode_search_req(search_in_t in);
pkt_req encode_insert_req(insert_in_t in);
pkt_resp encode_search_resp(search_out_t out);
pkt_resp encode_insert_resp(insert_out_t out);

//! @brief State machine to decode and redirect incoming instructions
void sm_decode(
	//! [in]  Incoming instructions
	hls::stream<pkt_req>& requests,
	//! [out] Decoded search instructions
	hls::stream<search_in_t>& searchInput,
	//! [out] Decoded insert instructions
	hls::stream<insert_in_t>& insertInput
);

//! @brief State machine to encode and queue responses for send
void sm_encode(
	//! [out]  Incoming instructions
	hls::stream<pkt_resp>& responses,
	//! [in] Search results
	hls::stream<search_out_t>& searchOutput,
	//! [in] Insert results
	hls::stream<insert_out_t>& insertOutput
);


#endif
