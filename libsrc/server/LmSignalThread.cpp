// LmSignalThread.cpp  -*- C++ -*-
// $Id: LmSignalThread.cpp,v 1.23 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmSignalThread.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <signal.h>
#include <string.h>
#include <map>

#include "LmSignalThread.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmSignalThread.i"
#endif

DECLARE_TheFileName;

// dummy signal handler routine, that (should) never be called
static void dummy_handler(int /* sig */)
{
  // TODO: ?
}

////
// LmSignalHandlerTable implementation
////

struct LmSignalHandlerTable : public std::map<int, LmSignalThread::SignalHandler, std::less<int> > {
  // no extra methods
};

////
// Constructor
////

LmSignalThread::LmSignalThread(LmMesgBufPool* bufpool, LmLog* log)
  : LmThread(bufpool, log),
    st_(LmNEW(LmSignalHandlerTable())),
    default_sighand_(0),
    signals_handled_(0)
{
#ifndef WIN32
  DECLARE_TheLineNum;
  // initialize delay to 15 seconds
  memset(&ts_, 0, sizeof(ts_));
  ts_.tv_sec = 15;
  ts_.tv_nsec = 0;

  // to use the pth_sigwait, we must add the signals one by one
  // note: use of all signals except SIGTERM deprecated
  sigemptyset(&allsigs_);
  sigaddset(&allsigs_, SIGTERM);
//  sigaddset(&allsigs_, SIGHUP);
 // sigaddset(&allsigs_, SIGALRM);
 // sigaddset(&allsigs_, SIGUSR1);
 // sigaddset(&allsigs_, SIGUSR2);
 // sigaddset(&allsigs_, SIGINT);
  //  sigaddset(&allsigs_, SIGSIGCHLD);
#endif
}

////
// Destructor
////

LmSignalThread::~LmSignalThread()
{
  DECLARE_TheLineNum;
#ifndef WIN32
  LmDELETE(st_);
#endif
}

////
// Run
////

void LmSignalThread::Run()
{
#ifndef WIN32
  DECLARE_TheLineNum;
  // modified LmThread main loop
  SetDone(false);
  // unblock all signals
  pth_sigmask(SIG_UNBLOCK, &allsigs_, NULL);
  while (!Done()) {
    int sig = WaitForSignal();
    if (sig != -1) {
      HandleSignal(sig);
    }
    HandleAvailableMessages();
    Yield();
  }
  DoneRunning();
#endif
}

////
// WaitForSignal: waits (up to the time limit) for a signal; returns it, or -1 if error
////

int LmSignalThread::WaitForSignal()
{
  int sig = 0;
#ifndef WIN32
  DECLARE_TheLineNum;
  // wait for any and all signals that can be caught
  int rc = sigwait(&allsigs_, &sig);
  if (rc < 0) {
    return -1;
  }
#endif
  return sig;
}

////
// HandleSignal: calls signal handler for given signal, if it exists; returns 0 if
//   handler exists, -1 if not; calls default signal handler if it exists.
////

int LmSignalThread::HandleSignal(int sig)
{
#ifndef WIN32
  DECLARE_TheLineNum;
  signals_handled_++;
  // get handler function
  SignalHandler sfunc = default_sighand_;
  LmSignalHandlerTable::iterator i = st_->find(sig);
  if (i != st_->end()) {
    sfunc = (*i).second;
  }
  // call it if it exists
  if (sfunc) {
    (this->*sfunc)(sig);
    return 0;
  }
#endif
  return -1;
}

////
// RegisterSignalHandler
////

void LmSignalThread::RegisterSignalHandler(int sig, SignalHandler sigfunc)
{
#ifndef WIN32
  DECLARE_TheLineNum;
  (*st_)[sig] = sigfunc;
  // install dummy handler
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = dummy_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(sig, &sa, NULL);
#endif
}

////
// HandlesSignal
////

bool LmSignalThread::HandlesSignal(int sig) const
{
#ifndef WIN32
  DECLARE_TheLineNum;
  return (!(bool)(st_->find(sig) == st_->end()));
#else
  return false;
#endif
}

////
// Dump
////

void LmSignalThread::Dump(FILE* f, int indent) const
{
#ifndef WIN32
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmSignalThread[%p,%d]: sigs=%d handlers=%d>\n"), this, sizeof(LmSignalThread),
	  signals_handled_, st_->size());
  if (st_->size() > 0) {
    INDENT(indent + 1, f);
   _ftprintf(f, _T("handled: "));
    for (LmSignalHandlerTable::iterator i = st_->begin(); !(bool)(i == st_->end()); ++i) {
     //_ftprintf(f, "[%d, '%s'] ", (*i).first, strsignal((*i).first));
		// BMP 2/01: strsignal call removed because apparently it no longer
		// exists!
     _ftprintf(f, _T("[%d] "), (*i).first);

    }
   _ftprintf(f, _T("\n"));
  }
  LmThread::Dump(f, indent + 1);
#endif
}
