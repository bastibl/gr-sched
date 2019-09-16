#ifndef RUN_OPTI_FLOWGRAPH_HPP
#define RUN_OPTI_FLOWGRAPH_HPP

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/copy.h>

#include <iostream>
#include <boost/program_options.hpp>

using namespace gr;

class run_opti_flowgraph {

private:

    int d_pipes;
    int d_stages;
    uint64_t d_samples;
    std::string d_config;

    void create_default();
    void create_opti();
    void create_perf();

public:
    run_opti_flowgraph(int pipes, int stages,
        uint64_t samples, std::string config);
	~run_opti_flowgraph();

	top_block_sptr d_tb;
    std::vector<blocks::copy::sptr> d_blocks;
};


#endif
