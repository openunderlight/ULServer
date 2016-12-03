// SMsg_LevelLogin.cpp  -*- C++ -*-
// $Id: SMsg_LevelLogin.cpp,v 1.12 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_LevelLogin.h"
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

#include "SMsg_LevelLogin.h"

#ifndef USE_INLINE
#include "SMsg_LevelLogin.i"
#endif

////
// constructor
////

SMsg_LevelLogin::SMsg_LevelLogin()
  : LmMesg(SMsg::LEVELLOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, _T("(unknown)"), 'L', INADDR_ANY, Lyra::PORT_UNKNOWN,
       LmAvatar::DEFAULT_INSTANCE, Lyra::ID_UNKNOWN, LmPeerUpdate::DEFAULT_INSTANCE, 0, _T("none"), 0, 0, 0);
}

////
// destructor
////

SMsg_LevelLogin::~SMsg_LevelLogin()
{
  // empty
}

void SMsg_LevelLogin::Init(lyra_id_t playerid, const TCHAR* playername, int acct_type, unsigned long ipaddr,
			   int serverport, const LmAvatar& avatar, lyra_id_t roomid,
			   const LmPeerUpdate& update, unsigned char hidden, const TCHAR* description, 
			   unsigned char newly_alert, unsigned char newly_awakened, unsigned char unused)
{
  SetPlayerID(playerid);
  SetPlayerName(playername);
  SetAccountType(acct_type);
  SetIPAddress(ipaddr);
  SetServerPort(serverport);
  SetAvatar(avatar);
  SetRoomID(roomid);
  SetPeerUpdate(update);
  SetHidden(hidden);
//  SetTCPOnly(tcp_only);
  SetDescription(description);
  SetNewlyAlert(newly_alert);
  SetNewlyAwakened(newly_awakened);
}

////
// hton
////

void SMsg_LevelLogin::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.acct_type);
  HTONL(data_.udp_portnum);
  data_.avatar.ConvertToNetwork();
  HTONL(data_.roomid);
  data_.update.ConvertToNetwork();
  // no conversion: ip address, playername, description, 
  // hidden, unused, newly_alert, newly_awakened
}

////
// ntoh
////

void SMsg_LevelLogin::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.acct_type);
  NTOHL(data_.udp_portnum);
  data_.avatar.ConvertToHost();
  NTOHL(data_.roomid);
  data_.update.ConvertToHost();
  // no conversion: ip address, playername, description, 
  // newly_alert, newly_awakened, hidden, unused
  calc_size();
}

////
// SetPlayerName
////

void SMsg_LevelLogin::SetPlayerName(const TCHAR* playername)
{
 _tcsnccpy(data_.playername, playername, sizeof(data_.playername));
}

////
// Dump: print to FILE stream
////

void SMsg_LevelLogin::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_LevelLogin[%p,%d]: "), this, sizeof(SMsg_LevelLogin));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player(%u)='%s' acct=%c addr=%lu port=%d room=%d hidden=%d newly_alert = %d>\n"),
	    PlayerID(), PlayerName(), AccountType(), IPAddress(), ServerPort(), RoomID(), 
		Hidden(), NewlyAlert());
    Avatar().Dump(f, indent + 1);
    PeerUpdate().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}

////
// SetDescription
////

void SMsg_LevelLogin::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// calc_size
////

void SMsg_LevelLogin::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
