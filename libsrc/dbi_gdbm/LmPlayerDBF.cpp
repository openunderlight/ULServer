// LmPlayerDBF.cpp  -*- C++ -*-
// $Id: LmPlayerDBF.cpp,v 1.5 1997-12-05 14:50:51-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmPlayerDBF.h"
#endif

#include <stdio.h>
#include <string.h>

#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LmPlayerDBF.h"
#include "LmPlayerDB.h"
#include "LmDatabase.h"
#include "LmDatabaseKey.h"

#ifndef USE_INLINE
#include "LmPlayerDBF.i"
#endif

////
// LoadFromFile
////

// handy macros
#define DBFETCH_I(f, v)   { key.SetField(f); if (db.Fetch(key, &v) < 0) return -1; }
#define DBFETCH_S(f, v)   { key.SetField(f); if (db.Fetch(key, v, sizeof(v)) < 0) return -1; }
#define DBFETCHO_I(f, v)  { key.SetField(f); if (db.Exists(key)) { if (db.Fetch(key, &v) < 0) return -1; } }

int LmPlayerDBF::LoadFromFile(LmDatabase& db, LmPlayerDB& player)
{
  // key: Player_{playerid}
  // fields: ID, Name, Password, NumItems, Item_*, ItemX_*, LastLogin, TimeOnline, RealName,
  //   Email, AccountType, NumLogins,
  //   XP, CurrentStat_*, MaxStat_*, FocusStat, GuildRank_*, Avatar, Art_*, GoalBook_*, Initiator_*
  // LmDatabaseKey key("Player", "", playerid, 0);
  LmDatabaseKey key(_T("Player"), _T(""), 0, 0);
  int tmp, i;
  // static fields
  DBFETCH_I(_T("ID"), player.playerid_);
  DBFETCH_S(_T("Name"), player.playername_);
  DBFETCH_S(_T("Password"), player.password_);
  DBFETCH_S(_T("Email"), player.email_);
  DBFETCH_S(_T("RealName"), player.realname_);
  // simple fields
  DBFETCH_I(_T("AccountType"), player.acct_type_);
  DBFETCH_I(_T("TimeOnline"), player.time_online_);
  DBFETCH_I(_T("NumLogins"), player.num_logins_);
  DBFETCH_I(_T("LastLogin"), player.last_login_);
  // stats
  DBFETCH_I(_T("XP"), tmp);
  player.Stats().SetXP(tmp);
  DBFETCH_I(_T("FocusStat"), tmp);
  player.Stats().SetFocusStat(tmp);
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    TCHAR statstr[32];
    // load max before current, so current can be properly assigned
   _stprintf(statstr, _T("MaxStat_%d"), i);
    DBFETCH_I(statstr, tmp);
    player.Stats().SetMaxStat(i, tmp);
   _stprintf(statstr, _T("CurrentStat_%d"), i);
    DBFETCH_I(statstr, tmp);
    player.Stats().SetCurrentStat(i, tmp);
  }
  // guild ranks
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR gstr[32];
   _stprintf(gstr, _T("GuildRank_%d"), i);
    tmp = Guild::NO_RANK;
    DBFETCHO_I(gstr, tmp); // guild ranks are optional fields
    if (tmp != Guild::NO_RANK) {
      player.Stats().SetGuildRank(i, tmp);
    }
  }
  // goalbook
  player.GoalBook().Empty();
  for (i = 0; i < Lyra::MAX_ACTIVE_GOALS; ++i) {
    TCHAR gbstr[32];
   _stprintf(gbstr, _T("GoalBook_%d"), i);
    tmp = 0;
    DBFETCHO_I(gbstr, tmp); // goalbook ids are optional
    if (tmp != 0) {
      player.GoalBook().Add(tmp);
    }
  }
  // arts
  for (i = 0; i < NUM_ARTS; ++i) {
    TCHAR astr[32];
   _stprintf(astr, _T("Art_%d"), i);
    tmp = 0;
    DBFETCHO_I(astr, tmp); // arts are optional fields
    player.Arts().SetSkill(i, tmp);
  }
  // avatar
  TCHAR val[LmDatabase::MAX_VALUELEN];
  DBFETCH_S(_T("Avatar"), val);
  if (player.Avatar().Parse(val) < 0) {
    return -2;
  }
  // get inventory
  int num_items;
  DBFETCH_I(_T("NumItems"), num_items);
  if (num_items > Lyra::INVENTORY_MAX) {
    return -3;
  }
  player.Inventory().RemoveAll();
  for (i = 0; i < num_items; ++i) {
    // get item
    LmItem item;
    TCHAR itemfield[32];
   _stprintf(itemfield, _T("Item_%d"), i + 1);
    DBFETCH_S(itemfield, val);
    if (item.Parse(val) < 0) {
      return -4;
    }
    player.Inventory().AddItem(item);
    // get item flags/positions
    tmp = 0;
   _stprintf(itemfield, _T("ItemX_%d"), i + 1);
    DBFETCH_I(itemfield, tmp);
    player.Inventory().SetItemX(item.Serial(), tmp);
  }
  // xp pool
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR kstr[32];
   _stprintf(kstr, _T("PoolXP_%d"), i);
    tmp = 0;
    DBFETCHO_I(kstr, tmp);
    player.Stats().SetPoolXP(i, tmp);
  }
  // guild initiators
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR kstr[32];
   _stprintf(kstr, _T("Initiator_%d"), i);
    tmp = 0;
    DBFETCHO_I(kstr, tmp);
    player.SetInitiator(i, tmp);
  }
  return 0;
}

