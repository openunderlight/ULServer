// SMsg_GS_Logout.h  -*- C++ -*-
// $Id: SMsg_GS_FakeLogout.h,v 1.7 1997-10-03 18:30:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// game server logout

#ifndef INCLUDED_SMsg_GS_FakeLogout
#define INCLUDED_SMsg_GS_FakeLogout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_GS_FakeLogout : public LmMesg {

public:

  SMsg_GS_FakeLogout();
  ~SMsg_GS_FakeLogout();

  void Init(lyra_id_t playerid, int gamed);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int OriginatingGamed() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetOriginatingGamed(int gamed);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int gamed;
 } data_;

};

#ifdef USE_INLINE
#include "SMsg_GS_FakeLogout.i"
#endif

#endif /* INCLUDED_SMsg_GS_Logout */
