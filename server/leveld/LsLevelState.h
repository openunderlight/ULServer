// LsLevelState.h  -*- C++ -*-
// $Id: LsLevelState.h,v 1.14 1997-09-26 14:59:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// dynamic level state class

#ifndef INCLUDED_LsLevelState
#define INCLUDED_LsLevelState

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "../../libsrc/shared/LyraDefs.h"
#include "PThMutex.h"
#include "LsLevelSerials.h"

// forward declarations

class LsMain;
class LsRoomState;
class LmLevelDBC;

// the class

class LsLevelState {

public:

public:

  LsLevelState(LsMain* lsmain);
  ~LsLevelState();

  void Init(void);

  int LoadFromDisk();
  int SaveToDisk();

  int LoadFromDB();
  int SaveToDB();

  LsRoomState* RoomState(lyra_id_t roomid) const;
  LsRoomState* Room(int num) const;

  LsLevelSerials& Serials();

  void RemovePlayer(lyra_id_t playerid, unsigned short realtime_id);

  void Dump(FILE* f, int indent = 0) const;

  unsigned short GetRealtimeID(lyra_id_t playerid);
  void FreeRealtimeID(short realtime_id);

private:

  // not implemented
  //LsLeveState(const LsLevelState&);
  //operator=(const LsLevelState&);

  // related objects
  LsMain* main_;
  const LmLevelDBC* dbc_;

  PThMutex lock_;

  // state of included rooms
  int num_rooms_;
  LsRoomState* rooms_;


  // serial numbers owned by level
  LsLevelSerials serials_;
};

#ifdef USE_INLINE
#include "LsLevelState.i"
#endif

#endif /* INCLUDED_LsLevelState */
