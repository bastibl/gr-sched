#!/usr/bin/env python3

from gnuradio import gr, eng_notation
from gnuradio import blocks, filter
from gnuradio.eng_arg import eng_float, intx
import pmt

import os, sys
import time
import numpy as np
from graphviz import Source
from argparse import ArgumentParser


class top(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        default_npdus = 1e6
        parser = ArgumentParser("Profile GNU Radio Scheduler")
        parser.add_argument("-R", "--run", type=int, default=0,
                            help="the run number (default=%(default)s)")
        parser.add_argument("-p", "--pipes", type=intx, default=1,
                            help="the number of pipelines to create (default=%(default)s)")
        parser.add_argument("-s", "--stages", type=intx, default=1,
                            help="the number of stages in each pipeline (default=%(default)s)")
        parser.add_argument("-r", "--repetitions", type=int, default=10,
                            help="the number of repetitions (default=%(default)s)")
        parser.add_argument("-b", "--burst_size", type=int, default=10,
                            help="the number of PDUs per burst (default=%(default)s)")
        parser.add_argument("-S", "--pdu_size", type=intx, default=8,
                            help="size of PDUs in byte (default=%(default)s)")
        parser.add_argument("-c", "--config", default="fork",
                          help=(f"the flow graph layout (default=%(default)s)"))
        parser.add_argument("-m", "--machine-readable", action="store_true", help="machine readable output")

        args = parser.parse_args()

        self.run_num = args.run
        self.pipes = args.pipes
        self.stages = args.stages
        self.config = args.config
        self.repetitions = args.repetitions
        self.burst_size = args.burst_size
        self.pdu_size = args.pdu_size
        self.machine_readable = args.machine_readable

        if self.config == "fork":
            self.create_fork()
        elif self.config == "diamond":
            self.create_diamond()
        else:
            print("Unknown config!")
            sys.exit(1)


    def create_fork(self):

        self.src = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)

        for pipe in range(self.pipes):
            prev = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)
            self.msg_connect((self.src, 'pdus'), (prev, 'pdus'))

            for stage in range(1, self.stages):
                block = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)
                self.msg_connect((prev, 'pdus'), (block, 'pdus'))
                prev = block


    def create_diamond(self):

        self.src = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)
        snk = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)

        for pipe in range(self.pipes):
            prev = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)
            self.msg_connect((self.src, 'pdus'), (prev, 'pdus'))

            for stage in range(1, self.stages):
                block = blocks.pdu_filter(pmt.intern("foo"), pmt.intern("bar"), False)
                self.msg_connect((prev, 'pdus'), (block, 'pdus'))
                prev = block

            self.msg_connect((prev, 'pdus'), (snk, 'pdus'))


def time_it(tb):

    port = pmt.intern('pdus')

    np_times = np.zeros(tb.repetitions)

    for r in range(tb.repetitions):
        for p in range(tb.burst_size):
            msg = pmt.cons(pmt.PMT_NIL, pmt.make_u8vector(tb.pdu_size, 0x42))
            tb.src.to_basic_block()._post(port, msg)

        tb.src.to_basic_block()._post(
            pmt.intern("system"),
            pmt.cons(pmt.intern("done"), pmt.from_long(1)))

        start = time.time_ns()
        tb.run()
        stop = time.time_ns()

        np_times[r] = (stop-start)/1e9

        if tb.machine_readable:
            print("%s, %4d, %4d, %4d, %4d, %3d, %4d, %20.10f" % (
                tb.config, tb.run_num, tb.pipes, tb.stages, r, tb.burst_size,
                tb.pdu_size, np_times[r]))

    avg_time = np.mean(np_times)
    var_time = np.var(np_times)

    if not tb.machine_readable:
        print("config           %20s"   % (tb.config,))
        print("run              %20d"   % (tb.run_num,))
        print("pipes            %20d"   % (tb.pipes,))
        print("stages           %20d"   % (tb.stages,))
        print("repetitions      %20d"   % (tb.repetitions))
        print("burst_size       %20d"   % (tb.burst_size))
        print("pdu_size         %20d"   % (tb.pdu_size))
        print("avg_time         %20.15f" % (avg_time,))
        print("var_time         %20.15f" % (var_time,))

        # breakpoint()
        # dot = Source(tb.dot_graph())
        # dot.view()


if __name__ == "__main__":
    try:
        if gr.enable_realtime_scheduling() != gr.RT_OK:
            print("Error: failed to enable real-time scheduling.")
        tb = top()
        time_it(tb)
    except KeyboardInterrupt:
        raise sys.exit("interrupted")

