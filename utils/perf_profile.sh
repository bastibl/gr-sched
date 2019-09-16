#!/bin/bash

sudo perf record -e "probe_libgnuradio:*" -R cset shield --userset=sdr --exec -- build/run_buf_flowgraph -s 10 -p 10 -N 10000000
