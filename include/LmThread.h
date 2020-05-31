// LmThread.h  -*- C++ -*-
// $Id: LmThread.h,v 1.22 1997-10-14 21:39:18-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server thread implementation class

#ifndef INCLUDED_LmThread
#define INCLUDED_LmThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "PTh.h"

// forward declarations

class LmConnection;
class LmSrvMesgBuf;
class LmMesgBufPool;
class LmThreadMQ;
class LmThreadHT;
class LmLog;

// the class

class LmThread : public PTh {

public:

  LmThread(LmMesgBufPool* bufpool, LmLog* log);
  LmThread(LmMesgBufPool* bufpool, LmLog* log, char* name);
  virtual ~LmThread();

  void PassMessage(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  bool HandlesMessage(int msgtype) const;

  bool Busy() const;
  int MessagesInQueue() const;

  virtual void Dump(FILE* f, int indent = 0) const;

  LmMesgBufPool* BufferPool() const;

  // message handler function type - must be public
  typedef void (LmThread::*MsgHandler)(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  typedef MsgHandler* MsgHandlerPtr;

  // NOTE: for VC++, we CAN pass subclass methods as function pointers, 
  //		but we CAN'T convert them to/from void*
  //	   for GCC 3.0, we can NOT pass subclass methods as function pointers, 
  //		but we CAN convert them to/from void*
  // Hence, for GCC, we pass everything as a void* and convert it in
  // MessageHandler; for VC++, we pass it as a typed method pointer

#ifdef WIN32
#define MSGHANDLER MsgHandler
#else
//#define MSGHANDLER void*
#define MSGHANDLER MsgHandler
#endif

protected:

  virtual void Run();

  LmLog* Log() const;

  void HandleNextMessage();
  void HandleAvailableMessages();

  bool MessageAvailable() const;

  void RegisterHandler(int msgtype, MsgHandler hfunc);
  void SetDefaultHandler(MsgHandler hfunc);
  MsgHandler GetHandler(int msgtype) const;

  bool Done() const;
  void SetDone(bool done = true);
  //virtual void Yield();
  void ThreadYield();

private:

  // operations/methods not implemented
  LmThread(const LmThread&);
  //operator=(const LmThread&);

  LmThreadMQ* mq_; // message queue
  LmThreadHT* ht_; // message handler table

  MsgHandler default_handler_;
  int messages_handled_;
  int messages_passed_;
  bool done_;
  bool busy_;
  LmMesgBufPool* bp_;
  LmConnection* curr_conn_;
  LmSrvMesgBuf* curr_mbuf_;
  LmLog* log_;
  unsigned long long ticks;
  unsigned long long ticknum;
  char *classname;
  //PthCond condition_;
  //PthMutex mutex_;

};

#ifdef USE_INLINE
#include "LmThread.i"
#endif

// these have to be macros, due to var args; usage: TLOG_Foo(actual args);

#define TLOG_Log       if (Log()) Log()->Log
#define TLOG_Warning   if (Log()) Log()->Warning
#define TLOG_Debug     if (Log()) Log()->Debug
#define TLOG_Error     if (Log()) Log()->Error
#define TLOG_Security  if (Log()) Log()->Security

#endif /* INCLUDED_LmThread */