#undef DBFETCH_I
#undef DBFETCH_S
#undef DBFETCHO_I

////
// SaveToFile
////

// handy macros
#define DBSTORE(f, v)  { key.SetField(f); if (db.Store(key, v) < 0) return -1; }

int LmPlayerDBF::SaveToFile(LmDatabase& db, const LmPlayerDB& player)
{
  // key: Player_{playerid}
  // mutable fields: AccountType, NumItems, Item_*, ItemX_*, LastLogin, TimeOnline, NumLogins
  //   XP, CurrentStat_*, MaxStat_*, GuildRank_*, Avatar, Art_*, GoalBook_*, PoolXP_*, Initiator_*
  // LmDatabaseKey key(_T("Player"), _T(""), player.PlayerID(), 0);
  LmDatabaseKey key(_T("Player"), _T(""), 0, 0);
  TCHAR val[LmDatabase::MAX_VALUELEN];
  int i;
  // name/id/email/password (static fields)
  DBSTORE(_T("ID"), player.PlayerID());
  DBSTORE(_T("Name"), player.PlayerName());
  DBSTORE(_T("Password"), player.Password());
  DBSTORE(_T("Email"), player.Email());
  DBSTORE(_T("RealName"), player.RealName());
  // store simple fields
  DBSTORE(_T("AccountType"), player.AccountType());
  DBSTORE(_T("LastLogin"), player.LastLogin());
  DBSTORE(_T("TimeOnline"), player.TimeOnline());
  DBSTORE(_T("NumLogins"), player.NumLogins());
  // clear items (so that "orphaned" items aren't in database)
  for (i = 0; i < Lyra::INVENTORY_MAX; ++i) {
    TCHAR itemfield[32];
   _stprintf(itemfield, _T("Item_%d"), i + 1);
    key.SetField(itemfield);
    db.Delete(key);
  }
  // store inventory
  DBSTORE(_T("NumItems"), player.Inventory().NumItems());
  for (i = 0; i < player.Inventory().NumItems(); ++i) {
    // store item
    TCHAR itemfield[32];
   _stprintf(itemfield, _T("Item_%d"), i + 1);
    player.Inventory().ItemByIndex(i).UnParse(val, sizeof(val));
    DBSTORE(itemfield, val);
    // store item X field
   _stprintf(itemfield, _T("ItemX_%d"), i + 1);
    DBSTORE(itemfield, player.Inventory().ItemX(i));
  }
  // stats
  DBSTORE(_T("XP"), player.Stats().XP());
  DBSTORE(_T("FocusStat"), player.Stats().FocusStat());
  for (i = 0; i < NUM_PLAYER_STATS; ++i) {
    TCHAR statstr[32];
   _stprintf(statstr, _T("CurrentStat_%d"), i);
    DBSTORE(statstr, player.Stats().CurrentStat(i));
   _stprintf(statstr, _T("MaxStat_%d"), i);
    DBSTORE(statstr, player.Stats().MaxStat(i));
  }
  // guild ranks
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR gstr[32];
   _stprintf(gstr, _T("GuildRank_%d"), i);
    DBSTORE(gstr, player.Stats().GuildRank(i));
  }
  // zero out all goals before saving goalbook
  for (i = 0; i < Lyra::MAX_ACTIVE_GOALS; ++i) {
    TCHAR gbstr[32];
   _stprintf(gbstr, _T("GoalBook_%d"), i);
    DBSTORE(gbstr, 0);
  }
  // goalbook
  for (i = 0; i < player.GoalBook().Size(); ++i) {
    TCHAR gbstr[32];
   _stprintf(gbstr, _T("GoalBook_%d"), i);
    DBSTORE(gbstr, player.GoalBook().Member(i));
  }
  // arts
  for (i = 0; i < NUM_ARTS; ++i) {
    TCHAR astr[32];
   _stprintf(astr, _T("Art_%d"), i);
    DBSTORE(astr, player.Arts().Skill(i));
  }
  // avatar
  player.Avatar().UnParse(val, sizeof(val));
  DBSTORE(_T("Avatar"), val);
  // save xp pool
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR kstr[32];
   _stprintf(kstr, _T("PoolXP_%d"), i);
    DBSTORE(kstr, player.Stats().PoolXP(i));
  }
  // save guild initiators
  for (i = 0; i < NUM_GUILDS; ++i) {
    TCHAR kstr[32];
   _stprintf(kstr, _T("Initiator_%d"), i);
    DBSTORE(kstr, player.Initiator(i));
  }
  return 0;
}

#undef DBSTORE_I
#undef DBSTORE_S
