#include "run_opti_flowgraph.hpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <functional>
#include <boost/format.hpp>

#include <gnuradio/realtime.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/blocks/interleave.h>

#include <sched/ncopy.h>

namespace po = boost::program_options;

using namespace gr;


run_opti_flowgraph::run_opti_flowgraph(int pipes, int stages,
        uint64_t samples, std::string config):
    d_pipes(pipes), d_stages(stages),
    d_samples(samples), d_config(config) {

    d_tb = gr::top_block::make("msg_flowgraph");

    if(d_config == "default") {
        create_default();
    } else if (d_config == "opti") {
        create_opti();
    } else if (d_config == "perf") {
        create_perf();
    } else {
        throw std::invalid_argument("Unknown Flowgraph Topology");
    }
}

void
run_opti_flowgraph::create_default() {

    for(int pipe = 0; pipe < d_pipes; pipe++) {

        auto src = blocks::null_source::make(sizeof(float));
        // uint64_t norm_samp = (uint64_t)((float)d_samples*3/((float)(d_stages*d_pipes)+2));
        auto head = blocks::head::make(sizeof(float), d_samples);

        d_tb->connect(src, 0, head, 0);

        for(int stage = 0; stage < d_stages; stage++) {
            auto block = blocks::copy::make(sizeof(float));
            if(stage == 0) {
                d_tb->connect(head, 0, block, 0);
            } else {
                d_tb->connect(d_blocks.back(), 0, block, 0);
            }
            d_blocks.push_back(block);
        }

        auto sink = blocks::null_sink::make(sizeof(float));
        d_tb->connect(d_blocks.back(), 0, sink, 0);
    }
}

void
run_opti_flowgraph::create_opti() {

    d_tb->set_max_noutput_items(32768 / 4);
    d_tb->set_max_output_buffer(32768 / 2);

    int cpu_map[] = {2, 3, 6, 7};

    for(int pipe = 0; pipe < d_pipes; pipe++) {

        auto src = blocks::null_source::make(sizeof(float));
        // uint64_t norm_samp = (uint64_t)((float)d_samples*3/((float)(d_stages*d_pipes)+2));
        auto head = blocks::head::make(sizeof(float), d_samples);

        src->set_processor_affinity({cpu_map[pipe]});
        head->set_processor_affinity({cpu_map[pipe]});

        d_tb->connect(src, 0, head, 0);

        for(int stage = 0; stage < d_stages; stage++) {
            auto block = blocks::copy::make(sizeof(float));
            block->set_processor_affinity({cpu_map[pipe]});
            if(stage == 0) {
                d_tb->connect(head, 0, block, 0);
            } else {
                d_tb->connect(d_blocks.back(), 0, block, 0);
            }
            d_blocks.push_back(block);
        }

        auto sink = blocks::null_sink::make(sizeof(float));
        sink->set_processor_affinity({cpu_map[pipe]});
        d_tb->connect(d_blocks.back(), 0, sink, 0);
    }
}


void
run_opti_flowgraph::create_perf() {

    d_tb->set_max_noutput_items(32768 / 4);
    d_tb->set_max_output_buffer(32768 / 4);

    int cpu_map[] = {2, 3, 6, 7};

    for(int pipe = 0; pipe < d_pipes; pipe++) {

        auto src = blocks::null_source::make(sizeof(float));
        src->set_processor_affinity({cpu_map[pipe]});

        // uint64_t norm_samp = (uint64_t)((float)d_samples*3/((float)(d_stages*d_pipes)+2));
        auto head = blocks::head::make(sizeof(float), d_samples);
        head->set_processor_affinity({cpu_map[pipe]});
        d_tb->connect(src, 0, head, 0);

        auto copy = sched::ncopy::make(d_stages);
        copy->set_processor_affinity({cpu_map[pipe]});
        d_tb->connect(head, 0, copy, 0);

        auto sink = blocks::null_sink::make(sizeof(float));
        sink->set_processor_affinity({cpu_map[pipe]});
        d_tb->connect(copy, 0, sink, 0);
    }
}


run_opti_flowgraph::~run_opti_flowgraph () {
}


int main (int argc, char **argv) {
    int run;
    int pipes;
    int stages;
    uint64_t samples;
    std::string config;
    bool machine_readable = false;
    bool rt_prio = false;

    po::options_description desc("Run Optifer Flow Graph");
    desc.add_options()
        ("help,h", "display help")
        ("run,R", po::value<int>(&run)->default_value(0), "Run Number")
        ("pipes,p", po::value<int>(&pipes)->default_value(4), "Number of pipes (set to num cores)")
        ("stages,s", po::value<int>(&stages)->default_value(6), "Number of stages")
        ("samples,N", po::value<uint64_t>(&samples)->default_value(15000000), "Number of samples")
        ("config,c", po::value<std::string>(&config)->default_value("default"), "Optimization Level")
        ("machine_readable,m", "Machine-readable Output")
        ("rt_prio,t", "Enable Real-time priority");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("machine_readable")) {
        machine_readable = true;
    }

    if (vm.count("rt_prio")) {
        rt_prio = true;
    }

    if (rt_prio && gr::enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    run_opti_flowgraph* runner = new run_opti_flowgraph(pipes, stages, samples, config);

    if(!machine_readable) {
        std::cout << boost::format("run         %1$20d") % run << std::endl;
        std::cout << boost::format("pipes       %1$20d") % pipes << std::endl;
        std::cout << boost::format("stages      %1$20d") % stages << std::endl;
        std::cout << boost::format("samples     %1$20d") % samples << std::endl;
        std::cout << boost::format("rt_prio     %1$20s") % rt_prio << std::endl;
        std::cout << boost::format("config      %1$20s") % config << std::endl;

        std::cout << std::endl << runner->d_tb->dot_graph() << std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();
    runner->d_tb->run();
    auto finish = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()/1e9;

    std::string rt = rt_prio ? "rt" : "normal";

    std::vector<float> pc(runner->d_blocks.size());
    auto it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_work_time_avg();});
    float work_avg = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_work_time_var();});
    float work_var = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_input_buffers_full_avg(0);});
    float input_avg = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_input_buffers_full_var(0);});
    float input_var = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_output_buffers_full_avg(0);});
    float output_avg = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_output_buffers_full_var(0);});
    float output_var = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_nproduced_avg();});
    float produced_avg = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_nproduced_var();});
    float produced_var = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();
    it = std::transform(runner->d_blocks.begin(), runner->d_blocks.end(), pc.begin(), [](blocks::copy::sptr f) { return f->pc_throughput_avg();});
    float throughput_avg = std::accumulate(pc.begin(), pc.end(), 0.0) / runner->d_blocks.size();

    std::cout <<
    boost::format("%1$s,  %2$4d, %3$4d,  %4$4d,   %5$15d,%6$s,    %7$20.15f, %8$20.15f,  %9$20.15f, %10$20.15f,  %11$20.15f,  %12$20.15f,   %13$20.15f,    %14$20.15f,    %15$20.15f,    %16$20.15f") %
                  config % run % pipes % stages % samples % rt % work_avg % work_var % input_avg % input_var % output_avg % output_var % produced_avg % produced_var % throughput_avg % time << std::endl;

    return 0;
}
