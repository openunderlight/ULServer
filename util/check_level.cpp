// check_level.cpp  -*- C++ -*-
// $Id: check_level.cpp,v 1.1 1997-07-02 15:19:20-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// attempt to load a level file, print if it loads successfully

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LmLevelDBC.h"

int _tmain(int argc, TCHAR** argv)
{
  st_init();

  if (argc < 2) {
   _tprintf(_T("usage: check_level <level_id>\n"));
    exit(-1);
  }

  int level_id = _ttoi(argv[1]);
  
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

  LmLevelDBC* leveldbc_ = new LmLevelDBC(serverdbc_->LevelDBUsername(), serverdbc_->LevelDBPassword(), serverdbc_->DatabaseHost(), serverdbc_->DatabasePort());

  // load level
  leveldbc_->Connect();
  // load level info, with item generators
  leveldbc_->Load(level_id, true);
  leveldbc_->Disconnect();

  // dump contents
 _tprintf(_T("Level file loaded successfully.  Dump follows.\n\n"));
  leveldbc_->Dump(stdout);

  LmDELETE(serverdbc_);
  LmDELETE(leveldbc_);

  //st_kill();

  return 0;
}
