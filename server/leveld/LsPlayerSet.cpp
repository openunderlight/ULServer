// LsPlayerSet.cpp  -*- C++ -*-
// $Id: LsPlayerSet.cpp,v 1.26 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsPlayerSet.h"
#endif

#include <stdio.h>
#include <list>
//#include <hash_map.h>
#include <unordered_map>

#include "LsPlayerSet.h"
#include "LyraDefs.h"
#include "LsMain.h"
#include "LsPlayer.h"
#include "LmLevelDBC.h"
#include "LmLogFile.h"
#include "LsLevelState.h"
#include "LsRoomState.h"
#include "LsPlayerList.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LsPlayerSet.i"
#endif


DECLARE_TheFileName;

////
// LsPlayerSetImp implementation
////

class LsPlayerSetImp :
  public std::unordered_map< lyra_id_t, LsPlayer*, std::hash<lyra_id_t>, std::equal_to<lyra_id_t> > {
  // nothing added
};

////
// Constructor
////

LsPlayerSet::LsPlayerSet(LsMain* lsmain)
  : main_(lsmain),
    imp_(LmNEW(LsPlayerSetImp())),
    logins_(0),
    max_logins_(0),
    max_players_(lsmain->LevelDBC()->MaxPlayers())
{
  DECLARE_TheLineNum;
  lock_.Init();
}

////
// Destructor
////

LsPlayerSet::~LsPlayerSet()
{
  DECLARE_TheLineNum;
  // delete all player objects
  for (LsPlayerList::iterator i = free_.begin(); !(bool)(i == free_.end()); ++i) {
    LmDELETE(*i);
  }
  for (LsPlayerSetImp::iterator k = imp_->begin(); k != imp_->end(); ++k) {
    LsPlayer* player = (*k).second;
    LmDELETE(player);
  }
  LmDELETE(imp_);
}

////
// NumPlayers: return number of players actually in level (not maximum number)
////

int LsPlayerSet::NumPlayers() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // return active_.size();
  return imp_->size();
}

////
// NumPlayers: return # of players of given acct type
////

int LsPlayerSet::NumPlayers(int acct_type) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  int retval = 0;
  for (LsPlayerSetImp::const_iterator i = imp_->begin(); i != imp_->end(); ++i) {
    LsPlayer* player = (*i).second;
    if (player->AccountType() == acct_type) {
      retval++;
    }
  }
  return retval;
}

////
// GetPlayer
////

LsPlayer* LsPlayerSet::GetPlayer(lyra_id_t playerid) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // search the hash table
  LsPlayerSetImp::iterator i = imp_->find(playerid);
  if (i != imp_->end()) {
    return (*i).second;
  }
  return 0;
}

////
// GetPlayerList: copy players into list; if roomid is Lyra::ID_UNKNOWN, copy all players
//   in level; otherwise copy only those players who are in the given room
////

void LsPlayerSet::GetPlayerList(LsPlayerList& plist, lyra_id_t roomid) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  for (LsPlayerSetImp::const_iterator i = imp_->begin(); i != imp_->end(); ++i) {
    LsPlayer* player = (*i).second;
    if ((roomid == Lyra::ID_UNKNOWN) || (player->RoomID() == roomid)) {
      plist.push_back(player);
    }
  }
}

////
// AllocatePlayer: find unused player slot, return pointer to it or 0 if none exist
////

LsPlayer* LsPlayerSet::AllocatePlayer(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // active list already full?
  // if (active_.size() >= max_players_) {
  if (imp_->size() >= max_players_) {
    return 0;
  }
  LsPlayer* player = 0;
  // free list non-empty?
  if (free_.size() > 0) {
    // get from free list
    player = free_.front();
    free_.pop_front();
  }
  else {
    // allocate a new one
    player = LmNEW(LsPlayer());
  }
  // login, initialize player object
  if (player->Login(main_, playerid) < 0) {
    // couldn't log in, put on free list and return 0
    player->Logout();
    free_.push_front(player);
    return 0;
  }
  // add to active table
  logins_++;
  (*imp_)[playerid] = player;
  if (imp_->size() > max_logins_) {
    max_logins_ = imp_->size();
  }
  //  main_->Log()->Debug(_T("LsPlayerSet::AllocatePlayer: %u -> [%p]"), playerid, player);
  return player;
}

////
// RemovePlayer
////

void LsPlayerSet::RemovePlayer(LsPlayer* player)
{
  DEFMETHOD(LsPlayerSet, RemovePlayer);
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  LsPlayerSetImp::iterator i = imp_->find(player->PlayerID());
  if (i != imp_->end()) {
    imp_->erase(i);
    free_.push_front(player);
    // remove player from level, just in case
    main_->LevelState()->RemovePlayer(player->PlayerID(), player->RealtimeID());
  }
  else {
    // not found in active list!
    main_->Log()->Error(_T("%s: player [%p] returned, not found in active set"), method, player);
  }
  // now, remove player from position list of all neighbors
  for (LsPlayerSetImp::iterator k = imp_->begin(); k != imp_->end(); ++k) {
    LsPlayer* neighbor = (*k).second;
    LsUpdateSet* ps = neighbor->UpdateSet();
    LsUpdateSet::iterator j = ps->find(player->PlayerID());
    if (j != ps->end()) {
      //      main_->Log()->Debug(_T("%s: player %d removed from position set of player %d"), method, player->PlayerID(), neighbor->PlayerID());
      LsLastUpdate* last_pos = (LsLastUpdate*)((*j).second);
      LmDELETE(last_pos);
      ps->erase(j);
      }
  }
}



////
// Dump
////

void LsPlayerSet::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LsPlayerSet[%p,%d]: main=[%p] active=%d/%d free=%d logins=%d (%d max)>\n"),
	  this, sizeof(LsPlayerSet),
	  main_, imp_->size(), max_players_, free_.size(), logins_, max_logins_);
  for (LsPlayerSetImp::const_iterator i = imp_->begin(); i != imp_->end(); ++i) {
    ((*i).second)->Dump(f, indent + 1);
  }
}
