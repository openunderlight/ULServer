// SMsg_RecvMesg.h  -*- C++ -*-
// $Id: SMsg_RecvMesg.h,v 1.1 1997-08-12 14:38:54-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// to message thread: recv message from given connection

#ifndef INCLUDED_SMsg_RecvMesg
#define INCLUDED_SMsg_RecvMesg

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

class LmConnection;

// message class

class SMsg_RecvMesg : public LmMesg {

public:

  SMsg_RecvMesg();
  ~SMsg_RecvMesg();

  void Init(LmConnection* conn);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  LmConnection* Connection() const;
  void SetConnection(LmConnection* conn);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmConnection* conn;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_RecvMesg.i"
#endif

#endif /* INCLUDED_SMsg_RecvMesg */
