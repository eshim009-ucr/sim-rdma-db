#!/usr/bin/env python3
from enum import Enum
from struct import pack
from sys import argv, exit, stderr


_uint8_t = "B"
_int32_t = "i"
_uint32_t = "I"

_bkey_t = _uint32_t
_bval_t = _int32_t
_ErrorCode = _uint8_t
_Opcode = _uint8_t

_bstatusval_t = _ErrorCode + _bval_t
_KvPair = _bkey_t + _bval_t

insert_in_t = _KvPair
insert_out_t = _ErrorCode
search_in_t = _bkey_t
search_out_t = _bstatusval_t


class ErrorCode(Enum):
	SUCCESS = 0
	KEY_EXISTS = 1
	NOT_IMPLEMENTED = 2
	NOT_FOUND = 3
	INVALID_ARGUMENT = 4
	OUT_OF_MEMORY = 5
	PARENT_FULL = 6

class Opcode(Enum):
	NOP = 0
	SEARCH = 1
	INSERT = 2


def nop_req():
	return pack(f"={_Opcode}8x", Opcode.NOP.value)
def search_req(key):
	return pack(f"={_Opcode}{search_in_t}4x", Opcode.SEARCH.value, key)
def insert_req(key, value):
	return pack(f"={_Opcode}{insert_in_t}", Opcode.INSERT.value, key, value)

def nop_resp():
	return pack(f"={_Opcode}5x", Opcode.NOP.value)
def search_resp(status, value):
	return pack(f"={_Opcode}{search_out_t}", Opcode.SEARCH.value, status, value)
def insert_resp(status):
	return pack(f"={_Opcode}{insert_out_t}4x", Opcode.INSERT.value, status)


def parse_req_resp(req_resp_str):
	split = req_resp_str[:-1].split(",")
	opcode = split[0]
	match opcode:
		case Opcode.NOP.name:
			return (nop_req(), nop_resp())
		case Opcode.SEARCH.name:
			return (
				search_req(int(split[1])),
				search_resp(ErrorCode[split[2]].value, ErrorCode[split[2]].value)
			)
		case Opcode.INSERT.name:
			return (
				insert_req(int(split[1]), int(split[2])),
				insert_resp(ErrorCode[split[3]].value)
			)


if __name__ == '__main__':
	if len(argv) < 2:
		print("Need to give a filename", file=stderr)
		exit(1)
	else:
		fname_in = argv[1]
		fname_req = f"{argv[1][:-4]}_req.bin"
		fname_resp = f"{argv[1][:-4]}_resp.bin"
		with open(fname_in) as fin:
			lines = fin.readlines()
		with open(fname_req, "wb") as fout_req:
			with open(fname_resp, "wb") as fout_resp:
				for line in lines:
					req, resp = parse_req_resp(line)
					fout_req.write(req)
					fout_resp.write(resp)
