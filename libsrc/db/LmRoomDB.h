// LmRoomDB.h  -*- C++ -*-
// $Id: LmRoomDB.h,v 1.9 1997-11-04 23:02:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// room properties class

#ifndef INCLUDED_LmRoomDB
#define INCLUDED_LmRoomDB

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmItemGenDB.h"

// forward declarations

// the class

class LmRoomDB {

public:

  enum {
    // constants
    ROOMNAME_MAX = Lyra::ROOMNAME_MAX,
    MAX_GENERATORS = 128,

    // defaults
    DEFAULT_ROOMID = Lyra::ID_UNKNOWN,
    DEFAULT_MAXPLAYERS = Lyra::ROOMPEOPLE_MAX,
    DEFAULT_NUMGENS = 0
  };
  static const TCHAR* DEFAULT_ROOMNAME;

public:

  LmRoomDB();
  ~LmRoomDB();

  void Init(lyra_id_t room_id, TCHAR* room_name, int entry, int max_players, int no_reap);

  // selectors
  lyra_id_t RoomID() const;
  const TCHAR* RoomName() const;
  bool EntryArea() const;
  int MaxPlayers() const;
  int NumGenerators() const;
  const LmItemGenDB& Generator(int index) const;
  int NumPortals() const;
  lyra_id_t PortalLevelID(int index) const;
  lyra_id_t PortalRoomID(int index) const;
  bool NoReapItems() const;

  bool HasPortal(lyra_id_t levelid, lyra_id_t roomid) const;

  // mutators
  void SetNumGens(int num_generators);
  void SetNumPortals(int num_portals);
  void InitPortal(int index, lyra_id_t levelid, lyra_id_t roomid);

  void Dump(FILE* f, int indent = 0) const;
 
private:

  // not implemented
  LmRoomDB(const LmRoomDB&);
  //operator=(const LmRoomDB&);

  lyra_id_t roomid_;
  TCHAR roomname_[ROOMNAME_MAX];
  int maxplayers_;
  int entry_;
  int no_reap_items_;

  int num_generators_;
  LmItemGenDB* generators_;

  int num_portals_;
  struct portal_t {
    lyra_id_t levelid;
    lyra_id_t roomid;
  };
  portal_t* portals_;

};

#ifdef USE_INLINE
#include "LmRoomDB.i"
#endif

#endif /* INCLUDED_LmRoomDB */
