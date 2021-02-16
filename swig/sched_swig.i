/* -*- c++ -*- */

#define SCHED_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "sched_swig_doc.i"

%{
#include "sched/ncopy.h"
#include "sched/msg_forward.h"
#include "sched/copy_rand.h"
%}

%include "sched/ncopy.h"
GR_SWIG_BLOCK_MAGIC2(sched, ncopy);
%include "sched/msg_forward.h"
GR_SWIG_BLOCK_MAGIC2(sched, msg_forward);
%include "sched/copy_rand.h"
GR_SWIG_BLOCK_MAGIC2(sched, copy_rand);
