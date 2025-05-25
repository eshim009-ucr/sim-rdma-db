#include "operations.hpp"


bool operator==(const Response& lhs, const Response& rhs) {
	if (lhs.opcode != rhs.opcode) return false;
	switch (lhs.opcode) {
		case NOP: return true;
		case SEARCH: return
			lhs.search.status == rhs.search.status &&
			lhs.search.value.data == rhs.search.value.data;
		case INSERT: return (uint_fast8_t) lhs.insert == (uint_fast8_t) rhs.insert;
		default: return memcmp(
				&lhs + sizeof(Opcode),
				&rhs + sizeof(Opcode),
				sizeof(Response)-sizeof(Opcode)
			) == 0;
	}
}

bool operator!=(const Response& lhs, const Response& rhs) {
	return (lhs == rhs) == false;
}

std::string to_str(const Response& resp) {
	std::stringstream ss;
	switch (resp.opcode) {
		case NOP:
			ss << "NOP Response";
			break;
		case SEARCH:
			ss << "Search Response ";
			if (resp.search.status >= 0 && resp.search.status <= 6) {
				ss << ERROR_CODE_NAMES[resp.search.status];
			} else {
				ss << "UNKNOWN";
			}
			ss << '(' << (int) resp.search.status << "), " << resp.search.value.data;
			break;
		case INSERT:
			ss << "Insert Response ";
			if (resp.search.status >= 0 && resp.search.status <= 6) {
				ss << ERROR_CODE_NAMES[resp.search.status];
			} else {
				ss << "UNKNOWN";
			}
			ss << '(' << (int) resp.insert << ')';
			break;
		default:
			ss << "Response Opcode " << (int) resp.opcode;
			break;
	}
	return ss.str();
}


void sm_decode(
	hls::stream<Request>& requests,
	hls::stream<search_in_t>& searchInput,
	hls::stream<insert_in_t>& insertInput,
	uint_fast32_t& opsIn
) {
	Request req;
	KvPair pair;
	if (!requests.empty()) {
		requests.read(req);
		switch (req.opcode) {
			case SEARCH:
				searchInput.write(req.search);
				opsIn++;
				break;
			case INSERT:
				insertInput.write(req.insert);
				opsIn++;
				break;
		}
	}
}

void sm_encode(
	hls::stream<Response>& responses,
	hls::stream<search_out_t>& searchOutput,
	hls::stream<insert_out_t>& insertOutput,
	uint_fast32_t& opsOut
) {
	search_out_t searchResultRaw;
	insert_out_t insertResultRaw;
	Response searchResultEnc, insertResultEnc;
	if (!searchOutput.empty()) {
		searchOutput.read(searchResultRaw);
		responses.write(encode_search_resp(searchResultRaw));
		opsOut++;
	}
	if (!insertOutput.empty()) {
		insertOutput.read(insertResultRaw);
		responses.write(encode_insert_resp(insertResultRaw));
		opsOut++;
	}
}
