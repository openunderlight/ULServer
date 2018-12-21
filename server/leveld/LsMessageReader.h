// LsMessageReader.h  -*- C++ -*-
// $Id: LsMessageReader.h,v 1.9 1997-09-16 03:03:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server message reader thread

#ifndef INCLUDED_LsMessageReader
#define INCLUDED_LsMessageReader

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmMessageReader.h"

// class forward declarations

class LsMain;
class LmLog;

// class declarations

class LsMessageReader : public LmMessageReader {

public:

  LsMessageReader(LsMain* lsmain, LmLog* log);
  ~LsMessageReader();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();
  bool HandleError(LmConnection* conn, int errcode, int mtype, int msize);

private:

  LsMain* main_;

};

#ifdef USE_INLINE
#include "LsMessageReader.i"
#endif

#endif /* INCLUDED_LsMessageReader */
