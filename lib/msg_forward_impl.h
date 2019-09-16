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

#ifndef INCLUDED_SCHED_MSG_FORWARD_IMPL_H
#define INCLUDED_SCHED_MSG_FORWARD_IMPL_H

#include <sched/msg_forward.h>

namespace gr {
namespace sched {

class msg_forward_impl : public msg_forward {
private:
    // Nothing to declare in this block.
    void handle_msg(pmt::pmt_t pdu);

public:
    msg_forward_impl();
    ~msg_forward_impl();
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_MSG_FORWARD_IMPL_H */
