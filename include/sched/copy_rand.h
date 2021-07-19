/* -*- c++ -*- */
/*
 * Copyright 2021 Bastian Bloessl.
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

#ifndef INCLUDED_SCHED_COPY_RAND_H
#define INCLUDED_SCHED_COPY_RAND_H

#include <gnuradio/block.h>
#include <sched/api.h>

namespace gr {
namespace sched {

/*!
 * \brief <+description of block+>
 * \ingroup sched
 *
 */
class SCHED_API copy_rand : virtual public gr::block
{
public:
    typedef std::shared_ptr<copy_rand> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of sched::copy_rand.
     *
     * To avoid accidental use of raw pointers, sched::copy_rand's
     * constructor is in a private implementation
     * class. sched::copy_rand::make is the public interface for
     * creating new instances.
     */
    static sptr make(size_t item_size, size_t max_copy = 0xffffffff);
};

} // namespace sched
} // namespace gr

#endif /* INCLUDED_SCHED_COPY_RAND_H */
