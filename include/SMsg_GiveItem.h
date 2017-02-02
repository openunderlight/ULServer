// SMsg_GiveItem.h  -*- C++ -*-
// $Id: SMsg_GiveItem.h,v 1.1 1997-09-18 16:49:05-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_SMsg_GiveItem
#define INCLUDED_SMsg_GiveItem

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

class SMsg_GiveItem : public LmMesg {

public:

  SMsg_GiveItem();
  ~SMsg_GiveItem();

  void Init(lyra_id_t sourceid, lyra_id_t targetid, const LmItem& item);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t SourceID() const;
  lyra_id_t TargetID() const;
  const LmItem& Item() const;

  // mutators
  void SetSourceID(lyra_id_t sourceid);
  void SetTargetID(lyra_id_t targetid);
  void SetItem(const LmItem& item);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t sourceid;
    lyra_id_t targetid;
    LmItem item;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GiveItem.i"
#endif

#endif /* INCLUDED_SMsg_GiveItem */
