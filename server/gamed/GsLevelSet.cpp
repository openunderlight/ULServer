// GsLevelSet.cpp  -*- C++ -*-
// $Id: GsLevelSet.cpp,v 1.11 1997/11/17 22:05:40 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsLevelSet.h"
#endif

#include <stdio.h>

#include <list>

#include "GsLevelSet.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "GsMain.h"
#include "PThMutex.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmRoomDB.h"
#include "LmServerDBC.h"
#include "LmLogFile.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "GsLevelSet.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

GsLevelSet::GsLevelSet(GsMain* gsmain)
  : main_(gsmain)
{
  DECLARE_TheLineNum;
  lock_.Init();
}

////
// Destructor
////

GsLevelSet::~GsLevelSet()
{
  DECLARE_TheLineNum;
  // delete all level databases stored in list
  for (llist_t::iterator i = levels_.begin(); !(bool)(i == levels_.end()); ++i) {
    LmDELETE(*i);
  }
}

////
// LevelDB - retrieve database information for given level, return pointer or 0 if
//   level doesn't exist
////

const LmLevelDBC* GsLevelSet::LevelDBC(lyra_id_t levelid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object during method
  return get_level(levelid);
}

////
// CanGoto - return true if it's possible to move from "from" to "to" level/room
////

bool GsLevelSet::CanGoto(lyra_id_t from_levelid, lyra_id_t from_roomid, lyra_id_t to_levelid, lyra_id_t to_roomid)
{
  // get target level
  LmLevelDBC* to_level = get_level(to_levelid);
  if (!to_level) {
    return false;
  }
  // check if target room is in level
  if (!to_level->ContainsRoom(to_roomid)) {
    return false;
  }
  // check if target room is an entry room
  if (to_level->RoomDB(to_roomid).EntryArea()) {
    return true;
  }
  // if from level is 0 (ie. initial login), then the above checks are sufficient
  if (from_levelid == Lyra::ID_UNKNOWN) {
    return false;
  }
  // get source level
  LmLevelDBC* from_level = get_level(from_levelid);
  if (!from_level) {
    return false;
  }
  // check if source room is in level
  if (!from_level->ContainsRoom(from_roomid)) {
    return false;
  }
  // check if there is a portal to the target
  if (from_level->RoomDB(from_roomid).HasPortal(to_levelid, to_roomid)) {
    return true;
  }
  // can't go there
  return false;
}

////
// Dump
////

void GsLevelSet::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object during method
  INDENT(indent, f);
 _ftprintf(f, _T("<GsLevelSet[%p,%d]: main=[%p] levels=%d>\n"), this, sizeof(GsLevelSet), main_, levels_.size());
  for (llist_t::const_iterator i = levels_.begin();  !(bool)(i == levels_.end()); ++i) {
    (*i)->Dump(f, indent + 1);
  }
}

////
// get_level - return pointer to leveldb; loads if necessary
////

LmLevelDBC* GsLevelSet::get_level(lyra_id_t levelid)
{
  // search list
  for (llist_t::iterator i = levels_.begin(); !(bool)(i == levels_.end()); ++i) {
    if ((*i)->LevelID() == levelid) {
      return *i;
    }
  }
  // not found
  LmLevelDBC* retval = load_level(levelid);
  if (retval) { // store on list
    levels_.push_back(retval);
  }
  return retval;
}

////
// load_level - loads level and returns it; returns 0 if not found/load error
////

LmLevelDBC* GsLevelSet::load_level(lyra_id_t levelid) const
{
  LmLevelDBC* retval = 0;

  retval = LmNEW(LmLevelDBC(main_->ServerDBC()->LevelDBUsername(), main_->ServerDBC()->LevelDBPassword(), main_->ServerDBC()->DatabaseHost(), main_->ServerDBC()->DatabasePort()));
  
  retval->Connect();

  // don't load item generators - the game server doesn't care about them
  retval->Load(levelid, false);

  retval->Disconnect();

  return retval;
}
