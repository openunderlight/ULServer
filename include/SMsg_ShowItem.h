// SMsg_ShowItem.h  -*- C++ -*-
// $Id: SMsg_ShowItem.h,v 1.1 1997-12-11 15:11:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// show item to player

#ifndef INCLUDED_SMsg_ShowItem
#define INCLUDED_SMsg_ShowItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"
#include "LmItemHdr.h"
#include "LmItem.h"

// message class

class SMsg_ShowItem : public LmMesg {

public:

  SMsg_ShowItem();
  ~SMsg_ShowItem();

  void Init(lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& itemheader, const TCHAR* itemname);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t SourceID() const;
  lyra_id_t TargetID() const;
  const LmItemHdr& ItemHeader() const;
  const TCHAR* ItemName() const;

  // mutators
  void SetSourceID(lyra_id_t sourceid);
  void SetTargetID(lyra_id_t targetid);
  void SetItemHeader(const LmItemHdr& itemheader);
  void SetItemName(const TCHAR* itemname);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t sourceid;
    lyra_id_t targetid;
    LmItemHdr itemheader;
    TCHAR itemname[LmItem::NAME_LENGTH];
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ShowItem.i"
#endif

#endif /* INCLUDED_SMsg_ShowItem */
