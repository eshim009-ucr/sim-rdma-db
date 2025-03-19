#include "operations.hpp"


Request encode_search_req(search_in_t in) {
	Request req = { .fields = {
		.opcode = SEARCH,
		{ .search = in }
	}};
	return req;
}

Request encode_insert_req(insert_in_t in) {
	Request req = { .fields = {
		.opcode = INSERT,
		{ .insert = in }
	}};
	return req;
}

Response encode_search_resp(search_out_t out) {
	Response resp = { .fields = {
		.opcode = SEARCH,
		{ .search = out }
	}};
	return resp;
}

Response encode_insert_resp(insert_out_t out) {
	Response resp = { .fields = {
		.opcode = INSERT,
		{ .insert = out }
	}};
	return resp;
}


void sm_decode(
	hls::stream<req_bits_t>& requests,
	IoPairs& opstream
) {
	Request req;
	KvPair pair;
	if (!requests.empty()) {
		requests.read(req.bits);
		switch (req.fields.opcode) {
			case SEARCH:
				opstream.search.input.write(req.fields.search);
				break;
			case INSERT:
				opstream.insert.input.write(req.fields.insert);
				break;
		}
	}
}

void sm_encode(
	hls::stream<resp_bits_t>& responses,
	IoPairs& opstream
) {
	search_out_t searchResult;
	insert_out_t insertResult;
	if (!opstream.search.output.empty()) {
		opstream.search.output.read(searchResult);
		responses.write_nb(encode_search_resp(searchResult).bits);
	}
	if (!opstream.insert.output.empty()) {
		opstream.insert.output.read(insertResult);
		responses.write_nb(encode_insert_resp(insertResult).bits);
	}
}
