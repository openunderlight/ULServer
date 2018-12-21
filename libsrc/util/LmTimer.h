// LmTimer.h  -*- C++ -*-
// $Id: LmTimer.h,v 1.7 1997-10-22 19:21:59-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Microsecond-resolution timer class

#ifndef INCLUDED_LmTimer
#define INCLUDED_LmTimer

#ifdef __GNUC__
#pragma interface
#endif

#include "../../libsrc/shared/LyraDefs.h"

// LmTimer

class LmTimer {

public:

  LmTimer(unsigned long* accumulator = 0);

  void Start();
  void Stop();

  unsigned long MicroSeconds() const;
  unsigned long MilliSeconds() const;

private:

  // not implemented
  LmTimer(const LmTimer&);
  //operator=(const LmTimer&);

  unsigned long* accum_;
  struct timespec start_;
  struct timespec end_;

};

#ifdef USE_INLINE
#include "LmTimer.i"
#endif

#endif /* INCLUDED_LmTimer */
