// PTh.h  -*- C++ -*-
// $Id: PTh.h,v 1.14 1997-08-04 13:14:46-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// PTh: abstract base class representing a POSIX thread

#ifndef INCLUDED_PTh
#define INCLUDED_PTh

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "PThAttr.h"

// PTh class

class PTh 
{

public:

  PTh();
  virtual ~PTh();

  // API methods
  int Cancel();
  int Create(const PThAttr* p_attr = 0);
  //  int Detach();
  //  int Equal(const PTh& t) const;
  int Join(void** status = 0);
  //  int SetSchedParam(int policy, const struct sched_param* param);
  //  int GetSchedParam(int *policy, struct sched_param* param) const;

  // is thread active?
  bool IsRunning() const;
  bool IsActive() const;

 
#ifdef WIN32
  pthread_t Thread() const;  // return this thread's id
  static pthread_t Self(); // return caller's id
#else
  pth_t Thread() const;  // return this thread's id
  static pth_t Self(); // return caller's id
#endif

  // print contents
  void Dump(FILE* f, int indent = 0) const;

protected:

  // API methods that should only be called by subclass:
  int AtFork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
  void Exit(void* status = 0);
  //  int SigMask(int how, const sigset_t* nset, sigset_t* oset);
  //  int SetCancelState(int state, int* oldstate);
  //  int SetCancelType(int type, int* oldtype);
  //  void TestCancel();
  //  void* GetSpecific(PThKey& key) const;
  //  int SetSpecific(PThKey& key, const void* value);

  // call this before returning from Run
  void DoneRunning();

  // missing: cleanup push/pop, since they are macros, can't be made into methods
#ifdef WIN32
  void YieldSlice();
#else
  void Yield();
#endif

  // pure virtual methods:

  // the thread's actual work routine
  virtual void Run() = 0;

private:

  // operations/methods not implemented
  PTh(const PTh&);
  //operator=(const PTh&);

  // thread entry point
  static void* entry(void *arg);

  char* stackaddr_;

  // thread id
#ifdef WIN32
  pthread_t thread_;
#else
  pth_t thread_;
#endif

  // prevent multiple calls to Create()
  bool running_;

};

#ifdef USE_INLINE
#include "PTh.i"
#endif

#endif /* INCLUDED_PTh */
