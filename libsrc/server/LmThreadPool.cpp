// LmThreadPool.cpp  -*- C++ -*-
// $Id: LmThreadPool.cpp,v 1.12 1998-04-16 19:03:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmThreadPool.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <map>

#include "LmThreadPool.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/pth/PThMutex.h"
#include "LmLocker.h"
#include "LmThread.h"

#ifndef USE_INLINE
#include "LmThreadPool.i"
#endif

DECLARE_TheFileName;

////
// LmThreadPool::TP definition
////

struct LmThreadPool::TP {

	typedef std::map<int, LmThread*, std::less<int> > p_t;
	p_t p;
	PThMutex p_lock;

	TP();

};

LmThreadPool::TP::TP()
{
  DECLARE_TheLineNum;
  p_lock.Init();
}

////
// Constructor
////

LmThreadPool::LmThreadPool()
  : tp_(LmNEW(TP()))
{
  DECLARE_TheLineNum;
  // empty
}

////
// Destructor
////

LmThreadPool::~LmThreadPool()
{
  DECLARE_TheLineNum;
  // iterate through tp_, cancelling threads and deleting thread pointers
  TP::p_t::iterator i;
  for (i = tp_->p.begin(); !(bool)(i == tp_->p.end()); ++i) {
    LmThread* thr = (*i).second;
    if (thr->IsActive()) {
      thr->Cancel();
    }
    LmDELETE(thr);
  }
  LmDELETE(tp_);
}

////
// AddThread
////

void LmThreadPool::AddThread(int id, LmThread* thread)
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  tp_->p[id] = thread;
}

////
// RemoveThread
////

void LmThreadPool::RemoveThread(int id, bool del)
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  LmThread* thr = get_thread(id);
  // if found
  if (thr != 0) {
    tp_->p.erase(id);
    // delete thread if so indicated
    if (del) {
      LmDELETE(thr);
    }
  }
}

void LmThreadPool::RemoveThread(LmThread* thread, bool del)
{
  LmLocker lock(tp_->p_lock); // lock during this method
  TP::p_t::iterator i;
  for (i = tp_->p.begin(); !(bool)(i == tp_->p.end()); ++i) {
    int id = (*i).first;
    LmThread* thr = (*i).second;
    if (thr == thread) {
      tp_->p.erase(id);
      if (del) {
	LmDELETE(thr);
      }
      return;
    }
  }
}

////
// GetThread
////

LmThread* LmThreadPool::GetThread(int id) const
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  return get_thread(id);
}  

////
// HasThread
////

bool LmThreadPool::HasThread(int id) const
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  return (get_thread(id) != 0);
}

////
// GetThreadIDs: copy id's of threads in pool into a list
////

void LmThreadPool::GetThreadIDs(std::list<int>& tids)
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  TP::p_t::iterator i;
  for (i = tp_->p.begin(); !(bool)(i == tp_->p.end()); ++i) {
    tids.push_back((*i).first);
  }
}

////
// Dump
////

void LmThreadPool::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker lock(tp_->p_lock); // lock during this method
  INDENT(indent, f);
 _ftprintf(f, _T("<LmThreadPool[%p,%d]: threads=%d>\n"), this, sizeof(LmThreadPool), tp_->p.size());
  indent++;
  TP::p_t::iterator i;
  for (i = tp_->p.begin(); !(bool)(i == tp_->p.end()); ++i) {
    LmThread* thr = (*i).second;
    INDENT(indent, f);
   _ftprintf(f, _T("thread(%d): active=%d running=%d\n"), (*i).first, thr->IsActive(), thr->IsRunning());
    thr->Dump(f, indent + 1);
  }
}

////
// get_thread -- assumes map is locked
////

LmThread* LmThreadPool::get_thread(int id) const
{
  DECLARE_TheLineNum;
  TP::p_t::iterator i = tp_->p.find(id);
  if (i == tp_->p.end()) {
    return 0;
  }
  return (*i).second;
}
