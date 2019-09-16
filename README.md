# GNU Radio Runtime Benchmarking and Profiling Tools

This is a set of tools and scripts to benchmark and profile the GNU Radio runtime environment and the scheduler.
A more detailed description and exemplary use-cases are shown in:

- Bastian Bloessl, Marcus Müller and Matthias Hollick, “Benchmarking and Profiling the GNU Radio Scheduler,” Proceedings of 9th GNU Radio Conference (GRCon 2019), Huntsville, AL, Sep 2019. [[BibTeX](https://www.bastibl.net/bib/bloessl2019benchmarking/bloessl2019benchmarking.bib), [PDF and Details…](https://www.bastibl.net/bib/bloessl2019benchmarking/)]

There is also a work-in-progress [series of blog posts](https://www.bastibl.net/gnuradio-performance-1/).


## Measurement Setup

Conducting reproducible measurements on a normal operating system is not trivial, as there are many things going on in parallel/in the background.
To get meaningful results, we

- create a CPU set that is used exclusively by GNU Radio. (We also migrate kernel threads out of this CPU set if possible.)
- set the CPU governor to performance to minimize CPU frequency scaling. (Yet, it is not completely avoidable on most CPUs. Cores may be boosted for some time, but all cores cannot run at max frequency permanently.)
- route interrupts away from the dedicated CPU set (i.e., adapt *irq affinity*).

## Metrics

Currently, the focus is on the following metrics:

- **run time**: Measure the run time that is required to process a given workload (PMT messages or samples). That's the most accurate measure, since it doesn't require us to instrument Linux or GNU Radio, i.e., there is zero overhead and no impact on signal processing. All other measures require us to add measurement probes, which have an unavoidable impact on the system.
- **latency**: Tag samples or messages with timestamps and measure how long it takes to propagate them through the flowgraph.
- **produced samples**: The distribution of samples that are produced in one call to `general_work`. A very small number of produced samples (like 1-32) might, for example, indicate suboptimal schedules, since every call to `general_work` introduces overhead.
- **thread interruptions**: When the scheduler interrupts a thread, while it was processing samples in `general_work`, the thread still holds a lock on the samples in its input queues. Upstream blocks, therefore, can only produce a small number of samples, which introduces overhead (see above).
- **thread migrations**: The Linux scheduler tries to balance the workload between CPU cores and migrates threads as needed. Whenever a thread is migrated, it might also trash at least the first-level cache. This is not really true for hyper-threads, which is why we also differentiate between migrations to (1) different NUMA nodes, (2) different CPU cores, and (3) the same hyper-thread.
- **CPU time**: The (distribution) of the time that a threads runs, once it is scheduled by the operating system. This depends heavily on the operating system scheduler and, for examples, varies between real-time threads and normal threads (handled by the Linux CFS scheduler).
- **cache efficiency**: The number of cache hits and misses, in particular the ones of the last-level cache (LLC).


## Installation

This modules comes with some custom GNU Radio blocks for performance measurements. Generally, it is installed like any other GNU Radio module.
Yet, comparing different GNU Radio version, you might have more than one GNU Radio installations on your system. When compiling the module, you have to tell `cmake` which one to use:

```bash
cmake .. [...] -DGnuradio_DIR=/home/basti/usr/gnuradio-maint-3.8/lib/cmake/gnuradio
```

Apart from GNU Radio blocks, this repository comes with C++ GNU Radio flowgraphs that are used for performance measurements. They use the custom block and have to be compiled with something like:

```bash
cd utils
mkdir build
cd build
cmake ..  [...] -DGnuradio_DIR=/home/basti/usr/gnuradio-maint-3.8/lib/cmake/gnuradio -Dgnuradio-sched_DIR=/home/basti/usr/gnuradio-maint-3.8/lib/cmake/sched
```

Use `ldd` on the binaries (libgnuradio-sched.so and run_{buf,msg,opti}_flowgraph) to make sure that they are linked against the indented library versions.
For realistic performance figures, make sure to compile in release mode.

## Usage

### Run Time Performance (Throughput)

You can perform automated throughput measurements as done in the paper cited above.
For now, the scripts are not generic and have to be parameterized manually.

- Edit `utils/create_cpu_set.sh` to configure which CPU cores are assigned exclusively for GNU Radio. (It also migrates kernel threads away from these cores, if possible.)
- Edit `utils/irq_affinity.sh` and assign IRQs to the CPU cores that are not used for performance measurements.
- Edit `utils/Makefile` to set the parameters for the measurement (repetitions, thread priorities, burst sizes, etc)
- Run the measurements with `cd utils; make {buf,msg,opti}`.
- Parse the results with `cd utils; ./parse_{buf,msg,opti}.sh`.
- Evaluate and plot the results with `utils/eval/plot_{buf,msg,opti}.py`.

The results may look similar to this (for details on the setup, please see the paper cited above):

![Message Passing Results](/docs/msg_results.png?raw=true)

### Output Sample Distribution

The `utils/bcc_output_distri.py` script records the number of samples that are recorded in one call to work.
It uses [BCC](https://github.com/iovisor/bcc) to attach user space probes dynamically while GNU Radio is running.
When executed, the script logs the return value of `general_work`, i.e., number of samples that were produced in a log2histogram.
Finally, the histogram is printed in CSV format and can be plotted with `utils/eval/plot_items.py`.

The results may look similar to this (for details on the setup, please see the paper cited above):

![Sample Distribution](/docs/item_results.png?raw=true)

### CPU Migrations

The CPU tries to balance the workload per CPU core by migrating threads to cores with a relatively low load.
This can be measured with `make migrations`, which will

- conduct measurements with the `perf` tool, running a GNU Radio flow graph with normal and real-time priority.
- parse the measurements with the `utils/eval/parse_perf_script.py`

Afterwards, you'll have CSVs, which can be plotted with `uilts/eval/plot_perf_sched.py`.
The results might look like this:

![Thread Migrations](/docs/migrations_cum.png?raw=true)

![Thread Migrations](/docs/migrations_time.png?raw=true)

### Thread Interruptions

Threads can be scheduled out by the operating system scheduler, while they are still processing samples and hold locks on data.
This can lead into problems.
Thread interruptions can be measured with `make resched`. It will
- use perf to collect data for a flow graph with normal and real-time priority
- use the `uilts/eval/parse_perf_sched.py` script to parse the data

The resulting CSV files can be plotted with `utils/eval/plot_perf_sched.py`
This can look like this:

![Thread Interruptions](/docs/resched_cum.png?raw=true)

### CPU Usage Flame Graph

The `utils/flamegraph.sh` script shows a workflow to generate CPU flame graphs.
You'll have to adapt the paths as needed.
It uses perf to randomly sample the instruction pointer and record statistics of the time spent in different code paths.
The data makes only sense if perf can unwind the stack, so you'll have to compile if `-fno-omit-frame-pointer`.

The resulting flame graph is an interactive SVG, i.e., you can zoom into different code paths.
There is an example flowgraph in `utils/default.svg`. Open it in a web browser.

![Flame Graph](/docs/flamegraph_screenshot.png?raw=true)

### GUI Live Monitoring

The `utils/mon_{cache,mig,tp}.py` scripts are GUI versions that use pyqtgraph to display cache hits, CPU migrations, and block throughput over time.

[![Live Monitoring](https://img.youtube.com/vi/sL0eUIInISg/0.jpg)](https://www.youtube.com/watch?v=sL0eUIInISg)

### Live Monitoring Cache Misses

The `uilts/bcc_perf.py` script shows how to access Perf HW events.
It uses [BCC](https://github.com/iovisor/bcc) to poll the number of cache hits and misses per CPU core and prints them once per second.

### Live Monitoring GNU Radio Throughput

The `utils/bcc_perf.py` script monitors the throughput of GNU Radio blocks.
It uses [BCC](https://github.com/iovisor/bcc) to attach user space probes dynamically while the flowgraph is running.
It sums up the number of produces samples and prints them every 0.1 seconds.
When using this script, I experienced a considerable performance degradation.
I'm not sure if this is a problem with BCC, eBPF, or user space probes.
For now, this is not well suited for performance evaluations.

### Live Monitoring CFS Scheduler Run Queue Statistics

The `utils/cfs.bt` script shows various simple ways get data out of the CFS scheduler.
It requires [bpftrace](https://github.com/iovisor/bpftrace).
By default it prints the number of CPU migrations between hyper-threads and CPU cores once per second.
(You'll have to adapt to your CPU topology.)

### User Space Probing

The `utils/trace.bt` script shows various simple [bpftrace](https://github.com/iovisor/bpftrace) scripts.
Most of them use user space probes to record data from GNU Radio functions like `block::general_work` or `block_executor::run_one_iteration`.
