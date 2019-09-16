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

class PlotThread(QtCore.QThread):
    global interval

    update_sig = QtCore.pyqtSignal(dict)

    def __init__(self):
        super(PlotThread, self).__init__()

    def run(self):
        b = BPF(text=prog)
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-blocks.so", sym="_ZTv0_n152_N2gr6blocks9copy_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-blocks.so", sym="_ZTv0_n152_N2gr6blocks15interleave_impl12general_workEiRSt6vectorIiSaIiEERS2_IPKvSaIS7_EERS2_IPvSaISB_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr5block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr10sync_block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr14sync_decimator12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr17sync_interpolator12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
        b.attach_uretprobe(name="/home/basti/usr/gnuradio/lib/libgnuradio-runtime.so", sym="_ZN2gr19tagged_stream_block12general_workEiRSt6vectorIiSaIiEERS1_IPKvSaIS6_EERS1_IPvSaISA_EE", fn_name="log_tp")
                           
        while 1:
            try:
                sleep(interval);
            except KeyboardInterrupt:
                exit()

            d = {}
            counts = b['tp_hash']
            for k, v in counts.items():
                d[str(k.value)] = v.value
            b['tp_hash'].clear()
            print(d)

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

    #d = filter_dict(d)
    xvals = xvals + interval

    # update existing items
    for k, (c, v) in data.items():
        if k in d:
            data[k] = (c, np.append(v[1:], d[k]/interval))
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

data = {}
interval = 0.5
nvals = 60
xvals = np.cumsum(np.array([interval]*nvals))

app = QtGui.QApplication.instance()

thread = PlotThread()
thread.update_sig.connect(repaint)
thread.finished.connect(app.exit)
thread.start()

sys.exit(app.exec_())
app.exec_()

