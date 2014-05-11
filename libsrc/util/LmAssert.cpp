// LmAssert.cpp  -*- C++ -*-
// $Id: LmAssert.cpp,v 1.5 1997-06-17 23:14:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Assert function

#ifdef __GNUC__
#pragma implementation "LmAssert.h"
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "LmAssert.h"
#include "LmLog.h"

////
// Failed: log "assertion failed" message to LmLog object
////

void LmAssert::Failed(LmLog* log, const TCHAR* expr, const TCHAR* filename, int linenum)
{
  if (log) {
    // print to log file, if it's not null
    log->AssertionFailed(expr, filename, linenum);
  }
  else {
    // try standard assert behavior in this case (print to stderr)
   _ftprintf(stderr, _T("Assertion failed: %s in file %s, line %d\n"), expr, filename, linenum);
  }
  // outta here, in a bad way (will generate SIGABRT signal)
  raise(SIGABRT);
}

////
// Failed: print "assertion failed" message to given FILE stream
////

void LmAssert::Failed(FILE* file, const TCHAR* expr, const TCHAR* filename, int linenum)
{
 _ftprintf(file, _T("Assertion failed: %s in file %s, line %d\n"), expr, filename, linenum);
  // outta here, in a bad way (will generate SIGABRT signal)
  raise(SIGABRT);
}
