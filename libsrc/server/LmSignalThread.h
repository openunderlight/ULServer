// LmSignalThread.h  -*- C++ -*-
// $Id: LmSignalThread.h,v 1.10 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// signal-handling thread class

#ifndef INCLUDED_LmSignalThread
#define INCLUDED_LmSignalThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <signal.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmThread.h"

// class forward declarations

struct LmSignalHandlerTable;

// the class

class LmSignalThread : public LmThread {

public:

  LmSignalThread(LmMesgBufPool* bufpool, LmLog* log);
  ~LmSignalThread();

  void Dump(FILE* f, int indent = 0) const;

  // signal handler function type - must be public
  typedef void (LmSignalThread::*SignalHandler)(int sig);

protected:

  void Run();

  void RegisterSignalHandler(int sig, SignalHandler sigfunc);
  void SetDefaultSignalHandler(SignalHandler sigfunc);


  bool HandlesSignal(int sig) const;

  int WaitForSignal();
  int HandleSignal(int sig);

private:

  // not implemented
  LmSignalThread(const LmSignalThread&);
  //operator=(const LmSignalThread&);

  LmSignalHandlerTable* st_;

  SignalHandler default_sighand_;
#ifndef WIN32
  struct timespec ts_;
  sigset_t allsigs_;
#endif

  int signals_handled_;

};

#ifdef USE_INLINE
#include "LmSignalThread.i"
#endif

#endif /* INCLUDED_LmSignalThread */
