// GsPlayerThreadSet.cpp  -*- C++ -*-
// $Id: GsPlayerThreadSet.cpp,v 1.8 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include "GsPlayerThreadSet.h"
#include "GsMain.h"
#include "GsPlayerThread.h"
#include "LmLogFile.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "GsPlayerThreadSet.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

GsPlayerThreadSet::GsPlayerThreadSet(GsMain* gsmain)
  : main_(gsmain)
{
  DECLARE_TheLineNum;
  lock_.Init();
}

////
// Destructor
////

GsPlayerThreadSet::~GsPlayerThreadSet()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_);  // lock object during method
  // cancel and delete any threads in the list
  for (ps_t::iterator i = pthrs_.begin(); !(bool)(i == pthrs_.end()); ++i) {
    GsPlayerThread* thr = *i;
    if (thr->IsRunning()) {
      thr->Cancel();
    }
    LmDELETE(thr);
  }
}

////
// StartIdlePlayerThreads - start up given number of player threads
////

void GsPlayerThreadSet::StartIdlePlayerThreads(int num_threads)
{
  LmLocker mon(lock_);  // lock object during method
  DECLARE_TheLineNum;
  for (int i = 0; i < num_threads; ++i) {
    GsPlayerThread* thr = start_player_thread();
    if (thr) {
      pthrs_.push_front(thr);
    }
  }
}

////
// GetPlayerThread
////

GsPlayerThread* GsPlayerThreadSet::GetPlayerThread()
{
  LmLocker mon(lock_);  // lock object during method
  DEFMETHOD(GsPlayerThreadSet, GetPlayerThread);
  DECLARE_TheLineNum;
  GsPlayerThread* retval = 0;
  // if list is non-empty, get first thread, remove it from list
  if (pthrs_.size() > 0) {
    retval = pthrs_.front();
    pthrs_.pop_front();
  }
  // otherwise allocate a new player thread, start it up
  else {
    retval = start_player_thread();
  }
  return retval;
}

////
// ReturnPlayerThread
////

void GsPlayerThreadSet::ReturnPlayerThread(GsPlayerThread* pthr)
{
  LmLocker mon(lock_);  // lock object during method
  DECLARE_TheLineNum;
  // put thread on list
  pthrs_.push_front(pthr);
}

////
// Dump
////

void GsPlayerThreadSet::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_);  // lock object during method
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<GsPlayerThreadSet[%p,%d]: threads=%d>\n"), this, sizeof(GsPlayerThreadSet), pthrs_.size());
  for (ps_t::const_iterator i = pthrs_.begin(); !(bool)(i == pthrs_.end()); ++i) {
    (*i)->Dump(f, indent + 1);
  }
}

////
// start_player_thread
////

GsPlayerThread* GsPlayerThreadSet::start_player_thread()
{
  DEFMETHOD(GsPlayerThreadSet, start_player_thread);
  DECLARE_TheLineNum;
  // allocate new thread object
  GsPlayerThread* pthr = LmNEW(GsPlayerThread(main_));
  if (!pthr) {
    main_->Log()->Error(_T("%s: could not allocate thread"), method);
    return 0;
  }
  // start player thread, with reasonable stack size 
  PThAttr attr;
  attr.Init();
  attr.SetJoinable( false );
  //attr.SetStackSize(65536);
  attr.SetStackSize(131072);
  if (pthr->Create(&attr) < 0) {
    main_->Log()->Error(_T("%s: could not create player thread"), method);
    LmDELETE(pthr);
    return 0;
  }
  return pthr;
}
