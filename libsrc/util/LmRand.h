// LmRand.h  -*- C++ -*-
// $Id: LmRand.h,v 1.2 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// random number generator class (MT-safe)

#ifndef INCLUDED_LmRand
#define INCLUDED_LmRand

#include <stdlib.h>
#include <time.h>

#include "LyraDefs.h"

// the class

class LmRand {

public:

  static void InitSeed(long seed = time(NULL));

  static int Generate(int min, int max);
  static int ChooseOne(int* choices, int num_choices);

private:

  // no constructor (all static methods)
  LmRand();

};

#ifdef USE_INLINE
#include "LmRand.i"
#endif

#endif /* INCLUDED_LmRand */
