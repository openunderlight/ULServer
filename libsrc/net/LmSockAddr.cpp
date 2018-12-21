// LmSockAddr.cpp  -*- C++ -*-
// $Id: LmSockAddr.cpp,v 1.5 1997-08-20 01:34:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmSockAddr.h"
#endif

#include <stdio.h>

#include "LmSockAddr.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmSockAddr.i"
#endif

////
// Destructor
////

LmSockAddr::~LmSockAddr()
{
  // empty
}
