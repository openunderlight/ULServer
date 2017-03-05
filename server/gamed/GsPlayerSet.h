// GsPlayerSet.h  -*- C++ -*-
// $Id: GsPlayerSet.h,v 1.14 1997-10-22 19:22:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// players logged into game server

#ifndef INCLUDED_GsPlayerSet
#define INCLUDED_GsPlayerSet

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"
#include "PThMutex.h"

// class forward declarations

class GsPlayer;
class GsMain;
class GsPlayerList;
class GsPlayerSetImp;

// the class

class GsPlayerSet {

public:

  GsPlayerSet(GsMain* gsmain);
  ~GsPlayerSet();

  bool IsInGame(lyra_id_t playerid) const;
  int NumPlayers() const;
  int MaxPlayers() const;
  int NumLogins() const;
  int NumPlayers(int acct_type) const;

  GsPlayer* GetPlayer(lyra_id_t playerid) const;
  void GetPlayerList(GsPlayerList& plist) const;
  GsPlayer* AllocatePlayer(lyra_id_t playerid);
  void RemovePlayer(GsPlayer* player, bool save = true);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  GsPlayerSet(const GsPlayerSet&);
  //operator=(const GsPlayerSet&);

  GsMain* main_;
  PThMutex lock_;
  GsPlayer* players_;
  int max_players_;
  int total_logins_;
  int max_loggedin_;
  GsPlayerSetImp* imp_; // map from playerid to player object
};

#ifdef USE_INLINE
#include "GsPlayerSet.i"
#endif

#endif /* INCLUDED_GsPlayerSet */
