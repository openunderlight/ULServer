// check_level.cpp  -*- C++ -*-
// $Id: check_level.cpp,v 1.1 1997-07-02 15:19:20-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//

// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmGlobalDB.h"
#include "LmPlayerDBC.h"
#include "LmServerDBC.h"

int _tmain(int argc, TCHAR** argv)
{
  pth_init();

  if (argc < 2) {
   _tprintf(_T("usage: fix_ghost <gamed ip address> <gamed port> (to unghost a given ip address/port) OR fix_ghost <gamed host ID> 0 (to unghost all players last logged into a gamd on the host ID)\n"));
    exit(-1);
  }

  // stores the IP addresses (stored in the gamed_ip field) to be unghosted;
  // if run from masterd, there will only be one (passed as a parameter). 
  // if run from ulctl, there may be multiple IPs assigned to a single
  // host ID

  LmServerDBC::ip_address_t ip_addresses[32];
  int num_ip_addresses = 0;
  int i = 0;
  
  // if port = 0, we are running from ulctl start, and we need
  // to clean up any existing player state files, and reset the
  // logged_in field to 0 for ALL players that were last logged into
  // this machine. we determine which IP addresses are assigned to
  // this host ID by looking in the server database for game 
  // servers assigned to this host ID

  unsigned int port = _ttoi(argv[2]);

  
  TCHAR root_dir[FILENAME_MAX] = _T("/opt/lyra/ul/prod/");
  // if port = 0, ensure that Underight isn't running
  if (port == 0) {
    TCHAR pid_file[FILENAME_MAX];
   _stprintf(pid_file, _T("%svar/pid/masterd.pid"), root_dir);
    FILE* fp =_tfopen(pid_file, _T("r"));
    if (fp) {
      fclose(fp);
     _tprintf(_T("ERROR: you can never run this script with port = 0 when Underlight is running, as it may modify live player records\n"));
      exit(-1);
    }
  }


  LmGlobalDB* globaldb_ = LmNEW(LmGlobalDB(root_dir));
  LmServerDBC* serverdbc_ = LmNEW(LmServerDBC());
  // load passwords & server info
  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  if (serverdbc_->Connect() < 0) {
   _tprintf(_T("Error - could not connect to server database\n"));
    exit(-1);
  }
  serverdbc_->Load();

  if (port == 0) {
    num_ip_addresses = serverdbc_->GetServerIPs(LmServerDBC::ST_GAME, argv[1], ip_addresses); 
    //    for (i=0; i<num_ip_addresses; i++)
    //_tprintf(_T("IP %d: %s\n"), i, ip_addresses[i]);
    exit;
  } else {
    _tcscpy(ip_addresses[0], argv[1]);
    num_ip_addresses = 1;
  }

  LmPlayerDBC* playerdbc_ = new LmPlayerDBC(serverdbc_->LevelDBUsername(), serverdbc_->LevelDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort());

  // load level
  playerdbc_->Connect();

  for (i=0; i<num_ip_addresses; i++)
    playerdbc_->UnGhost(ip_addresses[i], port, globaldb_);

  playerdbc_->Disconnect();

  LmDELETE(serverdbc_);
  LmDELETE(playerdbc_);

  pth_kill();

  return 0;
}
