// GsMessageReader.h  -*- C++ -*-
// $Id: GsMessageReader.h,v 1.9 1997-09-16 03:02:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server message reader thread

#ifndef INCLUDED_GsMessageReader
#define INCLUDED_GsMessageReader

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmMessageReader.h"

// class forward declarations

class LmLog;
class GsMain;

// class declarations

class GsMessageReader : public LmMessageReader {

public:

  GsMessageReader(GsMain* gsmain, LmLog* log);
  ~GsMessageReader();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();
  bool HandleError(LmConnection* conn, int errcode, int mtype, int msize);

private:

  GsMain* main_;

};

#ifdef USE_INLINE
#include "GsMessageReader.i"
#endif

#endif /* INCLUDED_GsMessageReader */
