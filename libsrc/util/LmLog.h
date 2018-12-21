// LmLog.h  -*- C++ -*-
// $Id: LmLog.h,v 1.19 1998-02-12 15:33:26-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Lyra Log class -- abstract base class

#ifndef INCLUDED_LmLog
#define INCLUDED_LmLog

#include <stdarg.h>

#include "LyraDefs.h"

// the class

class LmLog {

public:

  LmLog();
  virtual ~LmLog();

  void SetLogLevel(int level);
  int LogLevel() const;
  bool LogOpened() const;

  virtual void FlushLog();

  // printf-like output functions that check the loglevel
  // virtual void Log(int level, const TCHAR* fmt, ...);
  // virtual void Warning(int level, const TCHAR* fmt, ...);
  // virtual void Debug(int level, const TCHAR* fmt, ...);

  // printf-like output functions that always produce log entries
  virtual void Log(const TCHAR* fmt, ...);
  virtual void Warning(const TCHAR* fmt, ...);
  virtual void Debug(const TCHAR* fmt, ...);
  virtual void Error(const TCHAR* fmt, ...);

  virtual void Notice(const TCHAR* fmt, ...);
  virtual void Info(const TCHAR* fmt, ...);
  virtual void Critical(const TCHAR* fmt, ...);
  virtual void Alert(const TCHAR* fmt, ...);
  virtual void Emergency(const TCHAR* fmt, ...);

  // lyra-specific log needs
  virtual void Security(int level, const TCHAR* fmt, ...);
  virtual void Speech(const TCHAR* fmt, ...);

  // print "assertion failed" message
  virtual void AssertionFailed(const TCHAR* expr, const TCHAR* filename, int linenum);

protected:

  void SetLogOpened(bool opened);

  // method that actually writes to the log; pure virtual
  virtual void WriteLogEntry(const TCHAR* prefix, const TCHAR* fmt, va_list args) = 0;

private:

  // methods/operations not implemented
  LmLog(const LmLog&);
  //operator=(const LmLog&);

  int log_level_;
  bool log_opened_;

};

#ifdef USE_INLINE
#include "LmLog.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmLog */
