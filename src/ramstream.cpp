#include "ramstream.hpp"
#include <iostream>


// For some reason Vitis ignores enum initializers in this file specificlly,
// so that's why states are encoded as bools

static uint_fast64_t req_offset = REQUEST_OFFSET;
static uint_fast64_t resp_offset = RESPONSE_OFFSET;


void sm_ramstream_req(
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
			std::cout << "req_offset=" << req_offset << " ?= " << REQUEST_OFFSET << std::endl;
			if (!requests.full()) {
				req = *((Request*) &hbm[req_offset]);
				req_offset += sizeof(Request);
				if (req.opcode == NOP) {
					state = DONE;
				} else {
					requests.write(req);
					std::cout << "\tDecoded " << static_cast<std::string>(req) << std::endl;
					// Buffer overrun
					if (req_offset >= RESPONSE_OFFSET) {
						state = DONE;
					}
				}
			}
			break;
		case DONE:
			// A reset has occurred
			if (req_offset == REQUEST_OFFSET) {
				state = IDLE;
			}
			break;
	}
}


void sm_ramstream_resp(
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
			std::cout << "resp_offset=" << resp_offset << std::endl;
			responses.read(resp);
			std::cout << "\tEncoded " << static_cast<std::string>(resp) << std::endl;
			*((Response*) &hbm[resp_offset]) = resp;
			resp_offset += sizeof(Response);
			if (responses.empty()) {
				state = IDLE;
			}
			break;
	}
}


void reset_ramstream_offsets() {
	req_offset = REQUEST_OFFSET;
	resp_offset = RESPONSE_OFFSET;
}
