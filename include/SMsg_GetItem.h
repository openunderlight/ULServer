// SMsg_GetItem.h  -*- C++ -*-
// $Id: SMsg_GetItem.h,v 1.9 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// gs->rs item pickup request

#ifndef INCLUDED_SMsg_GetItem
#define INCLUDED_SMsg_GetItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"
#include "LmItemHdr.h"

// forward references

// message class

class SMsg_GetItem : public LmMesg {

public:

  SMsg_GetItem();
  ~SMsg_GetItem();

  void Init(lyra_id_t playerid, lyra_id_t roomid, const LmItemHdr& itemhdr);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  lyra_id_t RoomID() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetRoomID(lyra_id_t roomid);
  void SetItemHeader(const LmItemHdr& itemhdr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    lyra_id_t roomid;
    LmItemHdr itemhdr;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GetItem.i"
#endif

#endif /* INCLUDED_SMsg_GetItem */
