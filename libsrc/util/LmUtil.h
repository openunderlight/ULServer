// LmUtil.h  -*- C++ -*-
// $Id: LmUtil.h,v 1.13 1997-11-04 23:03:42-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Utility Functions

#ifndef INCLUDED_LmUtil
#define INCLUDED_LmUtil

#ifdef __GNUC__
#pragma interface
#endif

#include "../../libsrc/shared/LyraDefs.h"

#ifdef WIN32
#include <time.h>
#endif

// utility function "struct"

struct LmUtil {

  static void CloseAllDescriptors();
  static void NullStdDescriptors();

  static int DaemonStart();

  static void BlockAllSignals();
  static void UnBlockAllSignals();

  static time_t TimeSince(time_t t);
  static time_t TimeUntil(time_t t);
  static int DayNumber(time_t t);

  static void SendMail(const TCHAR* source, const TCHAR* target, const TCHAR* subject, const TCHAR* msg_fmt, ...);

  static double GetCPULoad(unsigned short* frac_value = 0);

  // these don't work
  static char** InitProcTitle(int argc, char** argv, char** envp);
  static void SetProcTitle(const char* fmt, ...);

};

#ifdef USE_INLINE
#include "LmUtil.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmUtil */
