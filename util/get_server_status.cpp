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
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "LmServerDBC.h"
#include "SMsg_All.h"

void get_status(FILE* outf, LmSocket& sock, bool list_players);

void bail(TCHAR* str)
{
 _tprintf(_T("ERROR %s\n"), str);
  exit(1);
}

int _tmain(int argc, TCHAR** argv)
{
  // check args
  if ((argc != 3) && (argc != 4) && (argc != 5)) {
    bail(_T("usage: get_server_status list_players hostid [universe] [outfile] "));
  }

  pth_init();

  // list players?
  bool list_players = _ttoi(argv[1]);

  // get target hostid ("all" == all hosts)
  TCHAR* hostid = argv[2];

  // which universe?
  TCHAR default_universe[] = _T("prod");
  TCHAR *universe;
  if (argc > 3)
    universe = argv[3];
  else
    universe = default_universe;

  // create output file, or use stdout if none specified
  FILE* outf = stdout;
  if (argc == 5) {
    outf =_tfopen(argv[4], _T("w"));
    if (!outf) { // couldn't open the file, so write to stdout
      outf = stdout;
    }
  }

  // ignore SIGPIPE (network broken pipes)
  //  sigignore(SIGPIPE);

  // load server database
  TCHAR root_dir[FILENAME_MAX];
 _stprintf(root_dir, _T("/opt/lyra/ul/%s/"), universe);
  LmGlobalDB* globaldb_ = LmNEW(LmGlobalDB(root_dir));
  LmServerDBC* serverdbc_ = LmNEW(LmServerDBC());
  // load passwords & server info
  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  serverdbc_->Connect();
  serverdbc_->Load();
  serverdbc_->Disconnect();

  // print time
  time_t now = time(NULL);
 _ftprintf(outf, _T("TIME %u : %s"), now,_tctime(&now)); //_tctime() adds newline

  // for each server in database
  // _ftprintf(outf, _T("About to enter loop; num servers = %d\n"), serverdbc_->NumServers());
  for (int i = 0; i < serverdbc_->NumServers(); ++i) {
    // if server arg1 is 0, it's an inactive server, don't try to connect
    if (serverdbc_->Arg1(i) == 0) {
      //_ftprintf(outf, _T("Skipping %d due to zero arg\n"), i);
      continue;
    }
    // check hostid against "all", and against current server's hostid
    //    if (_tcscmp(hostid, "all") != 0) &&
    //	(_ttoi(hostid) != _ttoi(serverdbc_->HostID(i)))) {
      // not "all", and doesn't match current server
    //     _ftprintf(outf, _T("Skipping %d due to hostid mismatch %s, %s\n"), i, hostid, serverdbc_->HostID(i));
    //     _ftprintf(outf, _T("value: %d\n"),_tcscmp(hostid, serverdbc_->HostID(i)));
    //      continue;

    //    }
    // create socket
    LmSocket sock;
    if (sock.Socket(LmSockType::Inet_Stream()) < 0) {
      bail(_T("could not create socket\n"));
    }
    // get server address
    LmSockAddrInet addr;
    addr.Init(serverdbc_->HostIPAddr(i), serverdbc_->Arg1(i));
    // print server header
   _ftprintf(outf, _T("SERVER hostid=%s hostname=%s ip=%s type=%c arg1=%d arg2=%d "),
	    serverdbc_->HostID(i), serverdbc_->HostName(i), addr.AddressString(), serverdbc_->ServerType(i),
	    serverdbc_->Arg1(i), serverdbc_->Arg2(i));
    // attempt to connect to server
    if (sock.Connect(addr) < 0) {
     _ftprintf(outf, _T("  STATUS DOWN (%s)\n"), strerror(errno));
    }
    else {
      // try to retrieve status, print to file
      get_status(outf, sock, list_players);
    }
    sock.Close();
  }
  fclose(outf);
  LmDELETE(serverdbc_);
  pth_kill();
  return 0;
}

////
// get_status
////

void get_status(FILE* outf, LmSocket& sock, bool list_players)
{
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
  if (list_players && (msg_ss.NumPlayers() > 0)) {
    for (int i = 0; i < msg_ss.NumPlayers(); ++i) {
      lyra_id_t playerid = msg_ss.PlayerID(i);
      // request player status
      msg_getserverstatus.Init(SMsg_GetServerStatus::STATUS_PLAYER, playerid);
      msgbuf.ReadMessage(msg_getserverstatus);
      if (conn.SendMessage(msgbuf) < 0) {
_ftprintf(outf, _T("    PLAYER ERROR (could not send getplayerstatus msg; player=%u)\n"), playerid);
	continue;	
      }
      // receive server status
      if (conn.ReceiveMessage(msgbuf) < 0) {
_ftprintf(outf, _T("    PLAYER ERROR (could not receive playerstatus msg)\n"));
	continue;
      }
      SMsg_PlayerStatus msg_ps;
      if (msg_ps.Read(msgbuf) < 0) {
_ftprintf(outf, _T("    PLAYER ERROR (could not read playerstatus msg; mtype=%d)\n"), msgbuf.Header().MessageType());
	continue;
      }
      // print it
     _ftprintf(outf, _T("    PLAYER id=%u level=%u room=%u pos=(%d,%d) acct=%c online=%d idle=%d ip=%s name=%s\n"),
	      msg_ps.PlayerID(), msg_ps.LevelID(), msg_ps.RoomID(), msg_ps.X(), msg_ps.Y(),
	      msg_ps.AccountType(), msg_ps.Online(), msg_ps.Idle(),
	      LmSockAddrInet::AddressString(msg_ps.IPAddress()), msg_ps.PlayerName());
    }
  }
  // logout from server
  SMsg_Logout msg_logout;
  msgbuf.ReadMessage(msg_logout);
  conn.SendMessage(msgbuf);
}
