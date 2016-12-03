// LmAssert.h  -*- C++ -*-
// $Id: LmAssert.h,v 1.6 1997-06-17 23:14:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Assert macro / function

#ifndef INCLUDED_LmAssert
#define INCLUDED_LmAssert

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "tchar.h"

// forward declarations

class LmLog;

// assert function

struct LmAssert {
  static void Failed(LmLog* log, const TCHAR* expr, const TCHAR* filename, int linenum);
  static void Failed(FILE* file, const TCHAR* expr, const TCHAR* filename, int linenum);
};

#endif /* INCLUDED_LmAssert */

// Since we presume that this file may be included multiple times, for
// varying definitions of NDEBUG, it is not protected by include
// guards (similar to the ANSI C standard assert.h)

#undef Assert

#ifdef NDEBUG
#define Assert(log, expr) ((void)0)
#else
#define Assert(log, expr) (void)((expr) || (LmAssert::Failed(log, #expr, __FILE__, __LINE__), 0))
#endif  /* NDEBUG */
