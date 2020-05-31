// PThMutex.i  -*- C++ -*-
// $Id: PThMutex.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// optionally inlined methods

INLINE PThMutex::PThMutex()
{
}

INLINE PThMutex::~PThMutex()
{
#ifdef WIN32
  pthread_mutex_destroy(&mutex_);
#endif
}

INLINE int PThMutex::Init()
{ 
#ifdef WIN32
  return pthread_mutex_init(&mutex_, 0);
#else
  mutex_ = st_mutex_new();
  return 0;
#endif
}

INLINE int PThMutex::Lock()
{
#ifdef WIN32
  return pthread_mutex_lock(&mutex_);
#else
  int rv = st_mutex_lock(mutex_);
  if(rv == EDEADLK)
    return 0;
  return rv;
#endif
}

INLINE int PThMutex::TryLock()
{ // non-blocking version
#ifdef WIN32
  return pthread_mutex_trylock(&mutex_);
#else
  return st_mutex_trylock(mutex_);
#endif
}

INLINE int PThMutex::UnLock()
{
#ifdef WIN32
  return pthread_mutex_unlock(&mutex_);
#else
  return st_mutex_unlock(mutex_);
#endif
}
