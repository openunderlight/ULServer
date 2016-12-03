// LsInputDispatch.h  -*- C++ -*-
// $Id: LsInputDispatch.h,v 1.7 1997-07-14 19:25:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server input message dispatcher

#ifndef INCLUDED_LsInputDispatch
#define INCLUDED_LsInputDispatch

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "LmDispatch.h"

// class forward declarations

class LsMain;
class LmSrvMesgBuf;
class LmConnection;
class LmThread;

// the class

class LsInputDispatch : public LmDispatch {

public:

  LsInputDispatch(LsMain* lsmain);
  ~LsInputDispatch();

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmThread* ComputeTarget(LmSrvMesgBuf* mbuf, LmConnection* conn);

private:

  LsMain* main_;

  // target threads
  enum {
    DT_LEVEL = 'L',
    DT_ROOM = 'R'
  };

  void initialize_table();

};

#ifdef USE_INLINE
#include "LsInputDispatch.i"
#endif

#endif /* INCLUDED_LsInputDispatch */
