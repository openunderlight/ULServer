// SMsg_ItemPickup.h  -*- C++ -*-
// $Id: SMsg_ItemPickup.h,v 1.9 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// rs->gs item pickup acknowledge message

#ifndef INCLUDED_SMsg_ItemPickup
#define INCLUDED_SMsg_ItemPickup

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"
#include "LmItem.h"

// forward references

// message class

class SMsg_ItemPickup : public LmMesg {

public:

  enum {
    // item pickup status
    PICKUP_UNKNOWN = 'U',  // invalid

    PICKUP_ERROR   = 'E',  // item not picked up
    PICKUP_OK      = 'K'   // pickup successful
  };

public:

  SMsg_ItemPickup();
  ~SMsg_ItemPickup();

  void Init(lyra_id_t playerid, const LmItem& item, int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const LmItem& Item() const;
  int Status() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetItem(const LmItem& item);
  void SetStatus(int status);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int status;
    LmItem item;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ItemPickup.i"
#endif

#endif /* INCLUDED_SMsg_ItemPickup */
