/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SCHED_MSG_BURST_H
#define INCLUDED_SCHED_MSG_BURST_H

#include <gnuradio/block.h>
#include <sched/api.h>

#include <cstdint>

namespace gr {
namespace sched {

class SCHED_API msg_burst : virtual public gr::block
{
public:
    typedef std::shared_ptr<msg_burst> sptr;

    static sptr make(double value, std::uint64_t count);
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_MSG_BURST_H */
