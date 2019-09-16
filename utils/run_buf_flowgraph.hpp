#ifndef RUN_BUF_FLOWGRAPH_HPP
#define RUN_BUF_FLOWGRAPH_HPP

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/copy.h>

#include <iostream>
#include <boost/program_options.hpp>

using namespace gr;

class run_buf_flowgraph {

private:

    int d_pipes;
    int d_stages;
    uint64_t d_samples;
    std::string d_config;

    void create_fork();
    void create_diamond();

public:
    run_buf_flowgraph(int pipes, int stages,
        uint64_t samples, std::string config);
	~run_buf_flowgraph();

	top_block_sptr tb;
    std::vector<blocks::copy::sptr> blocks;
};


#endif
