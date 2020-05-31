// rotate_logs.cpp  -*- C++ -*-
// Copyright 2000 Lyra LLC, All rights reserved.
//
// program which attempts to make a connection to active Lyra servers, and 
// tells them to rotate their logs

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmServerDBC.h"
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "LmServerDBC.h"
#include "SMsg_All.h"

void send_rotate_msg(FILE* outf, LmSocket& sock);

void bail(TCHAR* str)
{
 _tprintf(_T("ERROR %s\n"), str);
  exit(1);
}

// Note that, because the ulctl scripts that calls this relies on the
// local filesystem to do the log rotation, the script only sends
// rotate messages to servers running on the local machine

int _tmain(int argc, TCHAR** argv)
{
  // check args
  if ((argc != 3) && (argc != 4)) {
    bail(_T("usage: rotate_logs hostid [universe]"));
  }

  st_init();

  // list players?
  // get target hostid ("all" == all hosts)
  TCHAR* hostid = argv[1];

  // which universe?
  TCHAR default_universe[] = _T("prod");
  TCHAR *universe;
  if (argc == 3)
    universe = argv[2];
  else
    universe = default_universe;

  FILE* outf = stdout;

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

  // for each server in database
  //_ftprintf(outf, _T("About to enter loop; num servers = %d\n"), serverdbc_->NumServers());
  for (int i = 0; i < serverdbc_->NumServers(); ++i) {
    // if server arg1 is 0, it's an inactive server, don't try to connect
    if (serverdbc_->Arg1(i) == 0) {
     _ftprintf(outf, _T("Skipping %d due to zero arg\n"), i);
      continue;
    }
    // check hostid against "all", and against current server's hostid
    if ((_tcscmp(hostid, _T("all")) != 0) &&
	(_ttoi(hostid) != _ttoi(serverdbc_->HostID(i)))) {
      // not "all", and doesn't match current server
     _ftprintf(outf, _T("Skipping %d due to hostid mismatch %s, %s\n"), i, hostid, serverdbc_->HostID(i));
     _ftprintf(outf, _T("value: %d\n"),_tcscmp(hostid, serverdbc_->HostID(i)));
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
    // attempt to connect to server
    if (sock.Connect(addr) < 0) {
     _ftprintf(outf, _T("  SERVER DOWN (%s)\n"), strerror(errno));
    }
    else {
      // try to retrieve status, print to file
      send_rotate_msg(outf, sock);
    }
    sock.Close();
  }
  fclose(outf);
  LmDELETE(serverdbc_);
  //st_kill();
  return 0;
}

////
// send_rotate_msg
////

void send_rotate_msg(FILE* outf, LmSocket& sock)
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

  // _ftprintf(outf, _T("Sending rotate log message to server\n"));

  SMsg_RotateLogs msg;
  msg.Init(0);
  msgbuf.ReadMessage(msg);
  if (conn.SendMessage(msgbuf) < 0) {
     _ftprintf(outf, _T("  STATUS ERROR (could not send rotatelogs message)\n"));
      return;
  }

  // logout from server
  SMsg_Logout msg_logout;
  msgbuf.ReadMessage(msg_logout);
  conn.SendMessage(msgbuf);
}
