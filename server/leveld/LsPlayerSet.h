// LsPlayerSet.h  -*- C++ -*-
// $Id: LsPlayerSet.h,v 1.13 1997-09-19 23:06:12-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// players in level

#ifndef INCLUDED_LsPlayerSet
#define INCLUDED_LsPlayerSet

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "../../libsrc/shared/LyraDefs.h"
#include "PThMutex.h"
#include "LsPlayerList.h"

// class forward declarations

class LsPlayer;
class LsMain;
class LsPlayerSetImp;

// the class

class LsPlayerSet {

public:

  LsPlayerSet(LsMain* lsmain);
  ~LsPlayerSet();

  bool IsInLevel(lyra_id_t playerid) const;
  int NumPlayers() const;
  int NumPlayers(int acct_type) const;
  int MaxPlayers() const;
  int NumLogins() const;
  LsPlayer* GetPlayer(lyra_id_t playerid) const;
  void GetPlayerList(LsPlayerList& plist, lyra_id_t roomid = Lyra::ID_UNKNOWN) const;

  LsPlayer* AllocatePlayer(lyra_id_t playerid);
  void RemovePlayer(LsPlayer* player);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsPlayerSet(const LsPlayerSet&);
  //operator=(const LsPlayerSet&);

  LsMain* main_;

  PThMutex lock_;    // object lock

  // LsPlayerList active_;  // player objects in use

  LsPlayerSetImp* imp_;  // player objects in use
  LsPlayerList free_;    // player objects not in use
  int max_players_;      // maximum number of active players allowed

  int logins_;           // total number of players that have logged in
  int max_logins_;       // maximum number of concurrent players

};

#ifdef USE_INLINE
#include "LsPlayerSet.i"
#endif

#endif /* INCLUDED_LsPlayerSet */
