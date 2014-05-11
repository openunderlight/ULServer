// SMsg_PutItem.h  -*- C++ -*-
// $Id: SMsg_PutItem.h,v 1.10 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// gs->rs item drop request

#ifndef INCLUDED_SMsg_PutItem
#define INCLUDED_SMsg_PutItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"
#include "LmItem.h"
#include "LmPosition.h"

// forward references

// message class

class SMsg_PutItem : public LmMesg {

public:

  enum {
    // default values
    DEFAULT_LIFETIME = 0
  };

public:

  SMsg_PutItem();
  ~SMsg_PutItem();

  void Init(lyra_id_t playerid, lyra_id_t roomid, const LmItem& item,
	    const LmPosition& position, int lifetime);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  lyra_id_t RoomID() const;
  const LmItem& Item() const;
  const LmPosition& Position() const;
  int Lifetime() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetRoomID(lyra_id_t roomid);
  void SetItem(const LmItem& item);
  void SetPosition(const LmPosition& position);
  void SetLifetime(int lifetime);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    lyra_id_t roomid;
    LmItem item;
    LmPosition position;
    int lifetime;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_PutItem.i"
#endif

#endif /* INCLUDED_SMsg_PutItem */
