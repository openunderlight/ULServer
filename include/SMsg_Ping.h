// SMsg_Ping.h  -*- C++ -*-
// $Id: SMsg_Ping.h,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server ping message

#ifndef INCLUDED_SMsg_Ping
#define INCLUDED_SMsg_Ping

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_Ping : public LmMesg {

public:

  enum {
    // message type (initial, or response)
    PING = 1,
    PONG
  };

public:

  SMsg_Ping();
  ~SMsg_Ping();

  void Init(int nonce, int type);

  void InitPing(int nonce);
  void InitPong(int nonce);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Nonce() const;
  int PingType() const;

  // mutators
  void SetNonce(int nonce);
  void SetPingType(int type);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int nonce;
    int type;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_Ping.i"
#endif

#endif /* INCLUDED_SMsg_Ping */
