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
  return pth_mutex_init(&mutex_);
#endif
}

INLINE int PThMutex::Lock()
{
#ifdef WIN32
  return pthread_mutex_lock(&mutex_);
#else
  return pth_mutex_acquire(&mutex_, FALSE, NULL);
#endif
}

INLINE int PThMutex::TryLock()
{ // non-blocking version
#ifdef WIN32
  return pthread_mutex_trylock(&mutex_);
#else
  return pth_mutex_acquire(&mutex_, TRUE, NULL);
#endif
}

INLINE int PThMutex::UnLock()
{
#ifdef WIN32
  return pthread_mutex_unlock(&mutex_);
#else
  return pth_mutex_release(&mutex_);
#endif
}
