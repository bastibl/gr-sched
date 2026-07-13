/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SCHED_MSG_SINK_IMPL_H
#define INCLUDED_SCHED_MSG_SINK_IMPL_H

#include <sched/msg_sink.h>

namespace gr {
namespace sched {

class msg_sink_impl : public msg_sink
{
private:
    std::uint64_t d_received;

    void handle_msg(pmt::pmt_t msg);

public:
    msg_sink_impl();
    ~msg_sink_impl();

    std::uint64_t received() const override;
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_MSG_SINK_IMPL_H */
