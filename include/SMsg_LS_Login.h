// SMsg_LS_Login.h  -*- C++ -*-
// $Id: SMsg_LS_Login.h,v 1.7 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server: new player login

#ifndef INCLUDED_SMsg_LS_Login
#define INCLUDED_SMsg_LS_Login

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_LS_Login : public LmMesg {

public:

  SMsg_LS_Login();
  ~SMsg_LS_Login();

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
    lyra_id_t playerid;  // playerid of new player
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_LS_Login.i"
#endif

#endif /* INCLUDED_SMsg_LS_Login */
