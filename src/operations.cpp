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


void sm_decode(
	hls::stream<Request>& requests,
	IoPairs& opstream
) {
	Request req;
	KvPair pair;
	if (!requests.empty()) {
		requests.read(req);
		switch (req.opcode) {
			case SEARCH:
				opstream.search.input.write(req.search);
				break;
			case INSERT:
				opstream.insert.input.write(req.insert);
				break;
		}
	}
}

void sm_encode(
	hls::stream<Response>& responses,
	IoPairs& opstream
) {
	search_out_t searchResultRaw;
	insert_out_t insertResultRaw;
	Response searchResultEnc, insertResultEnc;
	if (!opstream.search.output.empty()) {
		opstream.search.output.read(searchResultRaw);
		responses.write_nb(encode_search_resp(searchResultRaw));
	}
	if (!opstream.insert.output.empty()) {
		opstream.insert.output.read(insertResultRaw);
		responses.write_nb(encode_insert_resp(insertResultRaw));
	}
}
