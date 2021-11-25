/* -*- c++ -*- */
/*
 * Copyright 2021 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SCHED_NULL_SOURCE_LATENCY_H
#define INCLUDED_SCHED_NULL_SOURCE_LATENCY_H

#include <gnuradio/sync_block.h>
#include <sched/api.h>

namespace gr {
namespace sched {

/*!
 * \brief <+description of block+>
 * \ingroup sched
 *
 */
class SCHED_API null_source_latency : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<null_source_latency> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sched::null_source_latency.
     *
     * To avoid accidental use of raw pointers, sched::null_source_latency's
     * constructor is in a private implementation
     * class. sched::null_source_latency::make is the public interface for
     * creating new instances.
     */
    static sptr make(size_t item_size, uint64_t granularity);
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_NULL_SOURCE_LATENCY_H */
