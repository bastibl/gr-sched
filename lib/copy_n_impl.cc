/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl.
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

#include "copy_n_impl.h"
#include <gnuradio/io_signature.h>
#include <algorithm>
#include <cstdint>
#include <cstring>

namespace gr {
namespace sched {

copy_n::sptr copy_n::make(size_t item_size, size_t n)
{
    return gnuradio::get_initial_sptr(new copy_n_impl(item_size, n));
}

copy_n_impl::copy_n_impl(size_t item_size, size_t n)
    : gr::block("copy_n",
                gr::io_signature::make(1, 1, item_size),
                gr::io_signature::make(1, 1, item_size)),
      d_item_size(item_size),
      d_n(n)
{
}

copy_n_impl::~copy_n_impl() {}

void copy_n_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int copy_n_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const uint8_t* in = static_cast<const uint8_t*>(input_items[0]);
    uint8_t* out = static_cast<uint8_t*>(output_items[0]);

    size_t m = std::min(static_cast<size_t>(noutput_items),
                        static_cast<size_t>(ninput_items[0]));
    m = std::min(m, d_n);

    if (m > 0) {
        std::memcpy(out, in, m * d_item_size);
    }

    consume_each(m);
    return m;
}

} /* namespace sched */
} /* namespace gr */
