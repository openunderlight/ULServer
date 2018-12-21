// SMsg_LocateAvatar.h  -*- C++ -*-
// $Id: SMsg_LocateAvatar.h,v 1.2 1997-10-02 14:19:09-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// locate a player in a level

#ifndef INCLUDED_SMsg_LocateAvatar
#define INCLUDED_SMsg_LocateAvatar

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// message class

class SMsg_LocateAvatar : public LmMesg {

public:

  SMsg_LocateAvatar();
  ~SMsg_LocateAvatar();

  void Init(lyra_id_t locatorid, lyra_id_t playerid, lyra_id_t roomid);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t LocatorID() const;
  lyra_id_t PlayerID() const;
  lyra_id_t RoomID() const;

  // mutators
  void SetLocatorID(lyra_id_t locatorid);
  void SetPlayerID(lyra_id_t playerid);
  void SetRoomID(lyra_id_t roomid);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t locatorid;
    lyra_id_t playerid;
    lyra_id_t roomid;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_LocateAvatar.i"
#endif

#endif /* INCLUDED_SMsg_LocateAvatar */
