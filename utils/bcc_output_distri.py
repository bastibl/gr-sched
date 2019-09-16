#!/usr/bin/env python

from bcc import BPF
from time import sleep

prog="""
#include <linux/sched.h>

BPF_HISTOGRAM(item_hist);

int log_tp(struct pt_regs *ctx) {
    //bpf_trace_printk("%d\\n", PT_REGS_RC(ctx));
    item_hist.increment(bpf_log2l(PT_REGS_RC(ctx)));
    return 0;
}
"""

b = BPF(text=prog)
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-blocks.so",  sym="_ZTv0_n120_N2gr6blocks9copy_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE", fn_name="log_tp")
b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr10sync_block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")

# while 1:
#     (task, pid, cpu, flags, ts, msg) = b.trace_fields()
#     print(task, msg)

sleep(20)
#b['item_hist'].print_log2_hist()

t = b['item_hist']
print('bin,value')
for k, v in t.items():
   print("%d,%d" % (k.value, v.value))

