// SMsg_GetItem.cpp  -*- C++ -*-
// $Id: SMsg_GetItem.cpp,v 1.9 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "SMsg_GetItem.h"
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

#include "SMsg_GetItem.h"
#include "LyraDefs.h"
#include "SMsg.h"
#include "LmItemHdr.h"

#ifndef USE_INLINE
#include "SMsg_GetItem.i"
#endif

////
// constructor
////

SMsg_GetItem::SMsg_GetItem()
  : LmMesg(SMsg::GETITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

SMsg_GetItem::~SMsg_GetItem()
{
  // empty
}

////
// Init
////

void SMsg_GetItem::Init(lyra_id_t playerid, lyra_id_t roomid, const LmItemHdr& itemhdr)
{
  SetPlayerID(playerid);
  SetRoomID(roomid);
  SetItemHeader(itemhdr);
}

////
// hton
////

void SMsg_GetItem::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.roomid);
  data_.itemhdr.ConvertToNetwork();
}

////
// ntoh
////

void SMsg_GetItem::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.roomid);
  data_.itemhdr.ConvertToHost();
}

////
// Dump: print to FILE stream
////

void SMsg_GetItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_GetItem[%p,%d]: "), this, sizeof(SMsg_GetItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u room=%u>\n"), PlayerID(), RoomID());
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
