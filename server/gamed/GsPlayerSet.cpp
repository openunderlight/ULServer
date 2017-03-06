// GsPlayerSet.cpp  -*- C++ -*-
// $Id: GsPlayerSet.cpp,v 1.25 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsPlayerSet.h"
#endif

#include <stdio.h>
#include <list>
//#include <hash_map.h>
#include <unordered_map>
#include "GsPlayerSet.h"
#include "LyraDefs.h"
#include "GsMain.h"
#include "GsPlayer.h"
#include "PThMutex.h"
#include "LmLevelDBC.h"
#include "LmLogFile.h"
#include "GsPlayerList.h"
#include "LmLocker.h"
#include "SMsg_All.h"
#include "GsUtil.h"

#ifndef USE_INLINE
#include "GsPlayerSet.i"
#endif

DECLARE_TheFileName;

////
// GsPlayerSetImp implementation
////

class GsPlayerSetImp :
public std::unordered_map< lyra_id_t, GsPlayer*, std::hash<lyra_id_t>, std::equal_to<lyra_id_t> > {
  // nothing added
};

////
// Constructor
////

GsPlayerSet::GsPlayerSet(GsMain* gsmain)
  : main_(gsmain),
    imp_(LmNEW(GsPlayerSetImp())),
    max_players_(gsmain->MaxPlayers() + GsMain::EXTRA_PLAYERS),
    total_logins_(0),
    max_loggedin_(0)
{
  DECLARE_TheLineNum;
  // allocate array of GsPlayer objects
  players_ = LmNEW(GsPlayer[max_players_]);
  for (int i = 0; i < max_players_; ++i) {
    players_[i].SetMain(gsmain);
  }
  lock_.Init();
}

////
// Destructor
////

GsPlayerSet::~GsPlayerSet()
{
  DECLARE_TheLineNum;
  LmDELETE(imp_);
  LmDELETEARRAY(players_);
}

////
// NumPlayers: return number of players actually in game (not maximum number)
////

int GsPlayerSet::NumPlayers() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return imp_->size();
}

////
// NumPlayers - return number of players of given account type
////

int GsPlayerSet::NumPlayers(int acct_type) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  int retval = 0;
  for (GsPlayerSetImp::iterator i = imp_->begin(); i != imp_->end(); ++i) {
    if ((*i).second->DB().AccountType() == acct_type) {
      retval++;
    }
  }
  return retval;
}

////
// GetPlayer
////

GsPlayer* GsPlayerSet::GetPlayer(lyra_id_t playerid) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // use the hash table
  GsPlayer* retval = 0;
  GsPlayerSetImp::const_iterator i = imp_->find(playerid);
  if (i != imp_->end()) {
    retval = (*i).second;
  }
  // main_->Log()->Debug(_T("GsPlayerSet::GetPlayer: %u -> [%p]"), playerid, retval);
  return retval;
}

////
// GetPlayerList: copy all players in level into list
////

void GsPlayerSet::GetPlayerList(GsPlayerList& plist) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  for (GsPlayerSetImp::iterator i = imp_->begin(); i != imp_->end(); ++i) {
    plist.push_back((*i).second);
  }
}

////
// AllocatePlayer: find unused player slot, return pointer to it or 0 if none exist
////

GsPlayer* GsPlayerSet::AllocatePlayer(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  GsPlayer* player = 0;
  for (int i = 0; i < max_players_; ++i) {
    if (!players_[i].InUse()) {
      player = &players_[i];
      player->SetInUse(true);
      (*imp_)[playerid] = player;
      total_logins_++;
      break;
    }
  }
  //  main_->Log()->Debug(_T("GsPlayerSet::AllocatePlayer: %u -> [%p]"), playerid, player);
  if (imp_->size() > max_loggedin_) {
    max_loggedin_ = imp_->size();
  }
  return player;
}

////
// RemovePlayer
////

void GsPlayerSet::RemovePlayer(GsPlayer* player, bool save)
{
  DEFMETHOD(GsPlayerSet, RemovePlayer);
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  GsPlayerSetImp::iterator i = imp_->find(player->PlayerID());
  if (i != imp_->end()) {
    imp_->erase(i);
    main_->Log()->Debug(_T("%s: player %u removed from player set; %d logins, %d players left"), method, 
		player->PlayerID(), main_->NumLogins(), this->NumPlayers());
    player->Logout(save);
  }
  else {
    main_->Log()->Warning(_T("%s: player %u not found in set"), method, player->PlayerID());
  }
//  if ((main_->NumLogins() >= GsMain::CLOSING_THRESHHOLD) &&
//	  (0 == this->NumPlayers()))
//  {  // if gamed is ready and the last player leaves, exit
 // 	SMsg_GS_Action smsg;
///	smsg.Init(SMsg_GS_Action::ACTION_EXIT);
 //   GsUtil::SendInternalMessage(main_, smsg, GsMain::THREAD_GAMESERVER);
  //  main_->Log()->Debug(_T("%s: last player has left game; shutting down"), method);
  //}
}

////
// Dump
////

void GsPlayerSet::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<GsPlayerSet[%p,%d]: main=[%p] players=[%p] active=%d/%d logins=%d (%d max)>\n"),
	  this, sizeof(GsPlayerSet), main_,
	  players_, imp_->size(), max_players_, total_logins_, max_loggedin_);
  for (GsPlayerSetImp::iterator i = imp_->begin(); i != imp_->end(); ++i) {
    GsPlayer* player = (*i).second;
    player->Dump(f, indent + 1);
  }
}
