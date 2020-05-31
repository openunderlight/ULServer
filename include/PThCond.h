// PThCond.h  -*- C++ -*-
// $Id: PThCond.h,v 1.8 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// PThCond: wrapper class for POSIX thread condition variables

#ifndef INCLUDED_PThCond
#define INCLUDED_PThCond

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "PThMutex.h"

// PThCond class

class PThCond {

public:

  // constructors
  PThCond();
  ~PThCond();

  // API methods
  int Init();
  int Wait(PThMutex* mutex);
  int TimedWait(PThMutex* mutex, const struct timespec* abstime);
  int Signal();
  int Broadcast();

private:

  // operations/methods not implemented
  PThCond(const PThCond&);
  //operator=(const PThCond&);

  // the condition variable
#ifdef WIN32
  pthread_cond_t cond_;
#else
  st_cond_t cond_;
#endif

};

#ifdef USE_INLINE
#include "PThCond.i"
#endif

#endif /* INCLUDED_PThCond */


