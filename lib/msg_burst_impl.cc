/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "msg_burst_impl.h"

#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace sched {

msg_burst::sptr msg_burst::make(double value, std::uint64_t count)
{
    return gnuradio::get_initial_sptr(new msg_burst_impl(value, count));
}

msg_burst_impl::msg_burst_impl(double value, std::uint64_t count)
    : gr::block(
          "msg_burst", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      d_value(value),
      d_count(count),
      d_start(pmt::mp("start")),
      d_out(pmt::mp("out"))
{
    message_port_register_out(d_out);
    message_port_register_in(d_start);
    set_msg_handler(d_start, [this](pmt::pmt_t msg) { handle_start(msg); });
}

msg_burst_impl::~msg_burst_impl() {}

bool msg_burst_impl::start()
{
    post(d_start, pmt::PMT_T);
    return true;
}

void msg_burst_impl::handle_start(pmt::pmt_t)
{
    for (std::uint64_t i = 0; i < d_count; ++i) {
        message_port_pub(d_out, pmt::from_double(d_value));
    }

    post(pmt::mp("system"), pmt::cons(pmt::intern("done"), pmt::from_long(1)));
}

} // namespace sched
} // namespace gr
