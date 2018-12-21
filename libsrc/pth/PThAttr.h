// PThAttr.h  -*- C++ -*-
// $Id: PThAttr.h,v 1.7 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// PThAttr: POSIX thread attribute wrapper class

#ifndef INCLUDED_PThAttr
#define INCLUDED_PThAttr

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

// class declarations

class PThAttr {

public:

  PThAttr();
  ~PThAttr();

  // API methods
  int Init();
  // scope is meaningless with pTh user sthreads
  // int GetScope(int* contentionscope)
  // int SetScope(int contentionscope); const;
  //  int SetDetachState(int detachstate);
  //  int GetDetachState(int* detachstate) const;
  int SetJoinable(int joinable); // true = 1, false = 0
  int GetJoinable(int *joinable) const;
  int SetStackSize(unsigned int stacksize);
  int GetStackSize(unsigned int* stacksize) const;
  //  int SetName(char* threadname);
  //  int GetName(char* threadname);
  //int SetStackAddr(void* stackaddr);
  //int GetStackAddr(void** stackaddr) const;
  //  int SetSchedParam(const struct sched_param* param);
  //  int GetSchedParam(struct sched_param* param) const;
  //  int SetSchedPolicy(int policy);
  //  int GetSchedPolicy(int* policy) const;
  //  int SetInheritSched(int inheritsched);
  //  int GetInheritSched(int* inheritsched) const;

private:

  // operations/methods not implemented
  PThAttr(const PThAttr&);
  //operator=(const PThAttr&);

  // the attribute object
  //  pth_attr_t attr_;

#ifdef WIN32
  pthread_attr_t attr_;
#else
  pth_attr_t attr_;
#endif

  // friend classes
  friend class PTh;

};

#ifdef USE_INLINE
#include "PThAttr.i"
#endif

#endif /* INCLUDED_PThAttr */
