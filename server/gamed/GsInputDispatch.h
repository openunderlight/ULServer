// GsInputDispatch.h  -*- C++ -*-
// $Id: GsInputDispatch.h,v 1.9 1997-07-14 19:25:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server input message dispatcher

#ifndef INCLUDED_GsInputDispatch
#define INCLUDED_GsInputDispatch

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "LmDispatch.h"

// class forward declarations

class GsMain;
class LmSrvMesgBuf;
class LmConnection;
class LmThread;

// the class

class GsInputDispatch : public LmDispatch {

public:

  GsInputDispatch(GsMain* gsmain);
  ~GsInputDispatch();

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmThread* ComputeTarget(LmSrvMesgBuf* mbuf, LmConnection* conn);

private:

  GsMain* main_;

  // targets
  enum {
    DT_GAME = 'G',    // to the game thread
    DT_PLAYER = 'P',  // to the player thread, from a client
    DT_FORWARD = 'F',  // to the message forwarding thread
	DT_POSITION = 'W', // to the position thread, from a client
  };

  void initialize_table();

};

#ifdef USE_INLINE
#include "GsInputDispatch.i"
#endif

#endif /* INCLUDED_GsInputDispatch */
