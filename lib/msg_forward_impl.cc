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

#include "msg_forward_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>

namespace gr {
namespace sched {

msg_forward::sptr msg_forward::make() {
    return gnuradio::get_initial_sptr(new msg_forward_impl());
}

msg_forward_impl::msg_forward_impl()
    : gr::block("msg_forward", gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)) {

    message_port_register_out("out");
    message_port_register_in("in");
    set_msg_handler("in", boost::bind(&msg_forward_impl::handle_msg, this, boost::placeholders::_1));
}

msg_forward_impl::~msg_forward_impl() {}

void msg_forward_impl::handle_msg(pmt::pmt_t pdu) {
    message_port_pub("out", pdu);
}


} /* namespace sched */
} /* namespace gr */
