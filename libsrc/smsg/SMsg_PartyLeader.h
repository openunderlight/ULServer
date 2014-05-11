// SMsg_PartyLeader.h  -*- C++ -*-
// $Id: SMsg_PartyLeader.h,v 1.1 1997-11-10 18:34:50-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// party leader time message

#ifndef INCLUDED_SMsg_PartyLeader
#define INCLUDED_SMsg_PartyLeader

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_PartyLeader : public LmMesg {

public:

  SMsg_PartyLeader();
  ~SMsg_PartyLeader();

  void Init(lyra_id_t playerid, int leader_time);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int LeaderTime() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetLeaderTime(int leader_time);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int leader_time;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_PartyLeader.i"
#endif

#endif /* INCLUDED_SMsg_PartyLeader */
