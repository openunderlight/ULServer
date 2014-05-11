// SMsg_ServerStatus.cpp  -*- C++ -*-
// $Id: SMsg_ServerStatus.cpp,v 1.4 1998-01-22 17:15:39-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_ServerStatus.h"
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

#include "SMsg_ServerStatus.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_ServerStatus.i"
#endif

////
// constructor
////

SMsg_ServerStatus::SMsg_ServerStatus()
  : LmMesg(SMsg::SERVERSTATUS, sizeof(data_t), sizeof(data_t), &data_)
{
  Init(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0);
}

////
// destructor
////

SMsg_ServerStatus::~SMsg_ServerStatus()
{
  // empty
}

////
// Init
////

void SMsg_ServerStatus::Init(int uptime, int num_logins, int num_players, int num_clients, int num_monsters,
			     int num_admin, int max_players, int num_conn, int max_conn, pid_t pid, pid_t ppid, 
			     double load)
{
  SetProcessID(pid);
  SetParentProcessID(ppid);
  SetUptime(uptime);
  SetTotalLogins(num_logins);
  SetNumPlayers(num_players);
  SetNumClients(num_clients);
  SetNumMonsters(num_monsters);
  SetNumAdmin(num_admin);
  SetMaxPlayers(max_players);
  SetNumConnections(num_conn);
  SetMaxConnections(max_conn);
  SetCPULoad(load);
}

////
// hton
////

void SMsg_ServerStatus::hton()
{
  for (int i = 0; i < NumPlayers(); ++i) {
    HTONL(data_.players[i]);
  }
  HTONL(data_.pid);
  HTONL(data_.ppid);
  HTONL(data_.uptime);
  HTONL(data_.total_logins);
  HTONL(data_.num_players);
  HTONL(data_.num_clients);
  HTONL(data_.num_monsters);
  HTONL(data_.num_admin);
  HTONL(data_.max_players);
  HTONL(data_.num_connections);
  HTONL(data_.max_connections);
  HTONL(data_.load);
}

////
// ntoh
////

void SMsg_ServerStatus::ntoh()
{
  NTOHL(data_.pid);
  NTOHL(data_.ppid);
  NTOHL(data_.uptime);
  NTOHL(data_.total_logins);
  NTOHL(data_.num_players);
  NTOHL(data_.num_clients);
  NTOHL(data_.num_monsters);
  NTOHL(data_.num_admin);
  NTOHL(data_.max_players);
  NTOHL(data_.num_connections);
  NTOHL(data_.max_connections);
  NTOHL(data_.load);
  for (int i = 0; i < NumPlayers(); ++i) {
    NTOHL(data_.players[i]);
  }
  calc_size();
}

////
// Dump: print to FILE stream
////

void SMsg_ServerStatus::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_ServerStatus[%p,%d]: "), this, sizeof(SMsg_ServerStatus));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("pid=%lu ppid=%lu uptime=%d load=%lf logins=%d players=%d/%d (%d,%d,%d) connections=%d/%d>\n"),
	    ProcessID(), ParentProcessID(), Uptime(), CPULoad(), TotalLogins(), NumPlayers(), MaxPlayers(),
	    NumClients(), NumMonsters(), NumAdmin(), NumConnections(), MaxConnections());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}

////
// SetNumPlayers
////

void SMsg_ServerStatus::SetNumPlayers(int num_players)
{
  if (RANGE_OK(num_players, 0, MAX_PLAYERS)) {
    data_.num_players = num_players;
  }
  else {
    data_.num_players = 0;
  }
  calc_size();
}

////
// calc_size
////

void SMsg_ServerStatus::calc_size()
{
  // initial size: overall size minus variable-length field
  int size = sizeof(data_t) - sizeof(data_.players);
  // add space for items
  size += NumPlayers() * sizeof(lyra_id_t);
  SetMessageSize(size);
}
