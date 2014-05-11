// LmLogFile.cpp  -*- C++ -*-
// $Id: LmLogFile.cpp,v 1.5 1998-02-12 15:33:26-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// LmLogFile Implementation

#ifdef __GNUC__
#pragma implementation "LmLogFile.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#endif

#include "LyraDefs.h"
#include "LmLogFile.h"
#include "LmLocker.h"

#include "PTh.h"

#ifndef USE_INLINE
#include "LmLogFile.i"
#endif

////
// constructor
////

LmLogFile::LmLogFile()
  : fp_(0),
    instance_(0),
    pid_(0)
{
  // zero out strings
  memset(progname_, 0, sizeof(progname_));
  memset(logname_, 0, sizeof(logname_));
  //memset(hostname_, 0, sizeof(hostname_));
  // get hostname
  // REMOVED due to conflicts between unistd.h and pth.h
#ifdef _UNICODE
  //char sbcs[64];
  //gethostname(sbcs, sizeof(sbcs));
  //int len = strlen(sbcs);
  //mbstowcs(hostname_, sbcs, len);
#else
  //gethostname(hostname_, sizeof(hostname_));
#endif
  // init mutex
  lock_.Init();
}

////
// destructor
////

LmLogFile::~LmLogFile()
{
  Close();
}

////
// Init: initialize log file parameters
////

void LmLogFile::Init(const TCHAR* prefix, const TCHAR* suffix, int instance, pid_t pid)
{
  LmLocker mon(lock_); // lock object during method duration

  instance_ = instance;
  pid_ = pid;

  // determine real suffix, program name
  TCHAR suf[32];
  if (_tcslen(suffix) > 0) {
   _stprintf(suf, _T("_%s"), suffix);
  }
  else {
   _tcsnccpy(suf, _T(""), sizeof(suf));
  }
  // determine program name
 _stprintf(progname_, _T("%s%s"), prefix, suf);
  TRUNC(progname_, sizeof(progname_));
  // determine logfile name
  if ((instance_ != 0) && (pid_ != 0)) {
    // instance and pid in filename
   _stprintf(logname_, _T("%s_%u:%lu%s.log"), prefix, instance_, pid_, suf);
  }
  else if (instance_ != 0) {
    // instance, but no pid in filename
   _stprintf(logname_, _T("%s_%u%s.log"), prefix, instance_, suf);
  }
  else if (pid_ != 0) {
    // pid, but no instance in filename
   _stprintf(logname_, _T("%s_%lu%s.log"), prefix, pid_, suf);
  }
  else {
    // no pid or instance in filename
   _stprintf(logname_, _T("%s%s.log"), prefix, suf);
  }
  TRUNC(logname_, sizeof(logname_));
}

////
// Stream: return pointer to FILE* stream
////

FILE* LmLogFile::Stream()
{
  LmLocker mon(lock_); // lock object during method duration
  return fp_; // dangerous: pointer may not be valid after return!
}

////
// UseStream: assign stream used by log
////

void LmLogFile::UseStream(FILE* stream)
{
  LmLocker mon(lock_); // lock object during method duration
  fp_ = stream;
}

////
// Open: open log file in given directory; return 0 if successful, -1 if not
////

int LmLogFile::Open(const TCHAR* directory)
{
  // close, in case log is already open (Close() locks the mutex, so don't lock it before calling)
  Close();

  LmLocker mon(lock_); // lock object during method duration
  TCHAR fname[FILENAME_MAX];
  // determine filename
#ifdef WIN32
 _stprintf(fname, _T("%s\\%s"), directory, logname_);
#else
 _stprintf(fname, _T("%s/%s"), directory, logname_);
#endif
  
  // open the file in append mode
  fp_ =_tfopen(fname, _T("a"));
  if (fp_ == 0) {
    return -1;
  }
  // print startup message to log
  write_header(_T("LOG log opened\n"), true);
  SetLogOpened(true);
  return 0;
}

////
// Close: close log, with optional log closing message
////

void LmLogFile::Close(bool verbose)
{
  LmLocker mon(lock_); // lock object during method duration
  // opened yet?
  if (fp_) {
    if (verbose) {
      write_header(_T("LOG log closed\n"), true);
    }
    fclose(fp_);
    fp_ = 0;
    SetLogOpened(false);
  }
}

////
// FlushLog: write any buffered output
////

void LmLogFile::FlushLog()
{
  LmLocker mon(lock_); // lock object during method duration
  flush_log();
}

////
// WriteLogEntry: actually write entry to FILE stream, and flush it
////

void LmLogFile::WriteLogEntry(const TCHAR* prefix, const TCHAR* fmt, va_list args)
{
  LmLocker mon(lock_); // lock object during method duration
  // sanity check
  if (!fp_) {
    return;
  }
  write_header(prefix);
 _vftprintf(fp_, fmt, args);
  // add newline
 _ftprintf(fp_, _T("\n"));
  flush_log();
}

////
// write_header: write standard log prefix string to logfile, with optional string;
//   flush log if indicated
////

void LmLogFile::write_header(const TCHAR* str, bool flush)
{
  // get time
   TCHAR timestr[80];
  //  struct timespec ts;
  time_t curtime;
  time(&curtime);
  struct tm* stm;
  stm = localtime(&curtime);
 _tcsftime(timestr, sizeof(timestr), _T("%y/%m/%d %H:%M:%S"), stm);
  //    int msecs = ts.tv_nsec / 1000000;
  //_stprintf(timestr, _T("%s.%3.3d"), tmpstr, msecs);
  //_stprintf(timestr, _T("%s"),_tctime(&curtime));
  // write header
  if (instance_ != 0) {
	 // Thread ID removed due to problems with gcc 3.0
//   _ftprintf(fp_, _T("%s[%u] T%03u %s %s"), progname_, instance_, PTh::Self(), timestr, str);
   _ftprintf(fp_, _T("%s[%u] %s %s"), progname_, instance_,  timestr, str);

  }
  else {
//   _ftprintf(fp_, _T("%s T%03u %s %s"), progname_, PTh::Self(), timestr, str);
   _ftprintf(fp_, _T("%s %s %s"), progname_, timestr, str);

  }
  // flush
  if (flush) {
    flush_log();
  }
}

////
// flush_log: non-locking
////

void LmLogFile::flush_log()
{
  if (fp_) {
    fflush(fp_);
  }
}
