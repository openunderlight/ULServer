// LsLevelState.cpp  -*- C++ -*-
// $Id: LsLevelState.cpp,v 1.26 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsLevelState.h"
#endif

#include <stdio.h>

#include "LsLevelState.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "LsRoomState.h"
#include "LsMain.h"
#include "LmLevelDBC.h"
#include "LmRoomDB.h"
#include "LmDatabase.h"
#include "LmDatabaseKey.h"
#include "LmGlobalDB.h"
#include "LmLocker.h"
#include "LmItemDBC.h"
#include "LmLogFile.h"

#ifndef USE_INLINE
#include "LsLevelState.i"
#endif

static lyra_id_t realtime_ids_[Lyra::MAX_LEVELPEOPLE];

DECLARE_TheFileName;

////
// Constructor
////

LsLevelState::LsLevelState(LsMain* lsmain)
  : main_(lsmain),
    serials_(lsmain)
{
  DECLARE_TheLineNum;
  dbc_ = main_->LevelDBC();
}


void LsLevelState::Init(void)
{
  DECLARE_TheLineNum;

  lock_.Init();

  // allocate room state array
  num_rooms_ = dbc_->NumRooms();
  rooms_ = LmNEW(LsRoomState[num_rooms_]);

  int i=0;
  for (i = 0; i < num_rooms_; ++i) {
    rooms_[i].Init(main_, &(dbc_->Room(i)));
  }

  // initialize all real-time ids as unused
  for (i=0; i<Lyra::ROOMPEOPLE_MAX; i++) {
	  realtime_ids_[i] = Lyra::ID_UNKNOWN;
  }

}

////
// Destructor
////

LsLevelState::~LsLevelState()
{
  DECLARE_TheLineNum;
  LmDELETEARRAY(rooms_);
  num_rooms_ = 0;
}

// support for loading and saving level data to/from the disk has been removed; 
// the supporting code has been commented out below. it is likely that the
// only reason to put the code that in would be to use support an additional 
// backup mechanism to save level state to disk when the database is not available, 
// which would also entail adding logic to recover from such a failure.  



////
// LoadFromDisk - load from GDBM file on disk
////

int LsLevelState::LoadFromDisk()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // open state database
  LmDatabase db;
  TCHAR statefile[FILENAME_MAX];
  main_->GlobalDB()->GetLevelState(statefile, dbc_->LevelID());
  if (db.Open(_T(""), statefile, GDBM_READER) == 0) {
    // load serial numbers
    serials_.LoadFromDisk(db);
    // load room state
    for (int i = 0; i < num_rooms_; ++i) {
      rooms_[i].LoadFromDisk(db);
    }
  } else {
    main_->Log()->Error(_T("Could not load level state from disk"));
	return -1;
  }
   
  db.Close();
  return 0;
}

////
// SaveToDisk - save to GDBM file on disk
////

#define DBSTORE(f, v)  { key.SetField(f); db.Store(key, v); }

int LsLevelState::SaveToDisk()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // open state database
  LmDatabase db;
  TCHAR statefile[FILENAME_MAX];
  main_->GlobalDB()->GetLevelState(statefile, dbc_->LevelID());
  if (db.Open(_T(""), statefile, GDBM_NEWDB) == 0) {
    // save serial numbers
    serials_.SaveToDisk(db);
    // save room state
    for (int i = 0; i < num_rooms_; ++i) {
      rooms_[i].SaveToDisk(db);
    }
  } else {
    main_->Log()->Error(_T("Could not save level state from disk"));
	return -1;
  }
  db.Close();
  return 0;
}

#undef DBSTORE

  
////
// LoadFromDB - load from Oracle, return 0 if successful, -1 otherwise
////

int LsLevelState::LoadFromDB()
{
  DEFMETHOD(LsLevelState, LoadFromDB);
  LmLocker mon(lock_); // lock object for method duration
  if (serials_.LoadFromDB() < 0) {
    main_->Log()->Warning(_T("%s: could not load serial numbers"), method);
    return -1;
  }
  // load rooms
  for (int i = 0; i < num_rooms_; ++i) {
    if (rooms_[i].LoadFromDB() < 0) {
      main_->Log()->Error(_T("%s: could not load state for room %u"), method, rooms_[i].DB()->RoomID());
      return -1;
    }
  }
  // loaded ok
  return 0;
}

////
// SaveToDB - save to Oracle, return 0 if successful, -1 otherwise
////

int LsLevelState::SaveToDB()
{
  DEFMETHOD(LsLevelState, SaveToDB);
  LmLocker mon(lock_); // lock object for method duration
  if (serials_.SaveToDB() < 0) {
    main_->Log()->Error(_T("%s: could not save free serial numbers"), method);
    return -1;
  }
  // save rooms
  for (int j = 0; j < num_rooms_; ++j) {
    if (rooms_[j].SaveToDB() < 0) {
      main_->Log()->Error(_T("%s: could not save state for room %u"), method, rooms_[j].DB()->RoomID());
      return -1;
    }
  }
  // saved ok
  return 0;
}

////
// RoomState: return roomstate object for given roomid, or 0 if room not found in level
////

LsRoomState* LsLevelState::RoomState(lyra_id_t roomid) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  for (int i = 0; i < num_rooms_; ++i) {
    if (rooms_[i].DB()->RoomID() == roomid) {
      return &rooms_[i];
    }
  }
  return 0;
}

////
// Room: return roomstate object for given room number (not id), or 0
//   if index is out of range
////

LsRoomState* LsLevelState::Room(int num) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  if (num >= num_rooms_) {
    return 0;
  }
  return &rooms_[num];
}

/////
// GetRealtimeID: generates the next realtime ID to be assigned to a player in
//    this level. These are used to cut down on the space the ID takes in the
//	  real-time update packets.
/////

unsigned short LsLevelState::GetRealtimeID(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  DEFMETHOD(LsLevelState, GetRealtimeID);
  LmLocker mon(lock_); // lock object for method duration
	
  // note - we don't use realtime ID 0 cuz that's Lyra::ID_UNKNOWN

  for (int i=1; i<Lyra::ROOMPEOPLE_MAX; i++) {
	  if (!realtime_ids_[i]) {
		realtime_ids_[i] = playerid;
		return i;
	  }
  }
}


/////
// FreeRealtimeID: frees a realtime ID
/////

void LsLevelState::FreeRealtimeID(short realtime_id)
{
  DECLARE_TheLineNum;
  DEFMETHOD(LsLevelState, FreeRealtimeID);
  LmLocker mon(lock_); // lock object for method duration
	
  realtime_ids_[realtime_id] = Lyra::ID_UNKNOWN;
}

////
// RemovePlayer: remove player from level (from every room)
////

void LsLevelState::RemovePlayer(lyra_id_t playerid, unsigned short realtime_id)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  for (int i = 0; i < num_rooms_; ++i) {
    rooms_[i].RemovePlayer(playerid);
  }
}

////
// Dump
////

void LsLevelState::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LsLevelState[%p,%d]: main=[%p] db=[%p] numrooms=%d>\n"),
	  this, sizeof(LsLevelState), main_, dbc_, num_rooms_);
  serials_.Dump(f, indent + 1);
  for (int j = 0; j < num_rooms_; ++j) {
    rooms_[j].Dump(f, indent + 1);
  }
}
