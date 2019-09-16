#!/usr/bin/env python

from bcc import BPF
from time import sleep

import os
import sys
os.environ['PYQTGRAPH_QT_LIB'] = 'PyQt5'

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui

import numpy as np

prog="""
#include <linux/sched.h>

struct key_t {
    char comm[TASK_COMM_LEN];
};

BPF_HASH(mig_hash, struct key_t);

TRACEPOINT_PROBE(sched, sched_migrate_task)
{
    u32 cpu = args->orig_cpu;
    if(cpu==0 || cpu==1 || cpu==4 || cpu==5) {
         return 0;
    }

    struct key_t key = {};
    bpf_get_current_comm(&key.comm, sizeof(key.comm));

    u64 *val;
    u64 zero = 0;
    val = mig_hash.lookup_or_init(&key, &zero);
    (*val)++;

    return 0;
}
"""

class PlotThread(QtCore.QThread):
    global interval

    update_sig = QtCore.pyqtSignal(dict)

    def __init__(self):
        super(PlotThread, self).__init__()

    def run(self):
        b = BPF(text=prog)
        while 1:
            try:
                sleep(interval);
            except KeyboardInterrupt:
                exit()

            d = {}
            counts = b.get_table('mig_hash')
            for k, v in counts.items():
                d[k.comm.encode('string-escape')] = v.value
            b['mig_hash'].clear()

            self.update_sig.emit(d)


def pen_map(s):

    n = 5

    if s.startswith("copy"):
        return (0, n)
    elif s.startswith("head"):
        return (1, n)
    elif s.startswith("null_sink"):
        return (2, n)
    elif s.startswith("null_source"):
        return (2, n)
    else:
        return(n-1, n)

def filter_dict(d):
    ret = {}
    for k, v in d.items():
        if k.startswith("copy"):
            ret[k] = v
        elif k.startswith("head"):
            ret[k] = v
        elif k.startswith("null_sink"):
            ret[k] = v
        elif k.startswith("null_source"):
            ret[k] = v
    return ret

def repaint(d):
    global data, nvals, xvals, p, interval

    d = filter_dict(d)
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
p.setLabel('left', "CPU Migration")
p.setLabel('bottom', "Time (in s)")
#p.setYRange(0, 180)

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

