// LmPlayerNameMap.h  -*- C++ -*-
// $Id: LmPlayerNameMap.h,v 1.4 1997-11-06 18:20:59-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// map between player names/ids

#ifndef INCLUDED_LmPlayerNameMap
#define INCLUDED_LmPlayerNameMap

#include <stdio.h>

<<<<<<< HEAD
#include "../../libsrc/shared/LyraDefs.h"
=======
#include "LyraDefs.h"
>>>>>>> parent of 07f5efa... include path update
#include "PThMutex.h"

// class forward declarations

struct LmPlayerNameMapImp;
class LmPlayerDBC;

// the class

class LmPlayerNameMap {

public:

  LmPlayerNameMap(LmPlayerDBC* playerdb);
  ~LmPlayerNameMap();

  // fetch player name/id given other
  lyra_id_t PlayerID(const TCHAR* playername);
  const TCHAR* PlayerName(lyra_id_t playerid);

  void AddMapping(lyra_id_t playerid, const TCHAR* playername);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmPlayerNameMap(const LmPlayerNameMap&);
  //operator=(const LmPlayerNameMap&);

  lyra_id_t lookup_playerid(TCHAR* playername);
  const TCHAR* lookup_playername(lyra_id_t playerid);
  void add_mapping(lyra_id_t playerid, const TCHAR* playername);

  PThMutex lock_;
  LmPlayerDBC* playerdb_;
  LmPlayerNameMapImp* imp_;  // private implementation
  TCHAR tmp[80];              // temporary string
};

#ifdef USE_INLINE
#include "LmPlayerNameMap.i"
#endif

#endif /* INCLUDED_LmPlayerNameMap */
