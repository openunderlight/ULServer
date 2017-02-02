// PTh.h  -*- C++ -*-
// $Id: PTh.i,v 1.5 1997-08-01 16:45:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE bool PTh::IsRunning() const
{
  return running_;
}

INLINE bool PTh::IsActive() const
{
#ifdef WIN32
// *** NOTE: pthread_kill not implemented yet
// This causes Cancel to be called on ALL threads
// when the thread pool is deleted.
  return true;
  //return (pthread_kill(thread_, 0) == 0);
#else
  return pth_raise(thread_, 0);
#endif
}

INLINE void PTh::DoneRunning()
{
  running_ = false;
}

#ifdef WIN32
INLINE pthread_t PTh::Thread() const
#else
INLINE pth_t PTh::Thread() const
#endif
{
  return thread_;
}

INLINE int PTh::Cancel()
{
#ifdef WIN32
  return pthread_cancel(thread_);
#else
  return pth_cancel(thread_);
#endif
}

INLINE void PTh::Exit(void* status)
{
#ifdef WIN32
  pthread_exit(status);
#else
  pth_exit(status);
#endif
}

INLINE int PTh::Join(void** status)
{
#ifdef WIN32
  return pthread_join(thread_, status);
#else
  return pth_join(thread_, status);
#endif
}


#ifdef WIN32
INLINE pthread_t PTh::Self()
{
  return pthread_self();
}
#else
INLINE pth_t PTh::Self()
{
  return pth_self();
}
#endif

#ifdef WIN32
INLINE void PTh::YieldSlice()
{
  Sleep(0);
}
#else
INLINE void PTh::Yield()
{
  pth_yield(NULL);
}
#endif





