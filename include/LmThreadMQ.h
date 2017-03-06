// LmThreadMQ.h  -*- C++ -*-
// $Id: LmThreadMQ.h,v 1.5 1997-08-20 01:34:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// thread message queue

#ifndef INCLUDED_LmThreadMQ
#define INCLUDED_LmThreadMQ

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

//#include <slist.h>
#include <list>

#include "LyraDefs.h"
#include "PThMutex.h"
#include "PThCond.h"

// forward declarations

class LmConnection;
class LmSrvMesgBuf;
class LmThread;

// queue element class

class LmThreadMQItem {

public:

  LmThreadMQItem(LmSrvMesgBuf* b = 0, LmConnection* c = 0);

  LmSrvMesgBuf* MessageBuffer() const;
  LmConnection* Connection() const;

private:

  LmSrvMesgBuf* msgbuf_;
  LmConnection* conn_;

};

// message queue class

class LmThreadMQ {

public:

  LmThreadMQ(LmThread* owner);
  ~LmThreadMQ();

  void Enq(const LmThreadMQItem& item);
  LmThreadMQItem Deq();

  int MessagesInQueue() const;

  void Dump(FILE* f, int indent = 0) const;

private:

  // operations/methods not implemented
  LmThreadMQ(const LmThreadMQ&);
  //operator=(const LmThreadMQ&);

  // object lock, and "not empty" condition variable
  PThMutex lock_;
  PThCond notempty_;

  // message queue
  //typedef slist<LmThreadMQItem> mq_t;
  typedef std::list<LmThreadMQItem> mq_t;
  mq_t q_;

  // thread that owns queue
  LmThread* thr_;

};

#ifdef USE_INLINE
#include "LmThreadMQ.i"
#endif

#endif /* INCLUDED_LmThreadMQ */
