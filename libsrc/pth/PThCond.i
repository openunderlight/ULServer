// PThCond.i  -*- C++ -*-
// $Id: PThCond.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// optionally inlined methods

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
  return pth_cond_init(&cond_);
#endif
}


INLINE int PThCond::Wait(PThMutex* mutex)
{
#ifdef WIN32
  return pthread_cond_wait(&cond_, &(mutex->mutex_));
#else
  pth_cond_await(&cond_, &(mutex->mutex_), NULL);
#endif
}

INLINE int PThCond::TimedWait(PThMutex* mutex, const struct timespec* abstime)
{
  return -1; // deprecated in Linux port - to re-implement, 
  // create a pTh event loop with a timer event that will simulate
  // the pth_cond_timedwait function

  //  return pth_cond_timedwait(&cond_, &(mutex->mutex_), abstime);
}

INLINE int PThCond::Signal()
{
#ifdef WIN32
  return pthread_cond_signal(&cond_);
#else
    
    // Save the current thread priority then jack it up to the max.
    // This will stop the current thread from suspending when signaling 
    // other threads that they have messages waiting.


    pth_attr_t attr;
    int currentPriority;

    attr = pth_attr_of(pth_self());
    
    pth_attr_get(attr, PTH_ATTR_PRIO, &currentPriority);
    pth_attr_set(attr, PTH_ATTR_PRIO, PTH_PRIO_MAX);

    int rc = pth_cond_notify(&cond_, FALSE);

    pth_attr_set(attr, PTH_ATTR_PRIO, currentPriority);
    pth_attr_destroy(attr);
    return (rc);
#endif
}

INLINE int PThCond::Broadcast()
{
#ifdef WIN32
  return pthread_cond_signal(&cond_);
#else
  return pth_cond_notify(&cond_, TRUE);
#endif
}



