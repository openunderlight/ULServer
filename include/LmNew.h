// LmNew.h  -*- C++ -*-
// $Id: LmNew.h,v 1.6 1997-08-21 15:43:06-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// new handler, operators new/delete

#ifndef INCLUDED_LmNew
#define INCLUDED_LmNew

#include <stddef.h>
#include "tchar.h"

// class forward declarations

class LmLog;

// class declarations

class LmNew {

public:

  static void InstallNewHandler();
  static void SetLog(LmLog* log);
#ifndef WIN32 // UNIX new handler can be a class method
  static void TheNewHandler();
#endif
  static void* New(size_t size, const TCHAR* file, int line);
  static void LogDelete(void* ptr, const TCHAR* file, int line);
  static void Delete(void* ptr);

};

#ifdef WIN32 // Win32 new handler must have a specific prototype
int __cdecl TheNewHandler(size_t size);
#endif

// if USE_LMNEW defined, use our own global new/delete
#ifdef USE_LMNEW

inline void* operator new(size_t size)
{
  return LmNew::New(size, "unknown", 0);
}

inline void* operator new(size_t size, const TCHAR* file, int line)
{
  return LmNew::New(size, file, line);
}

inline void operator delete(void* ptr)
{
  LmNew::Delete(ptr);
}

// macros that work via CPP __FILE__ and __LINE__ macros
#define LmNEW(x)          (new(__FILE__, __LINE__) x)
#define LmDELETE(x)       { LmNew::LogDelete(x, __FILE__, __LINE__); delete x; }
#define LmDELETEARRAY(x)  { LmNew::LogDelete(x, __FILE__, __LINE__); delete x; } /* not delete[] */

// macros that work via file/function global variables (allowing only an approximate line number, alas)
#define LmNEW2(x)         (new(TheFileName, TheLineNum) x)
#define LmDELETE2(x)      { LmNew::LogDelete(x, TheFileName, TheLineNum); delete x; }
#define LmDELETEARRAY2(x) { LmNew::LogDelete(x, TheFileName, TheLineNum); delete x; } /* not delete[] */

// macro to define TheFileName, TheLineNum
#define DECLARE_TheFileName  static const TCHAR* TheFileName = __FILE__; static int TheLineNum = 0
#define DECLARE_TheLineNum   /* static const int */ TheLineNum = __LINE__

#else /* !USE_LMNEW */

#define LmNEW(x)           new x
#define LmDELETE(x)        delete x
#define LmDELETEARRAY(x)   delete [] x

#define LmNEW2(x)          new x
#define LmDELETE2(x)       delete x
#define LmDELETEARRAY2(x)  delete [] x

// macro to define TheFileName, TheLineNum when USE_LMNEW isn't true
#define DECLARE_TheFileName  /* nothing */
#define DECLARE_TheLineNum   /* nothing */

#endif /* USE_LMNEW */

#endif /* INCLUDED_LmNew */
