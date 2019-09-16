cmake_minimum_required(VERSION 3.8)

find_package(Gnuradio "3.9" REQUIRED COMPONENTS blocks)

find_package(gnuradio-sched)
add_executable(run_msg_flowgraph run_msg_flowgraph.cpp)
target_link_libraries(run_msg_flowgraph
  gnuradio::gnuradio-runtime
  gnuradio::gnuradio-pmt
  gnuradio::gnuradio-blocks
  gnuradio::gnuradio-sched
  )

add_executable(run_buf_flowgraph run_buf_flowgraph.cpp)
target_link_libraries(run_buf_flowgraph
  gnuradio::gnuradio-runtime
  gnuradio::gnuradio-blocks
  )

add_executable(run_opti_flowgraph run_opti_flowgraph.cpp)
target_link_libraries(run_opti_flowgraph
  gnuradio::gnuradio-runtime
  gnuradio::gnuradio-blocks
  gnuradio::gnuradio-sched
  )
