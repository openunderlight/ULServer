// SMsg_PlayerStatus.cpp  -*- C++ -*-
// $Id: SMsg_PlayerStatus.cpp,v 1.2 1997-10-08 18:45:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_PlayerStatus.h"
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

#include "SMsg_PlayerStatus.h"
#include "LyraDefs.h"
#include "SMsg.h"
#include "LmSockAddrInet.h"

#ifndef USE_INLINE
#include "SMsg_PlayerStatus.i"
#endif

////
// constructor
////

SMsg_PlayerStatus::SMsg_PlayerStatus()
  : LmMesg(SMsg::PLAYERSTATUS, sizeof(data_t), sizeof(data_t), &data_)
{
  Init(Lyra::ID_UNKNOWN, _T("(unknown)"), 0, 0, 0, 0, 'U', 0, INADDR_ANY, 0);
}

////
// destructor
////

SMsg_PlayerStatus::~SMsg_PlayerStatus()
{
  // empty
}

////
// Init
////

void SMsg_PlayerStatus::Init(lyra_id_t playerid, const TCHAR* name, lyra_id_t levelid, lyra_id_t roomid,
			     int x, int y, int acct_type, int online, unsigned long ip_addr, int idle)
{
  SetPlayerID(playerid);
  SetPlayerName(name);
  SetLevelID(levelid);
  SetRoomID(roomid);
  SetPosition(x, y);
  SetAccountType(acct_type);
  SetOnline(online);
  SetIPAddress(ip_addr);
  SetIdle(idle);
}

////
// hton
////

void SMsg_PlayerStatus::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.levelid);
  HTONL(data_.roomid);
  HTONS(data_.x);
  HTONS(data_.y);
  HTONL(data_.account_type);
  HTONL(data_.online);
  HTONL(data_.idle);
  // not converted: ip_addr
}

////
// ntoh
////

void SMsg_PlayerStatus::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.levelid);
  NTOHL(data_.roomid);
  NTOHS(data_.x);
  NTOHS(data_.y);
  NTOHL(data_.account_type);
  NTOHL(data_.online);
  NTOHL(data_.idle);
  // not converted: ip_addr
}

////
// Dump: print to FILE stream
////

void SMsg_PlayerStatus::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_PlayerStatus[%p,%d]: "), this, sizeof(SMsg_PlayerStatus));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player(%u)='%s' acct=%c level=%u room=%u pos=(%d,%d) ip=%s online=%d idle=%d>\n"),
	    PlayerID(), PlayerName(), AccountType(), LevelID(), RoomID(), X(), Y(),
	    LmSockAddrInet::AddressString(IPAddress()), Online(), Idle());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}

////
// SetPlayerName
////

void SMsg_PlayerStatus::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
}
