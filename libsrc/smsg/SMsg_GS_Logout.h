// SMsg_GS_Logout.h  -*- C++ -*-
// $Id: SMsg_GS_Logout.h,v 1.7 1997-10-03 18:30:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server logout

#ifndef INCLUDED_SMsg_GS_Logout
#define INCLUDED_SMsg_GS_Logout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_GS_Logout : public LmMesg {

public:

  SMsg_GS_Logout();
  ~SMsg_GS_Logout();

  void Init(lyra_id_t playerid, int online, int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int Online() const;
  int Status() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetOnline(int online);
  void SetStatus(int status);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int online;
    int status;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GS_Logout.i"
#endif

#endif /* INCLUDED_SMsg_GS_Logout */
