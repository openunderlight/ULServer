// SMsg_ServerStatus.i  -*- C++ -*-
// $Id: SMsg_ServerStatus.i,v 1.4 1998-01-22 17:15:39-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE pid_t SMsg_ServerStatus::ProcessID() const
{
  return data_.pid;
}

INLINE pid_t SMsg_ServerStatus::ParentProcessID() const
{
  return data_.ppid;
}

INLINE int SMsg_ServerStatus::Uptime() const
{
  return data_.uptime;
}

INLINE int SMsg_ServerStatus::TotalLogins() const
{
  return data_.total_logins;
}

INLINE int SMsg_ServerStatus::NumPlayers() const
{
  return data_.num_players;
}

INLINE int SMsg_ServerStatus::NumClients() const
{
  return data_.num_clients;
}

INLINE int SMsg_ServerStatus::NumMonsters() const
{
  return data_.num_monsters;
}

INLINE int SMsg_ServerStatus::NumAdmin() const
{
  return data_.num_admin;
}

INLINE int SMsg_ServerStatus::MaxPlayers() const
{
  return data_.max_players;
}

INLINE int SMsg_ServerStatus::NumConnections() const
{
  return data_.num_connections;
}

INLINE int SMsg_ServerStatus::MaxConnections() const
{
  return data_.max_connections;
}

INLINE lyra_id_t SMsg_ServerStatus::PlayerID(int playernum) const
{
  return INDEX_OK(playernum, 0, MAX_PLAYERS) ? data_.players[playernum] : static_cast<lyra_id_t>(Lyra::ID_UNKNOWN);
}

INLINE double SMsg_ServerStatus::CPULoad() const
{
  return ((double) data_.load) / 1000.0;
}

INLINE void SMsg_ServerStatus::SetProcessID(pid_t pid)
{
  data_.pid = pid;
}

INLINE void SMsg_ServerStatus::SetParentProcessID(pid_t ppid)
{
  data_.ppid = ppid;
}

INLINE void SMsg_ServerStatus::SetUptime(int uptime)
{
  data_.uptime = uptime;
}

INLINE void SMsg_ServerStatus::SetTotalLogins(int total_logins)
{
  data_.total_logins = total_logins;
}

INLINE void SMsg_ServerStatus::SetMaxPlayers(int max_players)
{
  data_.max_players = max_players;
}

INLINE void SMsg_ServerStatus::SetNumClients(int num_clients)
{
  data_.num_clients = num_clients;
}

INLINE void SMsg_ServerStatus::SetNumMonsters(int num_monsters)
{
  data_.num_monsters = num_monsters;
}

INLINE void SMsg_ServerStatus::SetNumAdmin(int num_admin)
{
  data_.num_admin = num_admin;
}

INLINE void SMsg_ServerStatus::SetNumConnections(int num_conn)
{
  data_.num_connections = num_conn;
}

INLINE void SMsg_ServerStatus::SetMaxConnections(int max_conn)
{
  data_.max_connections = max_conn;
}

INLINE void SMsg_ServerStatus::SetPlayerID(int playernum, lyra_id_t playerid)
{
  if (INDEX_OK(playernum, 0, MAX_PLAYERS)) {
    data_.players[playernum] = playerid;
  }
}

INLINE void SMsg_ServerStatus::SetCPULoad(double load)
{
  data_.load = (int) (1000.0 * load);
}
