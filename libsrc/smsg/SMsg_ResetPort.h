// SMsg_ResetPort.h  -*- C++ -*-
// $Id: SMsg_ResetPort.h,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// used when the client's locally bound UDP port # is different than the one
// passed at login (usually because of NAT/ICS/etc.)

#ifndef INCLUDED_SMsg_ResetPort
#define INCLUDED_SMsg_ResetPort

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_ResetPort : public LmMesg {

public:


public:

  SMsg_ResetPort();
  ~SMsg_ResetPort();

  void Init(lyra_id_t playerid, int port);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int Port() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetPort(int port);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int port;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ResetPort.i"
#endif

#endif /* INCLUDED_SMsg_ResetPort */
