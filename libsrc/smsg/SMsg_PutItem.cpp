// SMsg_PutItem.cpp  -*- C++ -*-
// $Id: SMsg_PutItem.cpp,v 1.8 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "SMsg_PutItem.h"
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

#include "SMsg_PutItem.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"
#include "LmItem.h"
#include "LmPosition.h"

#ifndef USE_INLINE
#include "SMsg_PutItem.i"
#endif

////
// constructor
////

SMsg_PutItem::SMsg_PutItem()
  : LmMesg(SMsg::PUTITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, LmItem::DEFAULT_INSTANCE, 
       LmPosition::DEFAULT_INSTANCE, DEFAULT_LIFETIME);
}

////
// destructor
////

SMsg_PutItem::~SMsg_PutItem()
{
  // empty
}

////
// Init
////

void SMsg_PutItem::Init(lyra_id_t playerid, lyra_id_t roomid, const LmItem& item, 
			const LmPosition& position, int lifetime)
{
  SetPlayerID(playerid);
  SetRoomID(roomid);
  SetItem(item);
  SetPosition(position);
  SetLifetime(lifetime);
}

////
// hton
////

void SMsg_PutItem::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.roomid);
  HTONL(data_.lifetime);
  data_.item.ConvertToNetwork();
  data_.position.ConvertToNetwork();
}

////
// ntoh
////

void SMsg_PutItem::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.roomid);
  NTOHL(data_.lifetime);
  data_.item.ConvertToHost();
  data_.position.ConvertToHost();
}

////
// Dump: print to FILE stream
////

void SMsg_PutItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_PutItem[%p,%d]: "), this, sizeof(SMsg_PutItem));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u room=%u item="), PlayerID(), RoomID());
    Item().Dump1(f);
   _ftprintf(f, _T(" pos="));
    Position().Dump1(f);
   _ftprintf(f, _T(" lifetime=%d>\n"), Lifetime());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
