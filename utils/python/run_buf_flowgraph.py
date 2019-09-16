#!/usr/bin/env python3

from gnuradio import gr, eng_notation
from gnuradio import blocks, filter
from gnuradio.eng_arg import eng_float, intx

import os, sys
import time
from graphviz import Source
from argparse import ArgumentParser


class top(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        default_samples = 10e6
        parser = ArgumentParser("Profile GNU Radio Scheduler")
        parser.add_argument("-r", "--run", type=int, default=0,
                            help="the run number (default=%(default)s)")
        parser.add_argument("-p", "--pipes", type=intx, default=1,
                            help="the number of pipelines to create (default=%(default)s)")
        parser.add_argument("-s", "--stages", type=intx, default=1,
                            help="the number of stages in each pipeline (default=%(default)s)")
        parser.add_argument("-t", "--taps", type=intx, default=16,
                            help="the number of taps in the filter of each stage (default=%(default)s)")
        parser.add_argument("-N", "--samples", type=eng_float, default=default_samples,
                            help=("the number of samples to run through the graph (default=%s)" % (eng_notation.num_to_str(default_samples))))
        parser.add_argument("-c", "--config", default="fork",
                          help=(f"the flow graph layout (default=%(default)s)"))
        parser.add_argument("-m", "--machine-readable", action="store_true", help="machine readable output")

        args = parser.parse_args()

        self.run_num = args.run
        self.pipes = args.pipes
        self.stages = args.stages
        self.ntaps = args.taps
        self.samples = args.samples
        self.config = args.config
        self.machine_readable = args.machine_readable

        # Something vaguely like floating point ops
        self.flop = self.ntaps * self.pipes * self.stages * self.samples
        self.taps = self.ntaps*[1.0 / self.ntaps]

        self.fir_blocks = []

        if self.config == "fork":
            self.create_fork()
        elif self.config == "diamond":
            self.create_diamond()
        else:
            print("Unknown config!")
            sys.exit(1)


    def create_fork(self):

        src = blocks.null_source(gr.sizeof_float)
        head = blocks.head(gr.sizeof_float, self.samples)
        self.connect(src, head)

        for pipe in range(self.pipes):
            block = filter.fir_filter_fff(1, self.taps)
            self.connect(head, block)
            self.fir_blocks.append(block)

            for stage in range(1, self.stages):
                block = filter.fir_filter_fff(1, self.taps)
                self.connect(self.fir_blocks[-1], block)
                self.fir_blocks.append(block)

            copy = blocks.copy(gr.sizeof_float)
            copy.set_enabled(True)
            sink = blocks.null_sink(gr.sizeof_float)
            self.connect(self.fir_blocks[-1], copy, sink)


    def create_diamond(self):

        src = blocks.null_source(gr.sizeof_float)
        head = blocks.head(gr.sizeof_float, self.samples)
        self.connect(src, head)
        ends = []

        for pipe in range(self.pipes):
            block = filter.fir_filter_fff(1, self.taps)
            self.connect(head, block)
            self.fir_blocks.append(block)

            for stage in range(1, self.stages):
                block = filter.fir_filter_fff(1, self.taps)
                self.connect(self.fir_blocks[-1], block)
                self.fir_blocks.append(block)

            ends.append(block)

        interleave = blocks.interleave(gr.sizeof_float, 1)
        for i, b in enumerate(ends):
            self.connect((b, 0), (interleave, i))

        sink = blocks.null_sink(gr.sizeof_float)
        self.connect(interleave, sink)


def time_it(tb):
    start = time.time_ns()
    tb.run()
    stop = time.time_ns()

    work_avg = sum(map(lambda x: x.pc_work_time_avg(), tb.fir_blocks)) / len(tb.fir_blocks)
    work_var = sum(map(lambda x: x.pc_work_time_var(), tb.fir_blocks)) / len(tb.fir_blocks)
    input_avg = sum(map(lambda x: x.pc_input_buffers_full_avg(0), tb.fir_blocks)) / len(tb.fir_blocks)
    input_var = sum(map(lambda x: x.pc_input_buffers_full_var(0), tb.fir_blocks)) / len(tb.fir_blocks)
    output_avg = sum(map(lambda x: x.pc_output_buffers_full_avg(0), tb.fir_blocks)) / len(tb.fir_blocks)
    output_var = sum(map(lambda x: x.pc_output_buffers_full_var(0), tb.fir_blocks)) / len(tb.fir_blocks)
    produced_avg  = sum(map(lambda x: x.pc_nproduced_avg(), tb.fir_blocks)) / len(tb.fir_blocks)
    produced_var  = sum(map(lambda x: x.pc_nproduced_var(), tb.fir_blocks)) / len(tb.fir_blocks)
    throughput_avg  = sum(map(lambda x: x.pc_throughput_avg(), tb.fir_blocks)) / len(tb.fir_blocks)

    runtime = (stop-start) / 1e9

    if tb.machine_readable:
        print("%s, %4d, %4d, %4d, %10d, %10d, %10d, %20.10f, %20.10f, %20.10f, %20.10f, %20.10f, %20.10f, %20.10f, %20.10f, %20.10f, %20.15f" % (
            tb.config, tb.run_num, tb.pipes, tb.stages, tb.ntaps, tb.samples, tb.flop, work_avg, work_var, input_avg, input_var,
            output_avg, output_var, produced_avg, produced_var, throughput_avg, runtime))
    else:
        print("run              %20s"   % (tb.run_num,))
        print("config           %20s"   % (tb.config,))
        print("pipes            %20d"   % (tb.pipes,))
        print("stages           %20d"   % (tb.stages,))
        print("samples          %20s"   % (eng_notation.num_to_str(tb.samples),))
        print("pseudo_flop      %20s"   % (eng_notation.num_to_str(tb.flop),))
        print("work_avg         %20.10f" % (work_avg))
        print("work_var         %20.10f" % (work_var))
        print("input_avg        %20.10f" % (input_avg))
        print("input_var        %20.10f" % (input_var))
        print("output_avg       %20.10f" % (output_avg))
        print("output_var       %20.10f" % (output_var))
        print("produced_avg     %20.10f" % (produced_avg))
        print("produced_var     %20.10f" % (produced_var))
        print("throughput_avg   %20.10f" % (throughput_avg))
        print("throughput_avg   %20.10f" % (throughput_avg))
        print("run time         %20.10f" % (runtime))

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

