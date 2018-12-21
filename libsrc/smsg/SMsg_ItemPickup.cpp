// SMsg_ItemPickup.cpp  -*- C++ -*-
// $Id: SMsg_ItemPickup.cpp,v 1.7 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "SMsg_ItemPickup.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "SMsg_ItemPickup.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"
#include "LmItem.h"

#ifndef USE_INLINE
#include "SMsg_ItemPickup.i"
#endif

////
// constructor
////

SMsg_ItemPickup::SMsg_ItemPickup()
  : LmMesg(SMsg::ITEMPICKUP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItem::DEFAULT_INSTANCE, PICKUP_UNKNOWN);
}

////
// destructor
////

SMsg_ItemPickup::~SMsg_ItemPickup()
{
  // empty
}

////
// Init
////

void SMsg_ItemPickup::Init(lyra_id_t playerid, const LmItem& item, int status)
{
  SetPlayerID(playerid);
  SetItem(item);
  SetStatus(status);
}

////
// hton
////

void SMsg_ItemPickup::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.status);
  data_.item.ConvertToNetwork();
}

////
// ntoh
////

void SMsg_ItemPickup::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.status);
  data_.item.ConvertToHost();
}

////
// Dump: print to FILE stream
////

void SMsg_ItemPickup::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_ItemPickup[%p,%d]: "), this, sizeof(SMsg_ItemPickup));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u status=%c>\n"), PlayerID(), Status());
    Item().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
