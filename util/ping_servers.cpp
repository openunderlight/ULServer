// ping_servers.cpp  -*- C++ -*-
// $Id: ping_servers.cpp,v 1.6 1997-10-14 21:40:11-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// program which attempts to make a connection to active Lyra servers, and reports
// whether the servers were down or up (prints to stdout)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LmSocket.h"

void bail(TCHAR* str)
{
 _tprintf(_T("ERROR %s\n"), str);
  exit(1);
}

int _tmain(int argc, TCHAR** argv)
{
  // check args
  if ((argc != 2) && (argc != 3) && (argc != 1)) {
    bail(_T("usage: ping_servers [universe] [outfile]"));
  }

  pth_init();
    
  TCHAR default_universe[] = _T("prod");
  TCHAR *universe;
  if (argc > 1)
    universe = argv[1];
  else
    universe = default_universe;

  // create output file, or use stdout if none specified
  FILE* outf = stdout;
  if (argc == 3) {
    outf =_tfopen(argv[2], _T("w"));
  }

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

  // _tprintf(_T("num servers: %d"), serverdbc_->NumServers());
  // for each server in database
  for (int i = 0; i < serverdbc_->NumServers(); ++i) {
    // if server arg1 is 0, it's an inactive or dynamically assigned server, don't try to connect
    if (serverdbc_->Arg1(i) == 0) {
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
   _ftprintf(outf, _T("SERVER hostid=%s hostname=%s ip=%s type=%c arg1=%d arg2=%d\n"),
	    serverdbc_->HostID(i), serverdbc_->HostName(i), addr.AddressString(), serverdbc_->ServerType(i),
	    serverdbc_->Arg1(i), serverdbc_->Arg2(i));

    // attempt to connect to server
    if (sock.Connect(addr) < 0) {
     _ftprintf(outf, _T("  PING DOWN (%s)\n"), strerror(errno));
    }
    else {
     _ftprintf(outf, _T("  PING UP\n"));
    }
    // close connection
    sock.Close();
  }
  fclose(outf);
  LmDELETE(serverdbc_);

  pth_kill();
  return 0;
}
