#!/usr/bin/env python

from bcc import BPF, PerfType, PerfHWConfig, PerfSWConfig
from time import sleep
import multiprocessing

prog="""
#include <linux/ptrace.h>
#include <uapi/linux/bpf_perf_event.h>

BPF_PERF_ARRAY(cache_misses, NUM_CPUS);
BPF_HASH(old_misses, int, u64);

BPF_PERF_ARRAY(cache_hits, NUM_CPUS);
BPF_HASH(old_hits, int, u64);

BPF_PERF_OUTPUT(events);

struct output_t {
    int cpu;
    u64 miss;
    u64 hit;
};

int do_perf(struct bpf_perf_event_data *ctx)
{
    int cpu = bpf_get_smp_processor_id();

    if(cpu == 0 || cpu == 1 || cpu == 4 || cpu == 5) {
        return 0;
    }

    u64 new_miss = cache_misses.perf_read(cpu);
    u64 new_hit = cache_hits.perf_read(cpu);

    u64 zero = 0;
    u64 *old_miss = old_misses.lookup_or_init(&cpu, &zero);
    u64 *old_hit = old_hits.lookup_or_init(&cpu, &zero);

    struct output_t out = {};
    out.cpu = cpu;
    out.miss = new_miss - *old_miss;
    out.hit = new_hit - *old_hit;
    *old_miss = new_miss;
    *old_hit = new_hit;

    events.perf_submit(ctx, &out, sizeof(out));

    return 0;
}

"""

def handle_output(cpu, data, size):
    event = b['events'].event(data)
    print(event.cpu, event.miss, event.hit)

b = BPF(text=prog, cflags=['-DNUM_CPUS=%d' % multiprocessing.cpu_count()])
b['cache_misses'].open_perf_event(PerfType.HARDWARE, PerfHWConfig.CACHE_MISSES)
b['events'].open_perf_buffer(handle_output)
b.attach_perf_event(ev_type=PerfType.SOFTWARE, ev_config=PerfSWConfig.CPU_CLOCK, fn_name='do_perf', sample_freq=1)

while True:
    b.perf_buffer_poll()

