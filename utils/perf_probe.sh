#!/bin/bash

read -r -d '' run_syms <<EOF 
sync_block12general_work
sync_interpolator12general_work
tagged_stream_block12general_work
_ZN2gr5block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE
sync_decimator12general_work
EOF

for s in ${run_syms}
do
    addr=$(objdump /home/basti/usr/gnuradio-master/lib/libgnuradio-runtime.so --syms | grep $s | cut -f1 -d " ")
    echo $addr
    sudo perf probe -x /home/basti/usr/gnuradio-master/lib/libgnuradio-runtime.so "0x${addr}%return \$retval:s32"
done

read -r -d '' block_syms <<EOF 
_ZTv0_n152_N2gr6blocks9copy_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE
_ZTv0_n152_N2gr6blocks15interleave_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE
EOF

for s in ${block_syms}
do
    addr=$(objdump /home/basti/usr/gnuradio-master/lib/libgnuradio-blocks.so --syms | grep $s | cut -f1 -d " ")
    echo $addr
    sudo perf probe -x /home/basti/usr/gnuradio-master/lib/libgnuradio-blocks.so "0x${addr}%return \$retval:s32"
done
