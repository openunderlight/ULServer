// get_server_status.cpp  -*- C++ -*-
// $Id: get_server_status.cpp,v 1.6 1998-02-03 23:38:38-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// program which attempts to make a connection to active Lyra servers, and reports
// their status (contents of status message)

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmServerDBC.h"
#include "LmPlayerDBC.h"
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "LmServerDBC.h"
#include "SMsg_All.h"

//void get_status(FILE* outf, LmSocket& sock, bool list_players);
void deghost(int num_players_db, lyra_id_t* players_db, int num_players_server, lyra_id_t* players_server, LmPlayerDBC* pdbc);
void get_logged_in_players(LmSocket& sock, int* num_players, lyra_id_t** players);
void deghost_player(lyra_id_t player, LmPlayerDBC* pdbc);

void bail(TCHAR* str)
{
 _tprintf(_T("ERROR %s\n"), str);
  exit(1);
}

int _tmain(int argc, TCHAR** argv)
{
  // check args
  if (argc != 3) {
    bail(_T("usage: deghost root_dir gamed_port"));
  }

  pth_init();

  TCHAR* root_dir = argv[1];
  int gamed_port = _ttoi(argv[2]);
  LmGlobalDB* globaldb_ = LmNEW(LmGlobalDB(root_dir));
  LmServerDBC* serverdbc_ = LmNEW(LmServerDBC());
  // load passwords & server info
  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  LmPlayerDBC* pdbc =  LmNEW(LmPlayerDBC(serverdbc_->PlayerDBUsername(), serverdbc_->PlayerDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort()));
  serverdbc_->Connect();
  serverdbc_->Load();
  serverdbc_->Disconnect();
  FILE* outf = stdout;
  // print time
  time_t now = time(NULL);
  _ftprintf(outf, _T("TIME %u : %s"), now,_tctime(&now)); //_tctime() adds newline

  // for each server in database
  for (int i = 0; i < serverdbc_->NumServers(); ++i) {
    // if server arg1 is 0, it's an inactive server, don't try to connect
    if (serverdbc_->Arg1(i) == 0) {      
      continue;
    }

    if (serverdbc_->ServerType(i) != 'G') {
	continue;
    }
    
    if (serverdbc_->Arg1(i) != gamed_port) {
       continue;
    }
  
    // create socket
    LmSocket sock;
    if (sock.Socket(LmSockType::Inet_Stream()) < 0) {
      bail(_T("could not create socket\n"));
    }
    // get server address
    LmSockAddrInet addr;
    addr.Init(serverdbc_->HostIPAddr(i), serverdbc_->Arg1(i));
    // print server header
    int num_players_db = 0;
    lyra_id_t* players_db;
    pdbc->GetLoggedInPlayersForGamed(gamed_port, &num_players_db, &players_db);
   _ftprintf(outf, _T("SERVER hostid=%s hostname=%s ip=%s type=%c arg1=%d arg2=%d "),
	    serverdbc_->HostID(i), serverdbc_->HostName(i), addr.AddressString(), serverdbc_->ServerType(i),
	    serverdbc_->Arg1(i), serverdbc_->Arg2(i));
    // attempt to connect to server
    if (sock.Connect(addr) < 0) {
    	_ftprintf(outf, _T("  STATUS DOWN (%s)\n"), strerror(errno));
	deghost(num_players_db, players_db, 0, NULL, pdbc);
    }
    else {
      int num_players_server = 0;
      lyra_id_t* players_server = NULL;
      get_logged_in_players(sock, &num_players_server, &players_server);
      deghost(num_players_db, players_db, num_players_server, players_server, pdbc);
    }
    sock.Close();
    break;
  }
  fclose(outf);
  LmDELETE(serverdbc_);
  pdbc->Disconnect();
  LmDELETE(pdbc);
  pth_kill();
  return 0;
}

void deghost(int num_players_db, lyra_id_t* players_db, int num_players_server, lyra_id_t* players_server, LmPlayerDBC* pdbc)
{
	if(!num_players_db)
		return; // nothing to do
	for(int i = 0; i < num_players_db; i++)
	{
		lyra_id_t dbplayer = players_db[i];
		bool found = false;
		// O(n^2), whatever
		for(int j = 0; j < num_players_server; j++)
		{
			if(players_server[i] == dbplayer)
			{
				found = true;
				break;
			}
		}

		if(!found)
			deghost_player(dbplayer, pdbc);
	}
}

void deghost_player(lyra_id_t pid, LmPlayerDBC* pdbc)
{
	// MDA 20181219 -- left here!
	// TODO: Fetch player inventory here from LmItemDBC
	// 	 Iterate over items in pack
	// 	 Fetch player pos from LmPlayerDBC
	// 	 ForEach prime in pack: drop prime at pos
	// 	 TODO TODO: sendSMsgPutItem when you drop so it updates live.
	pdbc->ForceDeghost(pid);	
}

void get_logged_in_players(LmSocket& sock, int* num_players, lyra_id_t** players)
{
  FILE* outf = stdout;
  LmSrvMesgBuf msgbuf;
  // initialize connection object
  LmConnection conn;
  conn.Init(sock);
  conn.SetMessageRange(SMsg::MIN, SMsg::MAX);
  // login to server
  SMsg_Login msg_login;
  msg_login.Init(LmConnection::CT_MONITOR, 0);
  msgbuf.ReadMessage(msg_login);
  if (conn.SendMessage(msgbuf) < 0) {
   _ftprintf(outf, _T("  STATUS ERROR (could not send login message)\n"));
    return;
  }
  // request server status
  SMsg_GetServerStatus msg_getserverstatus;
  msg_getserverstatus.Init(SMsg_GetServerStatus::STATUS_SERVER);
  msgbuf.ReadMessage(msg_getserverstatus);
  if (conn.SendMessage(msgbuf) < 0) {
   _ftprintf(outf, _T("  STATUS ERROR (could not send getserverstatus message)\n"));
    return;
  }
  // receive server status
  if (conn.ReceiveMessage(msgbuf) < 0) {
   _ftprintf(outf, _T("  STATUS ERROR (could not receive serverstatus message)\n"));
    return;
  }
  SMsg_ServerStatus msg_ss;
  if (msg_ss.Read(msgbuf) < 0) {
   _ftprintf(outf, _T("  STATUS ERROR (could not read serverstatus message; mtype=%d)\n"), msgbuf.Header().MessageType());
    return;
  }
  // print it
 _ftprintf(outf, _T("  STATUS UP pid=%lu ppid=%lu cpu=%.3lf uptime=%d logins=%d players=%d/%d (P=%d M=%d A=%d) conns=%d/%d\n"),
	  msg_ss.ProcessID(), msg_ss.ParentProcessID(), msg_ss.CPULoad(), msg_ss.Uptime(), msg_ss.TotalLogins(),
	  msg_ss.NumPlayers(), msg_ss.MaxPlayers(), msg_ss.NumClients(), msg_ss.NumMonsters(), msg_ss.NumAdmin(),
	  msg_ss.NumConnections(), msg_ss.MaxConnections());
  // get player status for each player
  *num_players = msg_ss.NumPlayers();
  if(*num_players)
    *players = new lyra_id_t[*num_players];
  for (int i = 0; i < msg_ss.NumPlayers(); ++i) {
    lyra_id_t playerid = msg_ss.PlayerID(i);
    *players[i] = playerid;
  }
  // logout from server
  SMsg_Logout msg_logout;
  msgbuf.ReadMessage(msg_logout);
  conn.SendMessage(msgbuf);
}
