// PThCond.i  -*- C++ -*-
// $Id: PThCond.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// optionally inlined methods

#include <sys/time.h>

INLINE PThCond::PThCond()
{
}

INLINE PThCond::~PThCond()
{
#ifdef WIN32
  pthread_cond_destroy(&cond_);
#endif
}

INLINE int PThCond::Init()
{
#ifdef WIN32
  return pthread_cond_init(&cond_, 0);
#else
  cond_ = st_cond_new();
  return 0;
#endif
}


INLINE int PThCond::Wait(PThMutex* mutex)
{
#ifdef WIN32
  return pthread_cond_wait(&cond_, &(mutex->mutex_));
#else
  mutex->Lock();
  st_cond_wait(cond_);
  mutex->UnLock();
#endif
}

INLINE int PThCond::TimedWait(PThMutex* mutex, const struct timespec* abstime)
{
  struct timeval now;
  gettimeofday(&now, nullptr);
  return st_cond_timedwait(cond_, llabs((abstime->tv_nsec * 1000) - now.tv_usec));
}

INLINE int PThCond::Signal()
{
#ifdef WIN32
  return pthread_cond_signal(&cond_);
#else
    return st_cond_signal(cond_);
#endif
}

INLINE int PThCond::Broadcast()
{
#ifdef WIN32
  return pthread_cond_signal(&cond_);
#else
  return st_cond_signal(cond_);
#endif
}



