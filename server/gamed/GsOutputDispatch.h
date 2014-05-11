// GsOutputDispatch.h  -*- C++ -*-
// $Id: GsOutputDispatch.h,v 1.9 1997-08-25 17:11:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// output message dispatcher

#ifndef INCLUDED_GsOutputDispatch
#define INCLUDED_GsOutputDispatch

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "LmDispatch.h"

// class forward declarations

class GsMain;
class LmMesg;

// the class

class GsOutputDispatch : public LmDispatch {

public:

  GsOutputDispatch(GsMain* gsmain);
  ~GsOutputDispatch();

  int SendMessage(LmMesg* msg, LmConnection* conn);
  int SendMessage(LmSrvMesgBuf* mbuf, LmConnection* conn);

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmThread* ComputeTarget(LmSrvMesgBuf* mbuf, LmConnection* conn);

private:

  GsMain* main_;

};

#ifdef USE_INLINE
#include "GsOutputDispatch.i"
#endif

#endif /* INCLUDED_GsOutputDispatch */
