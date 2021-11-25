/* -*- c++ -*- */
/*
 * Copyright 2021 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SCHED_NULL_SOURCE_LATENCY_IMPL_H
#define INCLUDED_SCHED_NULL_SOURCE_LATENCY_IMPL_H

#include <sched/null_source_latency.h>

namespace gr {
namespace sched {

class null_source_latency_impl : public null_source_latency
{
private:
    // Nothing to declare in this block.
    uint64_t d_granularity;
    size_t d_item_size;

public:
    null_source_latency_impl(size_t item_size, uint64_t granularity);
    ~null_source_latency_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_NULL_SOURCE_LATENCY_IMPL_H */
