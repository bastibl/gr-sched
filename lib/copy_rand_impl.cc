/* -*- c++ -*- */
/*
 * Copyright 2021 Bastian Bloessl.
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

#include "copy_rand_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace sched {

copy_rand::sptr copy_rand::make(size_t item_size, size_t max_copy)
{
    return gnuradio::get_initial_sptr(new copy_rand_impl(item_size, max_copy));
}


/*
 * The private constructor
 */
copy_rand_impl::copy_rand_impl(size_t item_size, size_t max_copy)
    : d_item_size(item_size),
      d_max_copy(max_copy),
      gr::block("copy_rand",
                gr::io_signature::make(1, 1, item_size),
                gr::io_signature::make(1, 1, item_size))
{
    std::random_device rd;
    d_gen = std::mt19937(rd());
}

/*
 * Our virtual destructor.
 */
copy_rand_impl::~copy_rand_impl() {}

void copy_rand_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = noutput_items;
}

int copy_rand_impl::general_work(int noutput_items,
                                 gr_vector_int& ninput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const uint8_t* in = (const uint8_t*)input_items[0];
    uint8_t* out = (uint8_t*)output_items[0];

    size_t m = std::min(noutput_items, ninput_items[0]);
    m = std::min(m, d_max_copy);
    if (m > 0) {
        std::uniform_int_distribution<> distrib(1, m);
        m = distrib(d_gen);

        std::memcpy(out, in, m * d_item_size);
    }
    consume_each(m);
    return m;
}

} /* namespace sched */
} /* namespace gr */
