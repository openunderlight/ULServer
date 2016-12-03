// check_serverdb.cpp  -*- C++ -*-
// $Id: check_serverdb.cpp,v 1.1 1997-08-04 20:56:35-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// attempt to load a server database file, print if it loads successfully

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LyraDefs.h"


int _tmain(int argc, TCHAR** argv)
{
  pth_init();

  // load server database
  TCHAR root_dir[FILENAME_MAX] = _T("/opt/lyra/ul/prod/");
  LmGlobalDB* globaldb_ = LmNEW(LmGlobalDB(root_dir));
  LmServerDBC* serverdbc_ = LmNEW(LmServerDBC());
  // load passwords & server info
  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  serverdbc_->Connect();
  serverdbc_->Load();
  serverdbc_->Disconnect();

  // dump contents
  serverdbc_->Dump(stdout);
  LmDELETE(serverdbc_);

  pth_kill();

  return 0;
}
