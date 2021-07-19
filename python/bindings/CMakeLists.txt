# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT sched_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT sched_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND sched_python_files
    copy_rand_python.cc
    msg_forward_python.cc
    ncopy_python.cc
    python_bindings.cc
    )

GR_PYBIND_MAKE_OOT(sched 
   ../..
   gr::sched
   "${sched_python_files}")

install(TARGETS sched_python DESTINATION ${GR_PYTHON_DIR}/sched COMPONENT pythonapi)
