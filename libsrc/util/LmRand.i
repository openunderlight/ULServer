// LmRand.i  -*- C++ -*-
// $Id: LmRand.i,v 1.2 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE void LmRand::InitSeed(long seed)
{
#ifdef WIN32
  srand(seed);
#else
  srand48(seed);
#endif
}

INLINE int LmRand::Generate(int min, int max)
{
#ifdef WIN32
  return min + (rand() % (max - min + 1));
#else
  return min + (lrand48() % (max - min + 1));
#endif
}

INLINE int LmRand::ChooseOne(int* choices, int num_choices)
{
  return choices[Generate(0, num_choices - 1)];
}
