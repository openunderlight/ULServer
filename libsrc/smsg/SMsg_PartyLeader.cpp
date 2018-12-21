// SMsg_PartyLeader.cpp  -*- C++ -*-
// $Id: SMsg_PartyLeader.cpp,v 1.1 1997-11-10 18:34:50-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_PartyLeader.h"
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

#include "SMsg_PartyLeader.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_PartyLeader.i"
#endif

////
// constructor
////

SMsg_PartyLeader::SMsg_PartyLeader()
  : LmMesg(SMsg::PARTYLEADER, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0);
}

////
// destructor
////

SMsg_PartyLeader::~SMsg_PartyLeader()
{
  // empty
}

////
// Init
////

void SMsg_PartyLeader::Init(lyra_id_t playerid, int leader_time)
{
  SetPlayerID(playerid);
  SetLeaderTime(leader_time);
}

////
// hton
////

void SMsg_PartyLeader::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.leader_time);
}

////
// ntoh
////

void SMsg_PartyLeader::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.leader_time);
}

////
// Dump: print to FILE stream
////

void SMsg_PartyLeader::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_PartyLeader[%p,%d]: "), this, sizeof(SMsg_PartyLeader));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u time=%d>\n"), PlayerID(), LeaderTime());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
