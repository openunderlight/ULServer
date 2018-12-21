// SMsg_GetLevelPlayers.cpp  -*- C++ -*-
// $Id: SMsg_GetLevelPlayers.cpp,v 1.3 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_GetLevelPlayers.h"
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

#include "SMsg_GetLevelPlayers.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_GetLevelPlayers.i"
#endif

////
// constructor
////

SMsg_GetLevelPlayers::SMsg_GetLevelPlayers()
  : LmMesg(SMsg::GETLEVELPLAYERS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN);
}

////
// destructor
////

SMsg_GetLevelPlayers::~SMsg_GetLevelPlayers()
{
  // empty
}

////
// Init
////

void SMsg_GetLevelPlayers::Init(lyra_id_t playerid)
{
  SetPlayerID(playerid);
}

////
// hton
////

void SMsg_GetLevelPlayers::hton()
{
  HTONL(data_.playerid);
}

////
// ntoh
////

void SMsg_GetLevelPlayers::ntoh()
{
  NTOHL(data_.playerid);
}

////
// Dump: print to FILE stream
////

void SMsg_GetLevelPlayers::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_GetLevelPlayers[%p,%d]: "), this, sizeof(SMsg_GetLevelPlayers));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("playerid=%u>\n"), PlayerID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
