// LmTimer.i  -*- C++ -*-
// $Id: LmTimer.i,v 1.4 1997-10-22 19:21:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods

#ifdef WIN32
#include "mmsystem.h"
#endif

#if 0
# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;}
#endif
#include <sys/time.h>

INLINE LmTimer::LmTimer(unsigned long* accum)
  : accum_(accum)
{
  // empty
}

INLINE unsigned long LmTimer::MicroSeconds() const
{
  return (((end_.tv_sec * 1000000000) + end_.tv_nsec) - (start_.tv_sec * 1000000000 + start_.tv_nsec));
}

INLINE unsigned long LmTimer::MilliSeconds() const
{
  return MicroSeconds() / 1000;
}

INLINE void LmTimer::Start()
{
#ifdef WIN32
  DWORD msecs = timeGetTime();
  start_.tv_sec = msecs/1000;
  start_.tv_nsec = (msecs%1000)*1000000;
#else
  timeval t;
  gettimeofday(&t, NULL);
  TIMEVAL_TO_TIMESPEC(&t, &start_);
#endif
}

INLINE void LmTimer::Stop()
{
#ifdef WIN32
  DWORD msecs = timeGetTime();
  end_.tv_sec = msecs/1000;
  end_.tv_nsec = (msecs%1000)*1000000;
#else
  timeval t;
  gettimeofday(&t, NULL);
  TIMEVAL_TO_TIMESPEC(&t, &end_);
#endif
  if (accum_) {
    *accum_ += MilliSeconds();
  }
}
