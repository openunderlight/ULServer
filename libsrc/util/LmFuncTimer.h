// LmFuncTimer.h  -*- C++ -*-
// $Id: LmFuncTimer.h,v 1.1 1997-10-20 17:54:25-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// function timer class

#ifndef INCLUDED_LmFuncTimer
#define INCLUDED_LmFuncTimer

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "LmTimer.h"

// LmFuncTimer

class LmFuncTimer {

public:

  LmFuncTimer(int& num_calls, unsigned long& total_time, unsigned long& last_time);
  ~LmFuncTimer();

private:

  // not implemented
  LmFuncTimer(const LmFuncTimer&);
  //operator=(const LmFuncTimer&);

  unsigned long& total_time_;
  unsigned long& last_time_;

  LmTimer timer_;

};

#ifdef USE_INLINE
#include "LmFuncTimer.i"
#endif

#endif /* INCLUDED_LmFuncTimer */
