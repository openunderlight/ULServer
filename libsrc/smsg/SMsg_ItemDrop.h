// SMsg_ItemDrop.h  -*- C++ -*-
// $Id: SMsg_ItemDrop.h,v 1.11 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// rs->gs item drop acknowledge message

#ifndef INCLUDED_SMsg_ItemDrop
#define INCLUDED_SMsg_ItemDrop

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"
#include "../../libsrc/shared/LmItemHdr.h"

// forward references

// message class

class SMsg_ItemDrop : public LmMesg {

public:

  enum {
    // item drop status
    DROP_UNKNOWN = 'U',  // invalid

    DROP_ERROR   = 'E',  // item was not dropped
    DROP_OK      = 'K'   // item was dropped
  };

public:

  SMsg_ItemDrop();
  ~SMsg_ItemDrop();

  void Init(lyra_id_t playerid, const LmItemHdr& itemhdr, int status);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int Status() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetStatus(int status);
  void SetItemHeader(const LmItemHdr& itemhdr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    int status;
    LmItemHdr itemhdr;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ItemDrop.i"
#endif

#endif /* INCLUDED_SMsg_ItemDrop */
