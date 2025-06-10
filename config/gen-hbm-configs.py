#!/usr/bin/env python3

TOTAL_WIDTH=32

nk = 1
mem_width=TOTAL_WIDTH//2
io_width=TOTAL_WIDTH//4

while mem_width >= 1:
	print(f"nk={nk} mem_width={mem_width} io_width={io_width}")
	with open(f"hbm{nk}.cfg", "w") as fout:
		print(f"[connectivity]", file=fout)
		print(f"# Create {nk} kernels", file=fout)
		print(f"nk=krnl:{nk}", file=fout)
		for i in range(nk):
			offset = i*TOTAL_WIDTH//nk
			hbm_start = offset
			hbm_end = hbm_start + mem_width - 1
			req_start = hbm_end + 1
			req_end = req_start + io_width - 1
			resp_start = req_end + 1
			resp_end = resp_start + io_width - 1
			print(f"\n# Kernel {i} memory connections", file=fout)
			if mem_width > 1:
				print(f"sp=krnl_{i}.hbm:HBM[{hbm_start}:{hbm_end}]", file=fout)
			else:
				print(f"sp=krnl_{i}.hbm:HBM[{hbm_start}]", file=fout)
			if io_width > 1:
				print(f"sp=krnl_{i}.req_buffer:HBM[{req_start}:{req_end}]", file=fout)
				print(f"sp=krnl_{i}.resp_buffer:HBM[{resp_start}:{resp_end}]", file=fout)
			else:
				print(f"sp=krnl_{i}.req_buffer:HBM[{req_start}]", file=fout)
				print(f"sp=krnl_{i}.resp_buffer:HBM[{resp_start}]", file=fout)
	nk *= 2
	mem_width >>= 1
	io_width >>= 1
