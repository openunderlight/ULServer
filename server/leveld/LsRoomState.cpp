// LsRoomState.cpp  -*- C++ -*-
// $Id: LsRoomState.cpp,v 1.45 1998-05-01 17:09:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsRoomState.h"
#endif

#include <stdio.h>
#include <list>
#include <algorithm>

#include "LsRoomState.h"
#include "LyraDefs.h"
#include "LmDatabase.h"
#include "LmDatabaseKey.h"
#include "LsMain.h"
#include "LmRoomDB.h"
#include "LsItemGen.h"
#include "LmRoomItem.h"
#include "LmRoomItemList.h"
#include "LmLogFile.h"
#include "LmItemDefs.h"
#include "LsLevelState.h"
#include "LmLocker.h"
#include "LmItemDBC.h"
#include "LmLevelDBC.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "LsUtil.h"
#include "LsCodexText.h"

#ifndef USE_INLINE
#include "LsRoomState.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LsRoomState::LsRoomState()
  : main_(0),
    db_(0),
    itemgens_(0)
{
  DECLARE_TheLineNum;
  lock_.Init();
}

////
// Destructor
////

LsRoomState::~LsRoomState()
{
  DECLARE_TheLineNum;
  LmDELETEARRAY(itemgens_); // core dumps for some reason
}

////
// Init
////

void LsRoomState::Init(LsMain* lsmain, const LmRoomDB* roomdb)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  main_ = lsmain;
  db_ = roomdb;
  int i;

  // initialize item generators

  int num_generators = db_->NumGenerators();
  if (num_generators > 0) {
    itemgens_ = LmNEW(LsItemGen[num_generators]);
    for (i = 0; i < num_generators; ++i) {
      itemgens_[i].Init(&db_->Generator(i));
    }
  }

}


// support for loading and saving level data to/from the disk has been removed; 
// the supporting code has been commented out below. it is likely that the
// only reason to put the code that in would be to use support an additional 
// backup mechanism to save level state to disk when the database is not available, 
// which would also entail adding logic to recover from such a failure.  


////
// LoadFromDisk: load current state from GDBM file
////

// handy macros
#define DBFETCH_I(f, v)  { key.SetField(f); if (db.Exists(key)) db.Fetch(key, &v); }
#define DBFETCH_S(f, v)  { key.SetField(f); if (db.Exists(key)) db.Fetch(key, v, sizeof(v)); }

void LsRoomState::LoadFromDisk(LmDatabase& db)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // key: Room_{id}
  // fields: NumItems, RoomItem_*
  LmDatabaseKey key(_T("Room"), _T(""), db_->RoomID(), 0);
  // fetch simple fields
  int num_items = 0;
  DBFETCH_I("NumItems", num_items);
  // get items in room
  for (int i = 0; i < num_items; ++i) {
    char keystr[LmDatabaseKey::MAX_KEYLEN];
    char val[LmDatabase::MAX_VALUELEN];
    LmRoomItem roomitem;
   _stprintf(keystr, _T("RoomItem_%d"), i + 1);
    DBFETCH_S(keystr, val);
    if (roomitem.Parse(val) == 0) {
      add_item(roomitem);
    }
  }
}

#undef DBFETCH_I
#undef DBFETCH_S

////
// SaveToDisk: save current state to GDBM file
////

// handy macros
#define DBSTORE(f, v)  { key.SetField(f); db.Store(key, v, 1); }

void LsRoomState::SaveToDisk(LmDatabase& db)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // key: Room_{id}
  // fields: NumItems, RoomItem_*
  LmDatabaseKey key(_T("Room"), _T(""), db_->RoomID(), 0);
  // store simple fields
  DBSTORE(_T("NumItems"), (long int)items_.size());
  // store items in room
  LmRoomItemList::iterator ri;
  int i = 0;
  for (ri = items_.begin(); (bool)!(ri == items_.end()); ++ri) {
    char keystr[LmDatabaseKey::MAX_KEYLEN];
    char val[LmDatabase::MAX_VALUELEN];
   _stprintf(keystr, _T("RoomItem_%d"), i + 1);
    ++i;
    (*ri).UnParse(val, sizeof(val));
    DBSTORE(keystr, val);
  }
}

#undef DBSTORE

////
// LoadFromDB - load from database, return 0 if successful, -1 if not
////

int LsRoomState::LoadFromDB()
{
  DEFMETHOD(LsRoomState, LoadFromDB);
  LmLocker mon(lock_); // lock object for method duration
  // load room items
  int rc = main_->ItemDBC()->GetRoomItems(main_->LevelDBC()->LevelID(), db_->RoomID(), items_);
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::GetRoomItems took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Warning(_T("%s: could not get room items; rc=%d, sql=%d"), method, rc, sqlcode);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return -1;
  }
  // main_->Log()->Debug(_T("%s: loaded %d room items"), method, items_.size());
  // loaded ok
  return 0;
}

////
// SaveToDB - save to database, return 0 if successful, -1 if not
////

