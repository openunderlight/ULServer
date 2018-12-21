// SMsg_GetLevelPlayers.h  -*- C++ -*-
// $Id: SMsg_GetLevelPlayers.h,v 1.2 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_SMsg_GetLevelPlayers
#define INCLUDED_SMsg_GetLevelPlayers

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_GetLevelPlayers : public LmMesg {

public:

  SMsg_GetLevelPlayers();
  ~SMsg_GetLevelPlayers();

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
#include "SMsg_GetLevelPlayers.i"
#endif

#endif /* INCLUDED_SMsg_GetLevelPlayers */
