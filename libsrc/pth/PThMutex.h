// PThMutex.h  -*- C++ -*-
// $Id: PThMutex.h,v 1.8 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// PThMutex: C++ wrapper for POSIX mutexes

#ifndef INCLUDED_PThMutex
#define INCLUDED_PThMutex

#ifdef __GNUC__
#pragma interface
#endif

#include "../../libsrc/shared/LyraDefs.h"

// PThMutex class

class PThMutex {

public:

  PThMutex();
  ~PThMutex();

  // API methods
  int Init();
  int Lock();
  int TryLock();
  int UnLock();

private:

  // operations/methods not implemented
  PThMutex(const PThMutex&);
  //operator=(const PThMutex&);

  // the mutex object
  //  pth_mutex_t mutex_;
  
#ifdef WIN32
  pthread_mutex_t mutex_;
#else
  pth_mutex_t mutex_;
#endif

  // friend classes
  friend class PThCond;

};

#ifdef USE_INLINE
#include "PThMutex.i"
#endif

#endif /* INCLUDED_PThMutex */
