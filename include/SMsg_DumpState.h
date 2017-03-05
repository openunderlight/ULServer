// SMsg_DumpState.h  -*- C++ -*-
// $Id: SMsg_DumpState.h,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// tell servers to dump their state to the logs

#ifndef INCLUDED_SMsg_DumpState
#define INCLUDED_SMsg_DumpState

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_DumpState : public LmMesg {

public:

  SMsg_DumpState();
  ~SMsg_DumpState();

  void Init(int type);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int DumpStateType() const;

  // mutators
  void SetDumpStateType(int type);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int type;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_DumpState.i"
#endif

#endif /* INCLUDED_SMsg_DumpState */
