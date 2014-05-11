// get_server_status2.cpp  -*- C++ -*-
// $Id: get_server_status2.cpp,v 1.4 1998-02-12 15:33:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// program which connects to given host/port and retrieves server status

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "LmDatabase.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "SMsg_All.h"

void get_status(FILE* outf, LmConnection& conn, int what);

void bail(TCHAR* str)
{
 _tprintf(_T("ERROR %s\n"), str);
  exit(1);
}

int _tmain(int argc, TCHAR** argv)
{
  // check args
  if ((argc != 4) && (argc != 5)) {
    bail(_T("usage: get_server_status2 hostname port what [outfile]; what: 0=status, 1=players, 2=connections"));
  }

  TCHAR* hostname = argv[1];
  int port = _ttoi(argv[2]);

  // what to get
  int what = _ttoi(argv[3]);

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

  // print time
  time_t now = time(NULL);
 _ftprintf(outf, _T("TIME %u : %s"), now,_tctime(&now)); //_tctime() adds newline

  // create socket
  LmSocket sock;
  if (sock.Socket(LmSockType::Inet_Stream()) < 0) {
    bail(_T("could not create socket\n"));
  }
  // get server address
  LmSockAddrInet addr;
  addr.Init((const TCHAR*) hostname, port);
  // print server header
 _ftprintf(outf, _T("SERVER hostname=%s ip=%s port=%d\n"), hostname, addr.AddressString(), port);
  // message buffer to use
  LmSrvMesgBuf msgbuf;
  // attempt to connect to server
  if (sock.Connect(addr) < 0) {
   _ftprintf(outf, _T("  STATUS DOWN (%s)\n"), strerror(errno));
  }
  else {
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
    }
    else {
      // try to retrieve status, print to file
      get_status(outf, conn, what);
    }
    // logout from server
    SMsg_Logout msg_logout;
    msgbuf.ReadMessage(msg_logout);
    conn.SendMessage(msgbuf);
  }
  // close socket
  sock.Close();
  fclose(outf);

  return 0;
}

////
// get_status
////

void get_status(FILE* outf, LmConnection& conn, int what)
{
  LmSrvMesgBuf msgbuf;
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
  // get player status for each player?
  if ((what == 1) && (msg_ss.NumPlayers() > 0)) {
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
  // get connection status?
  if ((what == 2) && (msg_ss.NumConnections() > 0)) {
    // request connection status
    msg_getserverstatus.Init(SMsg_GetServerStatus::STATUS_CONN, 0);
    msgbuf.ReadMessage(msg_getserverstatus);
    if (conn.SendMessage(msgbuf) < 0) {
     _ftprintf(outf, _T("    CONN ERROR (could not send getconnstatus msg)\n"));
      return;
    }
    // receive conn status
    if (conn.ReceiveMessage(msgbuf) < 0) {
     _ftprintf(outf, _T("    CONN ERROR (could not receive connstatus msg)\n"));
      return;
    }
    SMsg_ConnStatus msg_cs;
    if (msg_cs.Read(msgbuf) < 0) {
     _ftprintf(outf, _T("    CONN ERROR (could not read connstatus msg; mtype=%d)\n"), msgbuf.Header().MessageType());
      return;
    }
    // print connections
    for (int i = 0; i < msg_cs.NumConnections(); ++i) {
      TCHAR local_ip[20];
      TCHAR remote_ip[20];
     _tcsnccpy(local_ip, LmSockAddrInet::AddressString(msg_cs.LocalIP(i)), sizeof(local_ip));
     _tcsnccpy(remote_ip, LmSockAddrInet::AddressString(msg_cs.RemoteIP(i)), sizeof(remote_ip));
     _ftprintf(outf, _T("    CONN id=%u type=%c local=%s:%d remote=%s:%d online=%d idle=%d/%d msgs=%d/%d bytes=%d/%d mtype=(%d,%d)\n"),
	      msg_cs.ConnectionID(i), msg_cs.ConnectionType(i),
	      local_ip, msg_cs.LocalPort(i), remote_ip, msg_cs.RemotePort(i),
	      msg_cs.Online(i), msg_cs.IdleIn(i), msg_cs.IdleOut(i),
	      msg_cs.MessagesIn(i), msg_cs.MessagesOut(i), msg_cs.BytesIn(i), msg_cs.BytesOut(i),
	      msg_cs.MinMessageType(i), msg_cs.MaxMessageType(i));
    }
  }
}
