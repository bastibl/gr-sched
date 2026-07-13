/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl <mail@bastibl.net>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "msg_sink_impl.h"

#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace sched {

msg_sink::sptr msg_sink::make()
{
    return gnuradio::get_initial_sptr(new msg_sink_impl());
}

msg_sink_impl::msg_sink_impl()
    : gr::block(
          "msg_sink", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      d_received(0)
{
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { handle_msg(msg); });
}

msg_sink_impl::~msg_sink_impl() {}

void msg_sink_impl::handle_msg(pmt::pmt_t) { ++d_received; }

std::uint64_t msg_sink_impl::received() const { return d_received; }

} // namespace sched
} // namespace gr
