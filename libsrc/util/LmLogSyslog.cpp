// LmLogSyslog.cpp  -*- C++ -*-
// $Id: LmLogSyslog.cpp,v 1.2 1997-10-20 17:54:25-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// LmLogSyslog Implementation

#ifdef __GNUC__
#pragma implementation "LmLogSyslog.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmLogSyslog.h"

#ifndef USE_INLINE
#include "LmLogSyslog.i"
#endif

////
// constructor
////

LmLogSyslog::LmLogSyslog()
{
  // empty
}

////
// destructor
////

LmLogSyslog::~LmLogSyslog()
{
  Close();
}

////
// Open
////

void LmLogSyslog::Open(const char* log_ident, int log_options, int log_facility)
{
  openlog(log_ident, log_options, log_facility);
  SetLogOpened(true);
}

////
// Close
////

void LmLogSyslog::Close()
{
  closelog();
  SetLogOpened(false);
}

////
// SetLogMask
////

void LmLogSyslog::SetLogMask(int mask)
{
  setlogmask(mask);
}

////
// WriteLogEntry: actually write entry to syslog
////

void LmLogSyslog::WriteLogEntry(const char* prefix, const char* fmt, va_list args)
{
  // Note: allocate stuff on the heap, since we can't extend the stack space by huge increments
  // without possibly exceeding a thread's stack area, and that is a very bad thing

  // since syslog() doesn't take varargs, we must put it into a string first
  char* out_tmp = new char[1024];
  int len =_vstprintf(out_tmp, fmt, args);
  // and then add the prefix (can't use the original string, doh!)
  char* out = new char[len +_tcslen(prefix) + 10];
 _stprintf(out, "%s%s", prefix, out_tmp); // prefix has space already
  // use LOG_INFO for everything, since we have our own hierarchy of log message types
  syslog(LOG_INFO, out); 
  // clean up
  delete out;
  delete out_tmp;
}
