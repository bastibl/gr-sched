/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SCHED_MSG_BURST_IMPL_H
#define INCLUDED_SCHED_MSG_BURST_IMPL_H

#include <sched/msg_burst.h>

namespace gr {
namespace sched {

class msg_burst_impl : public msg_burst
{
private:
    const double d_value;
    const std::uint64_t d_count;
    const pmt::pmt_t d_start;
    const pmt::pmt_t d_out;

    void handle_start(pmt::pmt_t msg);

public:
    msg_burst_impl(double value, std::uint64_t count);
    ~msg_burst_impl();

    bool start() override;
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_MSG_BURST_IMPL_H */
