// SMsg_GiveItemAck.h  -*- C++ -*-
// $Id: SMsg_GiveItemAck.h,v 1.1 1997-09-19 16:42:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message class template

#ifndef INCLUDED_SMsg_GiveItemAck
#define INCLUDED_SMsg_GiveItemAck

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

class SMsg_GiveItemAck : public LmMesg {

public:

  SMsg_GiveItemAck();
  ~SMsg_GiveItemAck();

  void Init(int status, lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& itemheader);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  lyra_id_t SourceID() const;
  lyra_id_t TargetID() const;
  const LmItemHdr& ItemHeader() const;

  // mutators
  void SetStatus(int status);
  void SetSourceID(lyra_id_t sourceid);
  void SetTargetID(lyra_id_t targetid);
  void SetItemHeader(const LmItemHdr& itemheader);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    lyra_id_t sourceid;
    lyra_id_t targetid;
    LmItemHdr itemheader;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GiveItemAck.i"
#endif

#endif /* INCLUDED_SMsg_GiveItemAck */
