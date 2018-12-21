// SMsg_GS_Login.h  -*- C++ -*-
// $Id: SMsg_GS_Login.h,v 1.1 1997-08-13 15:39:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server logout

#ifndef INCLUDED_SMsg_GS_Login
#define INCLUDED_SMsg_GS_Login

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_GS_Login : public LmMesg {

public:

  SMsg_GS_Login();
  ~SMsg_GS_Login();

  void Init(lyra_id_t playerid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  lyra_id_t PlayerID() const;
  void SetPlayerID(lyra_id_t playerid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GS_Login.i"
#endif

#endif /* INCLUDED_SMsg_GS_Login */
