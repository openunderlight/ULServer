// SMsg_SendMesg.h  -*- C++ -*-
// $Id: SMsg_SendMesg.h,v 1.1 1997-08-17 16:32:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// to network output thread: send given message out on given connection

#ifndef INCLUDED_SMsg_SendMesg
#define INCLUDED_SMsg_SendMesg

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

class LmConnection;
class LmSrvMesgBuf;

// message class

class SMsg_SendMesg : public LmMesg {

public:

  SMsg_SendMesg();
  ~SMsg_SendMesg();

  void Init(LmConnection* conn, LmSrvMesgBuf* mbuf);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  LmConnection* Connection() const;
  LmSrvMesgBuf* MesgBuf() const;

  //mutators
  void SetConnection(LmConnection* conn);
  void SetMesgBuf(LmSrvMesgBuf* mbuf);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmConnection* conn;
    LmSrvMesgBuf* mbuf;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_SendMesg.i"
#endif

#endif /* INCLUDED_SMsg_SendMesg */
