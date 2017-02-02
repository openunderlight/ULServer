// LmLogNull.h  -*- C++ -*-
// $Id: LmLogNull.h,v 1.1 1997-10-10 00:33:15-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// "null" log (ie. all logging functions are no-ops)

#ifndef INCLUDED_LmLogNull
#define INCLUDED_LmLogNull

#include "LmLog.h"

// the class

class LmLogNull {

public:

  LmLogNull() { }
  ~LmLogNull() { }

  void Log(int level, const char* fmt, ...) { }
  void Warning(int level, const char* fmt, ...) { }
  void Debug(int level, const char* fmt, ...) { }

  void Log(const char* fmt, ...) { }
  void Warning(const char* fmt, ...) { }
  void Debug(const char* fmt, ...) { }
  void Error(const char* fmt, ...) { }
  void Security(int level, const char* fmt, ...) { }

  void AssertionFailed(const char* expr, const char* filename, int linenum) { }

private:

  // methods/operations not implemented
  LmLogNull(const LmLogNull&);
  //operator=(const LmLogNull&);

};

#endif /* INCLUDED_LmLogNull */
