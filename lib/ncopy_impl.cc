/* -*- c++ -*- */
/*
 * Copyright 2019 Bastian Bloessl <mail@bastibl.net>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ncopy_impl.h"
#include <gnuradio/io_signature.h>

#include <new>

namespace gr {
namespace sched {

ncopy::sptr ncopy::make(int ntimes)
{
    return gnuradio::get_initial_sptr(new ncopy_impl(ntimes));
}


ncopy_impl::ncopy_impl(int ntimes)
    : gr::sync_block("ncopy",
                     gr::io_signature::make(1, 1, sizeof(float)),
                     gr::io_signature::make(1, 1, sizeof(float))),
      d_n(ntimes)
{

    if (ntimes > 99) {
        throw std::bad_alloc();
    }
}

ncopy_impl::~ncopy_impl() {}

int ncopy_impl::work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
{

    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    if (d_n < 2) {
        std::memcpy(out, in, noutput_items * sizeof(float));
        return noutput_items;
    }

    std::memcpy(buffers[0], in, noutput_items * sizeof(float));

    for (int i = 0; i < d_n - 2; i++) {
        std::memcpy(buffers[i + 1], buffers[i], noutput_items * sizeof(float));
    }

    std::memcpy(out, buffers[d_n - 2], noutput_items * sizeof(float));

    return noutput_items;
}

} /* namespace sched */
} /* namespace gr */
