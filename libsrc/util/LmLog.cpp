// LmLog.cpp  -*- C++ -*-
// $Id: LmLog.cpp,v 1.14 1998-02-12 15:33:26-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// LmLog implementation

#ifdef __GNUC__
#pragma implementation "LmLog.h"
#endif

#include <stdarg.h>
#include <stdio.h>

#include "LyraDefs.h"
#include "LmLog.h"

#ifndef USE_INLINE
#include "LmLog.i"
#endif

////
// Constructor
////

LmLog::LmLog()
  : log_level_(0),
    log_opened_(false)
{
  // empty
}

////
// Destructor
////

LmLog::~LmLog()
{
  // empty
}

////
// Log: write a standard log entry
////

#if 0
void LmLog::Log(int level, const TCHAR *fmt, ...)
{
  // if the message's level is too high, or if the log is not opened, return immediately
  if ((level > LogLevel()) || !LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry("LOG ", fmt, args);
  va_end(args);
}
#endif

void LmLog::Log(const TCHAR *fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("LOG "), fmt, args);
  va_end(args);
}

////
// Warning: write a "warning" log entry
////

#if 0
void LmLog::Warning(int level, const TCHAR *fmt, ...)
{
  // if the message's level is too high, or if the log is not opened, return immediately
  if ((level > LogLevel()) || !LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("WARN "), fmt, args);
  va_end(args);
}
#endif

void LmLog::Warning(const TCHAR *fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("WARN "), fmt, args);
  va_end(args);
}

////
// Debug: write a "debug" log entry
////

#if 0
void LmLog::Debug(int level, const TCHAR *fmt, ...)
{
  // if the message's level is too high, or if the log is not opened, return immediately
  if ((level > LogLevel()) || !LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("DEBUG "), fmt, args);
  va_end(args);
}
#endif

void LmLog::Debug(const TCHAR *fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("DEBUG "), fmt, args);
  va_end(args);
}

////
// Error: write an "error" log entry
////

void LmLog::Error(const TCHAR *fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("ERR "), fmt, args);
  va_end(args);
}

////
// Notice: write a "notice" log entry
////

void LmLog::Notice(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("NOTE "), fmt, args);
  va_end(args);
}

////
// Info: write an "info" log entry
////

void LmLog::Info(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("INFO "), fmt, args);
  va_end(args);
}

////
// Critical: write a "critical" log entry
////

void LmLog::Critical(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("CRIT "), fmt, args);
  va_end(args);
}

////
// Alert: write an "alert" log entry
////

void LmLog::Alert(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("ALERT "), fmt, args);
  va_end(args);
}

////
// Emergency: write an "emergency" log entry
////

void LmLog::Emergency(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("EMERG "), fmt, args);
  va_end(args);
}

////
// Security: write a "security violation" entry (positive == violation, negative == "log message")
////

void LmLog::Security(int level, const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // security header doubles as log header, if negative
  TCHAR hdr[10];
  if (level < 0) {
   _stprintf(hdr, _T("LOG%d "), -level);
  }
  else {
   _stprintf(hdr, _T("SEC%d "), level);
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(hdr, fmt, args);
  va_end(args);
}

////
// Speech: write a "speech" entry
////

void LmLog::Speech(const TCHAR* fmt, ...)
{
  // if the log is not opened, return immediately
  if (!LogOpened()) {
    return;
  }

  // get the arguments and print them to the log
  va_list args;
  va_start(args, fmt);
  WriteLogEntry(_T("SPEECH "), fmt, args);
  va_end(args);
}

////
// AssertionFailed: emulate assert() output
////

void LmLog::AssertionFailed(const TCHAR* expr, const TCHAR* filename, int linenum)
{
  Critical(_T("Assertion failed: %s in file %s, line %d"), expr, filename, linenum);
}
