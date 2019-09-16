#ifndef RUN_MSG_FLOWGRAPH_HPP
#define RUN_MSG_FLOWGRAPH_HPP

#include <gnuradio/top_block.h>
#include <sched/msg_forward.h>

#include <iostream>
#include <boost/program_options.hpp>

using namespace gr;

class run_msg_flowgraph {

private:

    int d_pipes;
    int d_stages;
    int d_burst_size;
    int d_pdu_size;
    std::string d_config;

    void create_fork();
    void create_diamond();

public:
    run_msg_flowgraph(int pipes, int stages, int burst_size,
            int pdu_size, std::string config);
	~run_msg_flowgraph();

    top_block_sptr tb;
    sched::msg_forward::sptr src;
};


#endif
