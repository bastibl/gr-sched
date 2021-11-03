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
/* BINDTOOL_HEADER_FILE(ncopy.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(b990247e52e660d5fca74d2570d845cc)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <sched/ncopy.h>
// pydoc.h is automatically generated in the build directory
#include <ncopy_pydoc.h>

void bind_ncopy(py::module& m)
{

    using ncopy    = ::gr::sched::ncopy;


    py::class_<ncopy, gr::sync_block, gr::block, gr::basic_block,
        std::shared_ptr<ncopy>>(m, "ncopy", D(ncopy))

        .def(py::init(&ncopy::make),
           py::arg("ntimes"),
           D(ncopy,make)
        )
        



        ;




}








