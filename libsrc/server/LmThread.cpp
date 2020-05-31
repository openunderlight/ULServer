// LmThread.cxpp  -*- C++ -*-
// $Id: LmThread.cpp,v 1.29 1997-09-16 03:02:33-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmThread.h"
#endif

#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <map>

#include "LmThread.h"
#include "LmLocker.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmConnection.h"
#include "LmThreadMQ.h"
#include "LmLog.h"
#include "LmRand.h"
#include <cxxabi.h>

#ifndef USE_INLINE
#include "LmThread.i"
#endif

DECLARE_TheFileName;

////
// LmThreadHT implementation
////

class LmThreadHT : public std::map<int, LmThread::MsgHandler, std::less<int> > {
  // no additional methods
};

////
// Constructor
////
const unsigned long long base_tick_num = 6000;
LmThread::LmThread(LmMesgBufPool* bufpool, LmLog* log)
  : LmThread::LmThread(bufpool, log, strdup("unnamed"))
{ 
}

LmThread::LmThread(LmMesgBufPool* bufpool, LmLog* log, char* name)
  : PTh::PTh(name),
    mq_(LmNEW(LmThreadMQ(this))),
    ht_(LmNEW(LmThreadHT())),
    done_(false),
    busy_(false),
    bp_(bufpool),
    messages_handled_(0),
    messages_passed_(0),
    default_handler_(0),
    curr_conn_(0),
    curr_mbuf_(0),
    log_(log),
    ticks(0)
{
  DECLARE_TheLineNum;
  ticknum = base_tick_num + LmRand::Generate(1, base_tick_num);
  const char *mname = typeid(this).name();
  classname = abi::__cxa_demangle(mname, nullptr, nullptr, nullptr);
}

////
// Destructor
////

LmThread::~LmThread()
{
  DECLARE_TheLineNum;
  // free the handler table
  LmDELETE(ht_);
  // free the message buffers
  LmDELETE(mq_);
  // NB: the thread's execution is not stopped here, it is assumed that
  // something else causes the thread to exit BEFORE the destructor is invoked;
  // if a thread continue execution after this destructor is called then
  // bad things happen
}

void LmThread::ThreadYield()
{
  PTh::Yield();
  ticks++;
  if(ticks % ticknum == 0) {
    TLOG_Debug(_T("%s: %llu ticks."), classname, ticks);
  }
}

////
// Run: thread _tmain() function
////

void LmThread::Run()
{
  DECLARE_TheLineNum;
  // this looks simple, doesn't it?
  SetDone(false);
  while (!Done()) {
    HandleNextMessage();
    ThreadYield();
  }
  TLOG_Debug(_T("LmThread::Run exiting"));
  DoneRunning();
} 

////
// PassMessage
////

void LmThread::PassMessage(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DECLARE_TheLineNum;
  mq_->Enq(LmThreadMQItem(msgbuf, conn));  // add message to queue
  messages_passed_++;
}

////
// MessageAvailable
////

bool LmThread::MessageAvailable() const
{
  DECLARE_TheLineNum;
  return (mq_->MessagesInQueue() > 0);
}

////
// MessagesInQueue
////

int LmThread::MessagesInQueue() const
{
  DECLARE_TheLineNum;
  return mq_->MessagesInQueue();
}  

////
// HandlesMessage: return true if message has a registered handler
////

bool LmThread::HandlesMessage(int msgtype) const
{
  DECLARE_TheLineNum;
  return (!(bool)(ht_->find(msgtype) == ht_->end())) ;
}

////
// RegisterHandler
////

void LmThread::RegisterHandler(int msgtype, MsgHandler hfunc)
{
  DECLARE_TheLineNum;

  (*ht_)[msgtype] = (MsgHandler)hfunc;
}

////
// GetHandler
////

LmThread::MsgHandler LmThread::GetHandler(int msgtype) const
{
  DECLARE_TheLineNum;
  MsgHandler mfunc = default_handler_;
  LmThreadHT::iterator i = ht_->find(msgtype);
  if (!(bool)(i == ht_->end())) {
    mfunc = (*i).second;
  }
  return mfunc;
}

////
// HandleAvailableMessages - handle any queued messages, and return
////

void LmThread::HandleAvailableMessages()
{
  DECLARE_TheLineNum;
  while (MessageAvailable() && !Done()) {
    HandleNextMessage(); // this won't block
  }
}

////
// HandleNextMessage
////

void LmThread::HandleNextMessage()
{
  DECLARE_TheLineNum;
  // get next message from queue (will wait)
  LmThreadMQItem qitem = mq_->Deq();
  // we got a message
  busy_ = true;
  curr_conn_ = qitem.Connection();
  curr_mbuf_ = qitem.MessageBuffer();
  // look up message handler
  MsgHandler mfunc = GetHandler(qitem.MessageBuffer()->Header().MessageType());
  // call handler if it exists
  if (mfunc) {
    (this->*mfunc)(qitem.MessageBuffer(), qitem.Connection());
    messages_handled_++;
  }
  // if buffer is managed, return it to the pool, otherwise delete it
  if (bp_) {
    bp_->ReturnBuffer(qitem.MessageBuffer());
  }
  else {
    LmDELETE(qitem.MessageBuffer());
  }
#if 0
  // if connection exists, signal end of message reading
  // TODO: move this into message reader thread?
  if (qitem.Connection()) {
    qitem.Connection()->EndMessageReading();
  }
#endif
  // done processing
  busy_ = false;
  curr_conn_ = 0;
  curr_mbuf_ = 0;
}

////
// Dump
////

void LmThread::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmThread[%p,%d]: queue=[%p] htable=[%p] bufpool=[%p] log=[%p] handled=%d/%d busy=%d currbuf=[%p] currconn=[%p]>\n"),
	  this, sizeof(LmThread), mq_, ht_, bp_, log_, messages_handled_, messages_passed_, busy_,
	  curr_mbuf_, curr_conn_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("handlers (%d): "), ht_->size());
  for (LmThreadHT::iterator i = ht_->begin(); !(bool)(i == ht_->end()); ++i) {
   _ftprintf(f, _T("%d "), (*i).first);
  }
  if (default_handler_) {
   _ftprintf(f, _T("default"));
  }
 _ftprintf(f, _T("\n"));
  mq_->Dump(f, indent + 1);
  // base class
  PTh::Dump(f, indent + 1);
}
