// LmDispatch.h  -*- C++ -*-
// $Id: LmDispatch.h,v 1.11 1997-07-30 23:09:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message dispatcher base class

#ifndef INCLUDED_LmDispatch
#define INCLUDED_LmDispatch

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"

// class forward declarations

class LmSrvMesgBuf;
class LmMesgBufPool;
class LmMesg;
class LmConnection;
class LmConnectionList;
class LmThread;
class LmDispatchTable;

// the class

class LmDispatch {

public:

  LmDispatch(LmMesgBufPool* bufpool = 0);
  virtual ~LmDispatch();

  int DispatchMessage(LmSrvMesgBuf* mbuf, LmConnection* conn);
  int DispatchMessage(LmMesg* msg, LmConnection* conn);
  int DispatchMessage(LmSrvMesgBuf* mbuf, LmConnectionList& conn_list);
  int DispatchMessage(LmMesg* msg, LmConnectionList& conn_list);

  virtual void Dump(FILE* f, int indent = 0) const;

protected:

  virtual LmThread* ComputeTarget(LmSrvMesgBuf* mbuf, LmConnection* conn) = 0;

  void AddMapping(int mesgtype, int target);
  void AddMappings(const int mesgtypes[], int num_types, int target);
  int GetTarget(int mesgtype) const;

private:

  // not implemented
  LmDispatch(const LmDispatch&);
  //operator=(const LmDispatch&);

  LmDispatchTable* dt_;
  LmMesgBufPool* bufpool_;

};

#ifdef USE_INLINE
#include "LmDispatch.i"
#endif

#endif /* INCLUDED_LmDispatch */
