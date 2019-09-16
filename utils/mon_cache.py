#!/usr/bin/env python

from bcc import BPF, PerfType, PerfHWConfig, PerfSWConfig
from time import sleep

import os
import sys
os.environ['PYQTGRAPH_QT_LIB'] = 'PyQt5'
import multiprocessing

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui

import numpy as np

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

class PlotThread(QtCore.QThread):
    global interval

    update_sig = QtCore.pyqtSignal(dict)

    def __init__(self):
        super(PlotThread, self).__init__()
        self.d = {}
        self.first = True


    def handle_output(self, cpu, data, size):
        event = self.b['events'].event(data)

        self.d['miss ' + str(event.cpu)] = event.miss
        self.d['hit ' + str(event.cpu)] = event.hit

        if len(self.d.keys()) >= 8:
            if self.first:
                self.first = False
            else:
                self.update_sig.emit(self.d)
                self.d = {}

            
    def run(self):
        self.b = BPF(text=prog, cflags=['-DNUM_CPUS=%d' % multiprocessing.cpu_count()])
        self.b['cache_misses'].open_perf_event(PerfType.HARDWARE, PerfHWConfig.CACHE_MISSES)
        self.b['cache_hits'].open_perf_event(PerfType.HARDWARE, PerfHWConfig.CACHE_REFERENCES)
        self.b['events'].open_perf_buffer(self.handle_output)
        self.b.attach_perf_event(ev_type=PerfType.SOFTWARE, ev_config=PerfSWConfig.CPU_CLOCK, fn_name='do_perf', sample_freq=int(1.0/interval))

        while True:
            try:
                self.b.perf_buffer_poll()
            except KeyboardInterrupt:
                exit()


def pen_map(s):
    cols = ('b', 'g', 'r', 'c', 'm', 'y', 'k', 'w')
    col = 'b'
    if s[-1] == '2':
        col = 'c'
    elif s[-1] == '3':
        col = 'y'
    elif s[-1] == '6':
        col = 'g'
    elif s[-1] == '7':
        col = 'm'

    if s.startswith('hit'):
        return pg.mkPen(col, width=1.5)
    else:
        return pg.mkPen(col, width=1.5, style=QtCore.Qt.DashLine)

def repaint(d):
    global data, nvals, xvals, p, interval

    xvals = xvals + interval

    # update existing items
    for k, (c, v) in data.items():
        if k in d:
            data[k] = (c, np.append(v[1:], d[k]))
            del(d[k])
        else:
            data[k] = (c, np.append(v[1:], 0))

    # add new items
    for k, v in d.items():
        data[k] = (p.plot(xvals, [0]*nvals, pen=pen_map(k), name=k), np.append(np.array([0]*(nvals-1)), v))

    # delete outdated
    for k, (c, v) in data.items():
        if np.max(v) == 0:
            c.clear()
            p.legend.removeItem(k)
            del(data[k])

    # plot
    for k, v in data.items():
        v[0].setData(xvals, v[1])
    
    

win = pg.GraphicsWindow()
win.setWindowTitle('GR Perf Monitoring')
p = win.addPlot()
p.addLegend()
p.setLabel('left', 'Cache Hits/Misses')
p.setLabel('bottom', 'Time (in s)')
#p.setYRange(0, 3.5e6)

data = {}
interval = 0.1
nvals = 60
xvals = np.cumsum(np.array([interval]*nvals))

app = QtGui.QApplication.instance()

thread = PlotThread()
thread.update_sig.connect(repaint)
thread.finished.connect(app.exit)
thread.start()

sys.exit(app.exec_())
app.exec_()

