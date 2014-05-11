// save_player.cpp  -*- C++ -*-
// $Id: save_player.cpp,v 1.1 1997-10-30 12:39:42-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// read a player db file and save to db, if the db file is newer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "LyraDefs.h"
#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "LmDatabase.h"
#include "LmPlayerDBF.h"
#include "LmPlayerDBC.h"
#include "LmItemDBC.h"

int _tmain(int argc, TCHAR** argv)
{
  pth_init();
  
  // check args
  if (argc != 2) {
   _tprintf(_T("usage: save_player player.db\n"));
    exit(1);
  }
  TCHAR* infile = argv[1];


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

  LmPlayerDB playerdb;

  // load player db file
  LmDatabase db;
  if (db.Open(infile, GDBM_READER) < 0) {
   _tprintf(_T("error: could not open player file '%s'\n"), infile);
    exit(1);
  }
  if (LmPlayerDBF::LoadFromFile(db, playerdb) < 0) {
   _tprintf(_T("error: could not load player file '%s'\n"), infile);
    exit(1);
  }
  db.Close();
  lyra_id_t playerid = playerdb.PlayerID();

 _tprintf(_T("player %u: last login time from file: %lu\n"), playerid, playerdb.LastLogin());

  // connect to player database
  LmPlayerDBC pdbc(_T("ul_player"), serverdbc_->PlayerDBPassword(), serverdbc_->DatabaseHost());

  int rc = pdbc.Connect();

  // connect to item database
  LmItemDBC idbc(_T("ul_item"), serverdbc_->ItemDBPassword(), serverdbc_->DatabaseHost());

  rc = idbc.Connect();
  int sc = pdbc.LastSQLCode();

  // get player's last login time from database
  int last_login = pdbc.GetLastLoginTime(playerid);
  if (last_login < 0) {
   _tprintf(_T("error: could not get last login time for player %u; rc=%d, sqlcode=%d\n"), playerid, rc, sc);
    exit(1);
  }

 _tprintf(_T("player %u: last login time from db: %lu\n"), playerdb.PlayerID(), last_login);

  // check if last login time in file is greater than database; if so, save to database
  if (last_login <= playerdb.LastLogin()) {
    rc = pdbc.SavePlayer(playerdb);
    sc = pdbc.LastSQLCode();
    if (rc < 0) {
     _tprintf(_T("error: could not save player db; rc=%d, sqlcode=%d\n"), rc, sc);
      exit(1);
    }
   _tprintf(_T("player %u: database saved\n"), playerid);
    // save inventory
    rc = idbc.SavePlayerInventory(playerid, playerdb.Inventory());
    sc = idbc.LastSQLCode();
    if (rc < 0) {
     _tprintf(_T("error: could not save player inventory; rc=%d, sqlcode=%d\n"), rc, sc);
      exit(1);
    }
   _tprintf(_T("player %u: inventory saved\n"), playerid);
  }

  // disconnect from databases
  idbc.Disconnect();
  pdbc.Disconnect();

  // remove player db file
  _tunlink(infile);

  pth_kill();

  return 0;
}
