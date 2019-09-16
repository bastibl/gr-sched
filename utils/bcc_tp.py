#!/usr/bin/env python

from bcc import BPF
from time import sleep

prog="""
#include <linux/sched.h>

BPF_HASH(tp_hash, u32);

int log_tp(struct pt_regs *ctx) {
    u64 *val;
    u32 tid = bpf_get_current_pid_tgid();
    u64 zero = 0;
    val = tp_hash.lookup_or_init(&tid, &zero);
    (*val) += PT_REGS_RC(ctx);
    return 0;
}
"""

def handle_output(cpu, data, size):
    event = b['events'].event(data)
    print(event.cpu, event.miss, event.hit)

b = BPF(text=prog)
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-blocks.so", sym="_ZTv0_n152_N2gr6blocks9copy_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-blocks.so", sym="_ZTv0_n152_N2gr6blocks15interleave_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr5block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr10sync_block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr14sync_decimator12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr17sync_interpolator12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr19tagged_stream_block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")

while True:
    sleep(0.1)
    t = b['tp_hash']
    for k, v in t.items():
        print(k, v)
    b['tp_hash'].clear()


