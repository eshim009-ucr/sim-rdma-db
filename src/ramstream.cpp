#include "ramstream.hpp"
#include <iostream>


static uint_fast64_t req_offset = 0;
static uint_fast64_t resp_offset = 0;


bool sm_ramstream_req(
	hls::stream<Request>& requests,
	uint8_t *hbm
) {
	static enum {IDLE, READ, DONE} state = IDLE;
	Request req;

	switch (state) {
		case IDLE:
			state = READ;
			break;
		case READ:
			if (!requests.full()) {
				req = *((Request*) &hbm[req_offset]);
				req_offset += sizeof(Request);
				if (req.opcode == NOP) {
					state = DONE;
				} else {
					requests.write(req);
					// Buffer overrun
					if (req_offset >= NUM_REQUESTS*sizeof(Request)) {
						state = DONE;
					}
				}
			}
			break;
		case DONE:
			// A reset has occurred
			if (req_offset == 0) {
				state = IDLE;
			}
			break;
	}
	return state == DONE;
}


bool sm_ramstream_resp(
	hls::stream<Response>& responses,
	uint8_t *hbm
) {
	static enum {IDLE, WRITE, RESET} state = IDLE;
	Response resp;

	switch (state) {
		case IDLE:
			if (!responses.empty()) {
				state = WRITE;
			}
			break;
		case WRITE:
			responses.read(resp);
			*((Response*) &hbm[resp_offset]) = resp;
			resp_offset += sizeof(Response);
			if (responses.empty()) {
				state = IDLE;
			}
			break;
	}
	return state == IDLE;
}


void reset_ramstream_offsets() {
	req_offset = 0;
	resp_offset = 0;
}
