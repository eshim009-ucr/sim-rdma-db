#include "ramstream.hpp"


void sm_ramstream_req(
	hls::stream<Request>& requests,
	uint8_t *hbm
) {
	static uint_fast64_t offset = REQUEST_OFFSET;
	static enum {READ, DONE} state = READ;
	Request req;

	if (state == READ && !requests.full()) {
		req = *((Request*) &hbm[offset]);
		if (req.opcode == NOP) {
			state = DONE;
		} else {
			requests.write(req);
		}
		offset += sizeof(Request);
		// Buffer overrun
		if (offset >= RESPONSE_OFFSET) {
			state = DONE;
		}
	}
}


void sm_ramstream_resp(
	hls::stream<Response>& responses,
	uint8_t *hbm
) {
	static uint_fast64_t offset = RESPONSE_OFFSET;
	static enum {IDLE, WRITE} state = READ;
	Response resp;

	switch (state) {
		case IDLE:
			if (!responses.empty()) {
				state = WRITE;
			}
			break;
		case WRITE:
			responses.read(resp);
			*((Response*) &hbm[offset]) = resp;
			offset += sizeof(RESPONSE);
			if (responses.empty()) {
				state = IDLE;
			}
			break;
	}
}
