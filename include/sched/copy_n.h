/* -*- c++ -*- */
/*
 * Copyright 2026 Bastian Bloessl.
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

#ifndef INCLUDED_SCHED_COPY_N_H
#define INCLUDED_SCHED_COPY_N_H

#include <gnuradio/block.h>
#include <sched/api.h>

namespace gr {
namespace sched {

/*!
 * \brief Copy input items to the output in fixed-size chunks.
 * \ingroup sched
 *
 * The block copies at most \p n items per general_work() call. It is intended
 * for deterministic scheduler/runtime overhead benchmarks.
 */
class SCHED_API copy_n : virtual public gr::block
{
public:
    typedef std::shared_ptr<copy_n> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sched::copy_n.
     *
     * \param item_size stream item size in bytes
     * \param n maximum number of items copied per general_work() call
     */
    static sptr make(size_t item_size, size_t n);
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_COPY_N_H */
