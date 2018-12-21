// SMsg_LocateAvatar.cpp  -*- C++ -*-
// $Id: SMsg_LocateAvatar.cpp,v 1.2 1997-10-02 14:19:09-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_LocateAvatar.h"
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

#include "SMsg_LocateAvatar.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_LocateAvatar.i"
#endif

////
// constructor
////

SMsg_LocateAvatar::SMsg_LocateAvatar()
  : LmMesg(SMsg::LOCATEAVATAR, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, 0);
}

////
// destructor
////

SMsg_LocateAvatar::~SMsg_LocateAvatar()
{
  // empty
}

////
// Init
////

void SMsg_LocateAvatar::Init(lyra_id_t locatorid, lyra_id_t playerid, lyra_id_t roomid)
{
  SetLocatorID(locatorid);
  SetPlayerID(playerid);
  SetRoomID(roomid);
}

////
// hton
////

void SMsg_LocateAvatar::hton()
{
  HTONL(data_.locatorid);
  HTONL(data_.playerid);
  HTONL(data_.roomid);
}

////
// ntoh
////

void SMsg_LocateAvatar::ntoh()
{
  NTOHL(data_.locatorid);
  NTOHL(data_.playerid);
  NTOHL(data_.roomid);
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_LocateAvatar::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_LocateAvatar[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("locator=%u player=%u room=%u>\n"), LocatorID(), PlayerID(), RoomID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
