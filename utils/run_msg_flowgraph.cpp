#include "run_msg_flowgraph.hpp"

#include <gnuradio/realtime.h>

#include <boost/format.hpp>
#include <chrono>

namespace po = boost::program_options;

using namespace gr;


run_msg_flowgraph::run_msg_flowgraph(int pipes, int stages, int burst_size,
        int pdu_size, std::string config, bool debug)
    : d_pipes(pipes), d_stages(stages), d_burst_size(burst_size),
      d_pdu_size(pdu_size), d_config(config), d_debug(debug) {

    this->tb = gr::top_block::make("msg_flowgraph");

    if (d_config == "fork") {
        create_fork();
    } else if (d_config == "diamond") {
        create_diamond();
    } else {
        throw std::invalid_argument("Unknown Flowgraph Topology");
    }
}

void run_msg_flowgraph::create_fork() {

    d_src = sched::msg_forward::make();

    sched::msg_forward::sptr prev;

    for (int pipe = 0; pipe < d_pipes; pipe++) {
        prev = sched::msg_forward::make();
        tb->msg_connect(d_src, "out", prev, "in");

        for (int stage = 1; stage < d_stages; stage++) {
            sched::msg_forward::sptr block = sched::msg_forward::make();
            tb->msg_connect(prev, "out", block, "in");
            prev = block;
        }

        if (d_debug) {
            blocks::message_debug::sptr dbg = blocks::message_debug::make();
            tb->msg_connect(prev, "out", dbg, "store");
            d_debug_sinks.push_back(dbg);
        }
    }
}


void run_msg_flowgraph::create_diamond() {

    d_src = sched::msg_forward::make();

    sched::msg_forward::sptr snk = sched::msg_forward::make();

    sched::msg_forward::sptr prev;

    for (int pipe = 0; pipe < d_pipes; pipe++) {
        prev = sched::msg_forward::make();
        tb->msg_connect(d_src, "out", prev, "in");

        for (int stage = 1; stage < d_stages; stage++) {
            sched::msg_forward::sptr block = sched::msg_forward::make();
            tb->msg_connect(prev, "out", block, "in");
            prev = block;
        }
        tb->msg_connect(prev, "out", snk, "in");
    }
}

run_msg_flowgraph::~run_msg_flowgraph() {}


int main(int argc, char **argv) {
    int run;
    int pipes;
    int stages;
    int repetitions;
    int burst_size;
    int pdu_size;
    std::string config;
    bool machine_readable = false;
    bool rt_prio = false;
    bool debug = false;

    po::options_description desc("Run MSG Flow Graph");
    desc.add_options()
        ("help,h", "display help")
        ("run,R", po::value<int>(&run)->default_value(1), "Run Number")
        ("pipes,p", po::value<int>(&pipes)->default_value(5), "Number of pipes")
        ("stages,s", po::value<int>(&stages)->default_value(6), "Number of stages")
        ("repetitions,r", po::value<int>(&repetitions)->default_value(100), "Number of repetitions")
        ("burst_size,b", po::value<int>(&burst_size)->default_value(0), "Number of PDUs per burst")
        ("pdu_size,S", po::value<int>(&pdu_size)->default_value(100), "Size of PDU in byte")
        ("config,c", po::value<std::string>(&config)->default_value("fork"), "Flowgraph Topology")
        ("machine_readable,m", "Machine-readable Output")
        ("debug,d", "Debug Mode")
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

    if (vm.count("debug")) {
        debug = true;
    }

    if (rt_prio && gr::enable_realtime_scheduling() != RT_OK) {
        std::cout << "Error: failed to enable real-time scheduling." << std::endl;
    }

    run_msg_flowgraph *runner = new run_msg_flowgraph(pipes, stages, burst_size, pdu_size, config, debug);

    if (!machine_readable) {
        std::cout << boost::format("run         %1$20d") % run << std::endl;
        std::cout << boost::format("pipes       %1$20d") % pipes << std::endl;
        std::cout << boost::format("stages      %1$20d") % stages << std::endl;
        std::cout << boost::format("repetitions %1$20d") % repetitions << std::endl;
        std::cout << boost::format("burst_size  %1$20d") % burst_size << std::endl;
        std::cout << boost::format("pdu_size    %1$20d") % pdu_size << std::endl;
        std::cout << boost::format("rt_prio     %1$20s") % rt_prio << std::endl;
        std::cout << boost::format("config      %1$20s") % config << std::endl;
        std::cout << boost::format("debug       %1$20s") % debug << std::endl;
    }

    if (debug) {
        std::cout << "Dot Graph" << std::endl;
        std::cout << std::endl << runner->tb->dot_graph() << std::endl;
    }

    std::string rt = rt_prio ? "rt" : "normal";

    for (int repetition = 0; repetition < repetitions; repetition++) {

        for (int p = 0; p < burst_size; p++) {

            pmt::pmt_t msg =
                    pmt::cons(pmt::PMT_NIL, pmt::make_u8vector(pdu_size, 0x42));
            runner->d_src->post("in", msg);
        }
        pmt::pmt_t msg = pmt::cons(pmt::intern("done"), pmt::from_long(1));
        runner->d_src->post("system", msg);

        auto start = std::chrono::high_resolution_clock::now();
        runner->tb->run();
        auto finish = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            finish - start)
                            .count() /
                    1e9;

        if (debug) {
            for (auto dbg : runner->d_debug_sinks) {
                if (dbg->num_messages() != burst_size) {
                    std::cout << "======= WRONG NUMBER OF MESSAGES ======"
                              << std::endl;
                    std::cout << dbg->alias() << " burst size: " << burst_size
                              << "  received: " << dbg->num_messages()
                              << std::endl;
                }
                for (int i = 0; i < burst_size; i++) {
                    if (!pmt::eqv(dbg->get_message(i),
                                pmt::make_u8vector(pdu_size, 0x42))) {
                        std::cout << "======= WRONG MESSAGE CONTENT ======"
                                  << std::endl;
                        std::cout << dbg->alias() << "want: " << std::endl
                                  << pmt::make_u8vector(pdu_size, 0x42)
                                  << std::endl
                                  << "received: " << std::endl
                                  << dbg->get_message(i) << std::endl;
                    }
                }
            }
        }

        std::cout << boost::format("%1$s,%2$s, %3$4d, %4$4d, %5$4d,  "
                                   "%6$4d,  %7$4d,  %8$4d,  %9$20.12f") %
                             config % rt % run % pipes % stages % repetition %
                             burst_size % pdu_size % time
                  << std::endl;
    }

    return 0;
}
