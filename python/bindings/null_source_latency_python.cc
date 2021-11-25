/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(null_source_latency.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(f388fd5235e20fadeaadab87c9964d48)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <sched/null_source_latency.h>
// pydoc.h is automatically generated in the build directory
#include <null_source_latency_pydoc.h>

void bind_null_source_latency(py::module& m)
{

    using null_source_latency    = ::gr::sched::null_source_latency;


    py::class_<null_source_latency, gr::sync_block, gr::block, gr::basic_block,
        std::shared_ptr<null_source_latency>>(m, "null_source_latency", D(null_source_latency))

        .def(py::init(&null_source_latency::make),
           py::arg("item_size"),
           py::arg("granularity"),
           D(null_source_latency,make)
        )
        



        ;




}








