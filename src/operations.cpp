#include "operations.hpp"


pkt_req encode_search_req(search_in_t in) {
	req_union req;
	req.data.opcode = SEARCH;
	req.data.search = in;
	pkt_req pkt;
	pkt.data = req.bytes;
	return pkt;
}

pkt_req encode_insert_req(insert_in_t in) {
	req_union req;
	req.data.opcode = INSERT;
	req.data.insert = in;
	pkt_req pkt;
	pkt.data = req.bytes;
	return pkt;
}

pkt_resp encode_search_resp(search_out_t out) {
	resp_union resp;
	resp.data.opcode = SEARCH,
	resp.data.search = out;
	pkt_resp pkt;
	pkt.data = resp.bytes;
	return pkt;
}

pkt_resp encode_insert_resp(insert_out_t out) {
	resp_union resp;
	resp.data.opcode = INSERT;
	resp.data.insert = out;
	pkt_resp pkt;
	pkt.data = resp.bytes;
	return pkt;
}


void sm_decode(
	hls::stream<pkt_req>& requests,
	//! [out] Decoded search instructions
	hls::stream<search_in_t>& searchInput,
	//! [out] Decoded insert instructions
	hls::stream<insert_in_t>& insertInput
) {
	req_union req;
	pkt_req req_pkt;
	KvPair pair;
	if (!requests.empty()) {
		requests.read(req_pkt);
		req.bytes = req_pkt.data;
		switch (req.data.opcode) {
			case SEARCH:
				searchInput.write(req.data.search);
				break;
			case INSERT:
				insertInput.write(req.data.insert);
				break;
		}
	}
}

void sm_encode(
	hls::stream<pkt_resp>& responses,
	hls::stream<search_out_t>& searchOutput,
	hls::stream<insert_out_t>& insertOutput
) {
	search_out_t searchResultRaw;
	insert_out_t insertResultRaw;
	if (!searchOutput.empty()) {
		searchOutput.read(searchResultRaw);
		responses.write(encode_search_resp(searchResultRaw));
	}
	if (!insertOutput.empty()) {
		insertOutput.read(insertResultRaw);
		responses.write(encode_insert_resp(insertResultRaw));
	}
}