int LsRoomState::SaveToDB()
{
  DEFMETHOD(LsRoomState, SaveToDB);
  LmLocker mon(lock_); // lock object for method duration
  // save room items
  int rc = main_->ItemDBC()->SaveRoomItems(main_->LevelDBC()->LevelID(), db_->RoomID(), items_);
  int sc = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::SaveRoomItems took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: room %u: could not save room items; rc=%d, sqlcode=%d"), method, db_->RoomID(), rc, sc);
    LsUtil::HandleItemError(main_, method, rc, sc);
  }
  return 0;
}

////
// NumPlayers
////

int LsRoomState::NumPlayers() const
{
  LmLocker mon(lock_); // lock object for method duration
  return players_.size();
}

int LsRoomState::NumPlayers(int ptype) const
{
  LmLocker mon(lock_); // lock object for method duration
  int retval = 0;
  // get each player, check their type
  for (std::list<lyra_id_t>::const_iterator i = players_.begin(); !(bool)(i == players_.end()); ++i) {
    lyra_id_t playerid = *i;
    LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
    if (!player) {
      continue;
    }
    if (player->AccountType() == ptype) {
      retval++;
    }
  }
  return retval;
}

////
// IsEmpty, IsFull
////

bool LsRoomState::IsEmpty() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return (players_.size() == 0);
}

bool LsRoomState::IsFull() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return (players_.size() >= (unsigned int)db_->MaxPlayers());
}

////
// HasPlayer
////

bool LsRoomState::HasPlayer(lyra_id_t playerid) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return (!(find(players_.begin(), players_.end(), playerid) == players_.end()));
}

////
// AddPlayer
////

void LsRoomState::AddPlayer(LsPlayer* player)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  players_.push_back(player->PlayerID());
  players_.sort();
  players_.unique();
}
  
////
// RemovePlayer
////

void LsRoomState::RemovePlayer(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  std::list<lyra_id_t>::iterator i = find(players_.begin(), players_.end(), playerid);
  if (!(i == players_.end())) {
    players_.erase(i);
  }
}



////
// AddItem
////

void LsRoomState::AddItem(const LmRoomItem& roomitem)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  add_item(roomitem);
}

////
// HasPrime
////

const LmItemHdr* LsRoomState::HasPrime(const lyra_id_t guild_id)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return has_prime(guild_id);
}

////
// HasItem
////

bool LsRoomState::HasItem(const LmItemHdr& itemhdr) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return has_item(itemhdr);
}


////
// RemoveItem
////

void LsRoomState::RemoveItem(const LmItemHdr& itemhdr)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  remove_item(itemhdr);
}

////
// CanAddItem
////

bool LsRoomState::CanAddItem() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return can_add_item();
}

////
// GetItem
////

const LmItem& LsRoomState::GetItem(const LmItemHdr& itemhdr) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  LmRoomItemList::const_iterator rv = items_.end();
  LmRoomItemList::const_iterator i;
  for (i = items_.begin(); !(bool)(i == items_.end()); ++i) {
    if ((*i).Item().Header().Equals(itemhdr)) {
      rv = i;
      break;
    }
  }
  return (*rv).Item();
}

////
// ReapItems
////

void LsRoomState::ReapItems(LmRoomItemList& reaped)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // loop through all the items and see which ones can be reaped
  time_t now = time(NULL);
  LmRoomItemList::iterator i;
  for (i = items_.begin(); !(bool)(i == items_.end()); ++i) {
    const LmRoomItem ri = *i;
    // artifacts are never reaped
    if (ri.Item().Header().FlagSet(LyraItem::FLAG_NOREAP)) {
      continue;
    }
    // if room has "noreap" flag set, reap time is extended by 30 days, and codexes are not reaped
    if (db_->NoReapItems() && !(ri.Item().Header().FlagSet(LyraItem::FLAG_ALWAYSREAP))) {
      if (LyraItem::StateFunction(ri.Item().StateField(0)) == LyraItem::SCROLL_FUNCTION) {
	continue;
      }
      if (ri.TimeToDie() < (now - (30 * 24 * 60 * 60))) { // extended by 7 days
	reaped.push_back(ri);
      }
    }
    else { // normal room, noreap flag is not set
      if (ri.TimeToDie() < now) {
	reaped.push_back(ri);
      }
    }
  }
}

////
// MakeSpaceForItem - make sure given item will be allowed to be added to room
////

void LsRoomState::MakeSpaceForItem(const LmItemHdr& itemhdr, LmRoomItem& reaped)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // if don't have it, and there's space, then nothing to do
  if (!has_item(itemhdr) && can_add_item()) {
    return;
  }
  // otherwise, if we have it, remove it
  if (has_item(itemhdr)) {
    remove_item(itemhdr);
  }
  // if we still can't add anything, reap a single item, which frees up a spot
  if (!can_add_item()) {
    for (LmRoomItemList::iterator i = items_.begin(); !(bool)(i == items_.end()); ++i) {
      const LmRoomItem ri = *i;
      if (!ri.Item().Header().FlagSet(LyraItem::FLAG_NOREAP)) {
	reaped = ri;
	return;
      }
    }
  }
}

////
// GenerateItems
////

