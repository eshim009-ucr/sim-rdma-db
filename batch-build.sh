#!/bin/bash
set -e


RERUNS=4
# 10^X
SZ_EMIN=3
SZ_EMAX=6
SZ_MAX=$(( 10 ** $SZ_EMAX ))
SAFE_SZ_MAX=$(( 2 * $SZ_MAX ))
# 2^X
M_EMIN=2
M_EMAX=7


for m_exp in `seq $M_EMIN $M_EMAX`; do
	m=$(( 2 ** $m_exp ))
	echo ' _________________________________'
	echo '/                                 \'
	printf '| BRANCHING FACTOR of 2^%2d = %4d |\n' $m_exp $m
	echo '\_________________________________/'
	cd krnl/core/
	./update-defs.py $m $SAFE_SZ_MAX
	cd ../..

	make build TARGET=hw
	cp build_dir.hw.xilinx_u280*/krnl.xclbin "krnl_m$m.xclbin"
done
