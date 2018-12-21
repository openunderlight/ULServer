// SMsg_ServerStatus.h  -*- C++ -*-
// $Id: SMsg_ServerStatus.h,v 1.4 1998-01-22 17:15:39-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server status data

#ifndef INCLUDED_SMsg_ServerStatus
#define INCLUDED_SMsg_ServerStatus

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_ServerStatus : public LmMesg {

public:

  enum {
    MAX_PLAYERS = 1024  // max # of players to list
  };

public:

  SMsg_ServerStatus();
  ~SMsg_ServerStatus();

  void Init(int uptime, int num_logins, int num_players, int num_clients, int num_monsters,
	    int num_admin, int max_players, int num_conn, int max_conn, pid_t pid, pid_t ppid, double load);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  pid_t ProcessID() const;
  pid_t ParentProcessID() const;
  int Uptime() const;
  int TotalLogins() const;
  int NumPMares() const;
  int NumPlayers() const;
  int NumClients() const;
  int NumMonsters() const;
  int NumAdmin() const;
  int MaxPlayers() const;
  int NumConnections() const;
  int MaxConnections() const;
  lyra_id_t PlayerID(int playernum) const;
  double CPULoad() const;

  // mutators
  void SetProcessID(pid_t pid);
  void SetParentProcessID(pid_t ppid);
  void SetUptime(int uptime);
  void SetTotalLogins(int total_logins);
  void SetNumPlayers(int num_players);
  void SetNumClients(int num_clients);
  void SetNumMonsters(int num_monsters);
  void SetNumAdmin(int num_admin);
  void SetMaxPlayers(int max_players);
  void SetNumConnections(int num_conn);
  void SetMaxConnections(int max_conn);
  void SetPlayerID(int playernum, lyra_id_t playerid);
  void SetCPULoad(double load);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  void calc_size();

  // message data structure
  struct data_t {
    pid_t pid;           // process id
    pid_t ppid;          // parent process id
    int uptime;          // uptime in seconds
    int total_logins;    // total logins to server
    int num_players;     // current # of active players
    int num_clients;     //   standard clients
    int num_monsters;    //   monsters
    int num_admin;       //   gamemasters
    int max_players;     // maximum # of players allowed
    int num_connections; // current # of active connections
    int max_connections; // maximum # of connections allowed
    int load;            // CPU load * 1000
    lyra_id_t players[MAX_PLAYERS]; // playerids of active players
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ServerStatus.i"
#endif

#endif /* INCLUDED_SMsg_ServerStatus */