void LsRoomState::GenerateItems(LmRoomItemList& generated)
{
  DECLARE_TheLineNum;
  DEFMETHOD(LsRoomState, GenerateItems);
  LmLocker mon(lock_); // lock object for method duration
  int tmpsize = items_.size();
  for (int i = 0; i < db_->NumGenerators(); ++i) {
    LsItemGen* ig = &itemgens_[i];
    if (!ig->ReadyToGenerate()) { // generator not ready
      continue;
    }
    if (tmpsize >= Lyra::MAX_ROOMITEMS) { // room full of items?
      break;
    }
    bool ok_to_add = true;
    // check that there isn't already an item at the same position
    for (LmRoomItemList::const_iterator j = items_.begin(); j != items_.end(); ++j) {
      if ((*j).Position() == ig->DB()->Position()) {
	ok_to_add = false;
	break;
      }
    }
    if (ok_to_add) {
      // get serial
      int serial = main_->LevelState()->Serials().GetNextSerial();
      // create item if serial is valid
      if (serial != 0) {
	LmRoomItem the_item = ig->Generate(serial);
	bool add_item = true; // default: add item to list
	// if item has a description, then it was a generated codex.
	// need to get a description, and put it in the database, since items that
	// have descriptions are purged from the DB when reaped
	if (the_item.Item().FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
	  // get description
	  TCHAR desc[Lyra::MAX_ITEMDESC];
	  main_->CodexText()->CopyDescription(desc, sizeof(desc));
	  // update db
	  int rc = main_->ItemDBC()->SetItemDescription(serial, desc);
	  int sc = main_->ItemDBC()->LastSQLCode();
	  if (rc < 0) {
	    LsUtil::HandleItemError(main_, method, rc, sc);
	    add_item = false;
	    // continue
	  }
	}
	// add to list?
	if (add_item) {
	  generated.push_back(the_item);
	  ++tmpsize;
	}
      }
    }
  }
}

////
// Dump
////

void LsRoomState::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LsRoomState[%p,%d]: main=[%p] db=[%p] roomid=%u>\n"), this, sizeof(LsRoomState),
	  main_, db_, db_->RoomID());
  indent++;
  // print players
  INDENT(indent, f);
 _ftprintf(f, _T("players(%d): "), players_.size());
  for (std::list<lyra_id_t>::const_iterator i = players_.begin(); !(bool)(i == players_.end()); ++i) {
   _ftprintf(f, _T("%u "), *i);
  }
 _ftprintf(f, _T("\n"));
  // print items
  INDENT(indent, f);
 _ftprintf(f, _T("items(%d):\n"), items_.size());
  for (LmRoomItemList::const_iterator j = items_.begin(); !(bool)(j == items_.end()); ++j) {
    (*j).Dump(f, indent + 1);
  }
  // print item generators
  INDENT(indent, f);
 _ftprintf(f, _T("generators(%d):\n"), db_->NumGenerators());
  for (int k = 0; k < db_->NumGenerators(); ++k) {
    itemgens_[k].Dump(f, indent + 1);
  }
}

////
// add_item - non-locking
////

void LsRoomState::add_item(const LmRoomItem& roomitem)
{
  DECLARE_TheLineNum;
  items_.push_back(roomitem);
}

////
// has_item - non-locking
////

bool LsRoomState::has_item(const LmItemHdr& itemhdr) const
{
  DECLARE_TheLineNum;
  bool retval = false;
  LmRoomItemList::const_iterator i;
  for (i = items_.begin(); !(bool)(i == items_.end()); ++i) {
    if ((*i).Item().Header().Equals(itemhdr)) {
      retval = true;
      break;
    }
  }
  return retval;
}

////
// has_prime - non-locking
////

const LmItemHdr* LsRoomState::has_prime(const lyra_id_t guild_id)
{
  DECLARE_TheLineNum;
  LmItemHdr* retval = NULL;
  const void* state;
  struct lyra_item_meta_essence_t prime; 

  LmRoomItemList::const_iterator i;
  for (i = items_.begin(); !(bool)(i == items_.end()); ++i) {
		state = (*i).Item().StateField(0);
		unsigned char type = *((unsigned char*)state);
		if (type != LyraItem::META_ESSENCE_FUNCTION)
			continue;
		state = (*i).Item().StateField(0);
		memcpy(&prime, state, sizeof(lyra_item_meta_essence_t));
		if (prime.guild_id == guild_id) {
			return &((*i).Item().Header());
		}
  }
  return retval;
}

////
// remove_item - non-locking
////

void LsRoomState::remove_item(const LmItemHdr& itemhdr)
{
  DECLARE_TheLineNum;
  LmRoomItemList::iterator i;
  for (i = items_.begin(); !(bool)(i == items_.end()); ++i) {
    if ((*i).Item().Header().Equals(itemhdr)) {
      items_.erase(i);
      return;
    }
  }
}

////
// can_add_item - non-locking
////

bool LsRoomState::can_add_item() const
{
  DECLARE_TheLineNum;
  return (items_.size() < Lyra::MAX_ROOMITEMS);
}
