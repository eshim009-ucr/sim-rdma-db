#include "operations.hpp"


Request encode_search_req(search_in_t in) {
	Request req = {
		.opcode = SEARCH,
		{ .search = in }
	};
	return req;
}

Request encode_insert_req(insert_in_t in) {
	Request req = {
		.opcode = INSERT,
		{ .insert = in }
	};
	return req;
}

Response encode_search_resp(search_out_t out) {
	Response resp = {
		.opcode = SEARCH,
		{ .search = out }
	};
	return resp;
}

Response encode_insert_resp(insert_out_t out) {
	Response resp = {
		.opcode = INSERT,
		{ .insert = out }
	};
	return resp;
}


bool sm_decode(
	hls::stream<Request>& requests,
	//! [out] Decoded search instructions
	hls::stream<search_in_t>& searchInput,
	//! [out] Decoded insert instructions
	hls::stream<insert_in_t>& insertInput
) {
	Request req;
	KvPair pair;
	if (!requests.empty()) {
		requests.read(req);
		switch (req.opcode) {
			case SEARCH:
				searchInput.write(req.search);
				break;
			case INSERT:
				insertInput.write(req.insert);
				break;
		}
	}
	return requests.empty() && searchInput.empty() && insertInput.empty();
}

bool sm_encode(
	hls::stream<Response>& responses,
	hls::stream<search_out_t>& searchOutput,
	hls::stream<insert_out_t>& insertOutput
) {
	search_out_t searchResultRaw;
	insert_out_t insertResultRaw;
	Response searchResultEnc, insertResultEnc;
	if (!searchOutput.empty()) {
		searchOutput.read(searchResultRaw);
		responses.write(encode_search_resp(searchResultRaw));
	}
	if (!insertOutput.empty()) {
		insertOutput.read(insertResultRaw);
		responses.write(encode_insert_resp(insertResultRaw));
	}
	return searchOutput.empty() && insertOutput.empty() && responses.empty();
}
