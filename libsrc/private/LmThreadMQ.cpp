// LmThreadMQ.cpp  -*- C++ -*-
// $Id: LmThreadMQ.cpp,v 1.7 1997-08-20 01:34:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmThreadMQ.h"
#endif

#include "LmThreadMQ.h"
#include "LmThread.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LmThreadMQ.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LmThreadMQ::LmThreadMQ(LmThread* owner)
  : thr_(owner)
{
  DECLARE_TheLineNum;
  lock_.Init();
  notempty_.Init();
}

////
// Destructor
////

LmThreadMQ::~LmThreadMQ()
{
  DECLARE_TheLineNum;
  // get owner thread's buffer pool
  LmMesgBufPool* bp = thr_->BufferPool();
  // free any message buffers still in queue
  for (mq_t::iterator i = q_.begin(); !(bool)(i == q_.end()); ++i) {
    // if there is a buffer pool, return them; otherwise, delete them
    LmSrvMesgBuf* msgbuf = (*i).MessageBuffer();
    if (bp) {
      bp->ReturnBuffer(msgbuf);
    }
    else {
      LmDELETE(msgbuf);
    }
  }
}

////
// Enq: append item to message queue
////

void LmThreadMQ::Enq(const LmThreadMQItem& item)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  //mq_t::iterator back = q_.previous(q_.end());
  //q_.insert_after(back, item);
  q_.push_back(item);
  notempty_.Signal();  // signal that queue is not empty
}

////
// Deq: wait for queue to become non-empty, remove queue item at head, return it
////

LmThreadMQItem LmThreadMQ::Deq()
{
  DECLARE_TheLineNum;
  // lock message queue
  lock_.Lock();
  // wait for queue to become non-empty
  while (q_.empty()) {
    notempty_.Wait(&lock_);
  }
  // remove the first message
  LmThreadMQItem qitem = q_.front();
  q_.pop_front();
  // unlock queue
  lock_.UnLock();
  // return item
  return qitem;
}

////
// MessagesInQueue
////

int LmThreadMQ::MessagesInQueue() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return q_.size();
}  

////
// Dump
////

void LmThreadMQ::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmThreadMQ[%p,%d]: qsize=%d owner=[%p]>\n"), this, sizeof(LmThreadMQ), q_.size(), thr_);
  for (mq_t::const_iterator i = q_.begin(); !(bool)(i == q_.end()); ++i) {
    INDENT(indent + 1, f);
   _ftprintf(f, _T("qitem: msgbuf=[%p] conn=[%p]\n"), (*i).MessageBuffer(), (*i).Connection());
  }
}
