// LmLogSyslog.h  -*- C++ -*-
// $Id: LmLogSyslog.h,v 1.1 1997-10-16 18:14:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Lyra log class based on syslog facility

#ifndef INCLUDED_LmLogSyslog
#define INCLUDED_LmLogSyslog

#ifdef __GNUC__
#pragma interface
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmLog.h"

// log class

class LmLogSyslog : public LmLog {

public:

  LmLogSyslog();
  virtual ~LmLogSyslog();

  void Open(const char* log_ident, int log_options, int log_facility);
  void Close();
  void SetLogMask(int mask);

protected:

  virtual void WriteLogEntry(const char* prefix, const char* fmt, va_list args);

private:

  // methods/operations not implemented
  LmLogSyslog(const LmLogSyslog&);
  //operator=(const LmLogSyslog&);

};

#ifdef USE_INLINE
#include "LmLogSyslog.i"
#endif

#endif /* INCLUDED_LmLogSyslog */
