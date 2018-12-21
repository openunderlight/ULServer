// LmPlayerDBC.h  -*- C++ -*-
// $Id: LmPlayerDBC.h,v 1.17 1998-04-16 19:02:44-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player database connection class

#ifndef INCLUDED_LmPlayerDBC
#define INCLUDED_LmPlayerDBC

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"
#include <mysql.h>
#include "PThMutex.h"
#include "../../libsrc/shared/SharedConstants.h"
#include "../../libsrc/gmsg/GMsg_LocateNewliesAck.h"
#include "../../libsrc/gmsg/GMsg_LocateMaresAck.h"
#include "../../libsrc/db/LmPlayerDB.h"
#include "../../libsrc/shared/LnMD5.h"

// local types

typedef TCHAR lyra_player_name[16];
typedef TCHAR lyra_player_password[12];
typedef TCHAR lyra_player_realname[40];
typedef TCHAR lyra_player_email[40];

// forward declarations

class LmLog;
class LmAvatar;
class LmGlobalDB;
class GMsg_LocateAvatarAck;

// the class

class LmPlayerDBC {

public:

  enum {
    // return codes
    MYSQL_ERROR = -1, // bad things, man
    MYSQL_NODATA = -2 // empty result,
  };

public:

  LmPlayerDBC(const TCHAR *dbuser, const TCHAR *dbpassword, const TCHAR* dbserver, int dbport);
  ~LmPlayerDBC();

  int Connect();
  void Disconnect();

  // database mutators
  //  int CreatePlayer(const TCHAR* playername, int focus_stat, const LmAvatar& avatar, unsigned int billing_id, const TCHAR* email, int acct_type, lyra_id_t& playerid);
  int SavePlayer(LmPlayerDB& player_record, bool force);
  int Login(lyra_id_t player_id, int pmare_type, int pmare_billing_type, TCHAR* gamed_ip, unsigned int gamed_port, bool first_login=false);
  int Logout(lyra_id_t player_id, unsigned int timeonline);
  int UpdateLocation(lyra_id_t player_id, lyra_id_t level_id, lyra_id_t room_id);
  int SetInitiator(lyra_id_t player_id, lyra_id_t initiator_id, lyra_id_t guild_id);
  int SetKiller(lyra_id_t player_id, lyra_id_t killer_id);
  int LockPlayerOut(lyra_id_t player_id, lyra_id_t locker_id);
  int SuspendPlayer(lyra_id_t player_id, lyra_id_t locker_id, int suspended_days);
  int ModifyXPJournal(lyra_id_t player_id, int guild, int xp_change);
  int AddOfflineXP(lyra_id_t playerid, int xp_gain);
  int DeleteArt(lyra_id_t playerid, lyra_id_t artid);
  int SaveGuildRanks(lyra_id_t playerid, LmStats& stats);
  int LogQuest(lyra_id_t origin_id, lyra_id_t target_id, int art, int skill);
  int UnGhost(TCHAR* gamed_ip, unsigned int gamed_port, LmGlobalDB* globaldb_);
  int PutLocalFileInDB(lyra_id_t player_id, LmGlobalDB* globaldb_);
  int GrantPP(lyra_id_t granter, lyra_id_t grantee, TCHAR* why, int acct_type);
  int UsePP(lyra_id_t player_id, int cost, int how, int var1, int var2, int var3);

  // database selectors
  int LoadPlayer(lyra_id_t player_id, LmPlayerDB& player_record, int pmare_type = 0);
  int GetPlayerID(const TCHAR* playername, TCHAR* outputname = 0);
  int GetPlayerName(lyra_id_t playerid, TCHAR* playername);
  int CheckPassword(lyra_id_t player_id, const MD5Hash_t* phash, const TCHAR* challenge);
  int GetLocation(lyra_id_t player_id, lyra_id_t& level_id, lyra_id_t& room_id, int& acct_type, bool isGM);
  int GetLoginStatus(lyra_id_t player_id);
  int CanLogin(lyra_id_t player_id, int* suspended_days, bool* first_login, int pmare_type);
  int GetBillingID(lyra_id_t player_id, lyra_id_t& billing_id);
  int LocateNewlyAwakened(GMsg_LocateNewliesAck* pnewly_msg);
  int LocateMares(GMsg_LocateMaresAck* pmare_msg);
  int FindHouseMembers(GMsg_LocateAvatarAck& locate_msgack, lyra_id_t guild_id, bool gm, lyra_id_t player_id);
  int NewlyNeedsAnnounce(lyra_id_t player_id, bool* announce);

  long LastSQLCode() const;

  void SetLog(LmLog* log);

  int NumCalls() const;
  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmLog* Log() const;

private:

  // mysql object
  MYSQL m_mysql;

  // not implemented
  LmPlayerDBC(const LmPlayerDBC&);
  //operator=(const LmPlayerDBC&);

  bool connected_;
  PThMutex lock_;
  long last_sql_code_;
  LmLog* log_;

  TCHAR username_[40]; 
  TCHAR password_[40];   
  TCHAR db_server_[80];
  TCHAR dbname_[20];
  int db_port_;

  int num_calls_;          // total number of calls to class
  unsigned long last_ms_;  // time of last call (milliseconds)
  unsigned long num_ms_;   // total time of all calls
  unsigned long sql_ms_;   // total time spent in "EXEC SQL" blocks

};

#ifdef USE_INLINE
#include "LmPlayerDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmPlayerDBC */
