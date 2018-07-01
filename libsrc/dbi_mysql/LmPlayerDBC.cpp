// LmPlayerDBC.pc  -*- C++ -*-
// $Id: LmPlayerDBC.pc,v 1.26 1998-04-17 16:37:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#endif

#include "LmPlayerDBF.h"
#include "LmDatabase.h"
#include "LmGlobalDB.h"
#include "LmPlayerDBC.h"
#include "LmLocker.h"
#include "LmLog.h"
#include "LmTimer.h"
#include "LmFuncTimer.h"
#include "LmAvatar.h"
#include "LmUtil.h"
#include "GMsg_LoginAck.h"
#include "GMsg_LocateAvatarAck.h"
#include "GMsg_GrantPPoint.h"
#include "GMsg_UsePPoint.h"
#include "GMsg_PPointAck.h"

const int FOCUS_INIT = 30;
const int NORM_INIT = 10;
const int MIN_TIME_FOR_COOLOFF = 60; // 1 minute min login time
const int COOLOFF_TIME = 60*15; // 15 minutes (Seeker)

inline unsigned int ATOI(char* value)
{
  if (!value)
    return 0;
  else
    return ((unsigned int)(atof(value)));
}

#ifndef USE_INLINE
#include "LmPlayerDBC.i"
#endif /* !USE_INLINE */

// constants for the precompiler
#define db_NUM_GUILDS 8        /* NUM_GUILDS */
#define db_NUM_ARTS 300        /* LmArts max # of arts */
#define db_NUM_PLAYER_STATS 5  /* NUM_PLAYER_STATS */
#define db_INITIATE 1          /* Guild::INITIATE */

// some log macros (from LmThread.h)
#define LOG_Log       if (Log()) Log()->Log
#define LOG_Warning   if (Log()) Log()->Warning
#define LOG_Debug     if (Log()) Log()->Debug
#define LOG_Error     if (Log()) Log()->Error
#define LOG_Security  if (Log()) Log()->Security


////
// Constructor
////

LmPlayerDBC::LmPlayerDBC(const TCHAR *dbuser, const TCHAR* dbpassword, const TCHAR* dbserver, int dbport)
  : connected_(false),
    log_(0),
    last_sql_code_(0),
    num_calls_(0),
    num_ms_(0),
    sql_ms_(0),
    last_ms_(0),
	db_port_(dbport)
{
  lock_.Init();
  // copy to member variables
 _tcscpy(password_, dbpassword);
 _tcscpy(db_server_, dbserver); //
#ifdef Ul3D
 _tcscpy(username_, _T("ul3d_player"));
 _tcscpy(dbname_, _T("ul3d_player"));
#else
 _tcscpy(username_, _T("ul_player"));
 _tcscpy(dbname_, _T("ul_player"));
#endif
}

////
// Destructor
////

LmPlayerDBC::~LmPlayerDBC()
{
  Disconnect();
}

////
// Dump
////

void LmPlayerDBC::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmPlayerDBC[%p,%d] calls=%d time=%lu/%lu last=%lu>\n"), this, sizeof(LmPlayerDBC),
	  num_calls_, sql_ms_, num_ms_, last_ms_);
}

////
// Connect
////

int LmPlayerDBC::Connect()
{
  DEFMETHOD(LmPlayerDBC, Connect);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (connected_) {
    // don't connect twice (disconnect, perhaps?)
    return 0; // fake successful result
  }

  ////timer.Start();

  if (!mysql_init(&m_mysql))
    {
      LOG_Error(_T("%s: MYSQL init error %s\n"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&m_mysql, db_server_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))
    {
      LOG_Error(_T("%s: MYSQL ul_player connect error %s\n"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  ////timer.Stop();

  // _tprintf(_T("\nConnected to %s as user: %s\n\n"), db_server_, username_);
  connected_ = true;
  return 0;
}

////
// Disconnect
////

void LmPlayerDBC::Disconnect()
{
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (!connected_) {
    return;
  }

  ////timer.Start();
  mysql_close(&m_mysql);  ////timer.Stop();

  connected_ = false;
}

////
// database mutators
////
////
// SavePlayer
//	force = true means save everything
//  force = false means check the player record and save only what is changed
////

int LmPlayerDBC::SavePlayer(LmPlayerDB& player_record, bool force)
{
  DEFMETHOD(LmPlayerDBC, SavePlayer);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  TCHAR query[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int i;

  static int counter = -1;

  counter++;

  // declare local variables
  unsigned int player_id;
  unsigned int avatar1;
  unsigned int avatar2;
  unsigned int xp_penalty;
  unsigned int new_skill, old_skill;
  int xp;
  int xp_bonus;
  int num_rows;
  int x,y, level_id;
  int quest_xp_pool;
  int pps, pp_pool;

  // Lyra::MAX_ACTIVE_GOALS = 8

  player_id = player_record.PlayerID();

  // skip saving player record unless we're forcing, or there was a change
  //if (force || player_record.PlayerModified())
  {

	  xp = player_record.Stats().XP();
	  xp_bonus = player_record.XPGained();
	  xp_penalty = player_record.XPLost();
	  int bt = player_record.Avatar().AvatarType();
	  avatar1 = player_record.Avatar().Avatar1();
	  avatar2 = player_record.Avatar().Avatar2();
	  x = player_record.X();
	  y = player_record.Y();
	  level_id = player_record.LevelID();
	  quest_xp_pool = player_record.Stats().QuestPoolXP();
	  pps = player_record.Stats().PP();
	  pp_pool = player_record.Stats().PPPool();

	  TCHAR escaped_descrip[768];
	  mysql_escape_string((TCHAR*)escaped_descrip,  player_record.AvatarDescrip(),_tcslen( player_record.AvatarDescrip()));

	  _stprintf(query, _T("UPDATE player SET avatar = %u, avatar2 = %u, xp = %u, xp_bonus = xp_bonus + %u, xp_penalty = xp_penalty + %u, x = %d, y = %d, last_level_id = %u, avatar_descrip = '%s', quest_xp_pool = %u, pps = %u, pp_pool = %u WHERE player_id = %u;"),
		  avatar1, avatar2, xp, xp_bonus, xp_penalty, x, y, level_id, escaped_descrip, quest_xp_pool, pps, pp_pool, player_id);

	  ////timer.Start();
	  int error = mysql_query(&m_mysql, query);
	  ////timer.Stop();

	  //  delete escaped_descrip;

	  if (error)  {
		  LOG_Error(_T("%s: Could not save player record; mysql error %s"), method, mysql_error(&m_mysql));
		  return MYSQL_ERROR;
	  }

  }

  // save stats; skip unless we're forcing, or there was a change
  //if (force || player_record.StatModified())
  {

  for (i = 0; i < NUM_PLAYER_STATS; i++) {
   _stprintf(query, _T("UPDATE stat SET  curr_stat_level = %u, max_stat_level = %u WHERE player_id = %u AND stat = %u"),  player_record.Stats().CurrentStat(i),  player_record.Stats().MaxStat(i), player_id, i);

    ////timer.Start();
    int error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)  {
      LOG_Error(_T("%s: Could not update stat %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  }

  }
  // now update arts for which there is a non-zero skill and for which
  // the current skill is not equal to the skill in the database
  // skip unless we're forcing, or there was a change
  //if (force || player_record.SkillModified())
  {

	  //int jjj = NUM_ARTS;
	  //LOG_Debug(_T("arts: %d\n"), NUM_ARTS);


	  for (i = 0; i < NUM_ARTS; i++)  {
		  new_skill = player_record.Arts().Skill(i);
		  //LOG_Debug(_T("checking art %d\n"), i);
		  if (!new_skill)
			  continue;

		  //LOG_Debug(_T("New skill in art %d is %d\n"), i, new_skill);

		  _stprintf(query, _T("SELECT skill_level FROM skill WHERE player_id = %u AND skill = %u"), player_id, i);

		  ////timer.Start();
		  int error = mysql_query(&m_mysql, query);
		  ////timer.Stop();

		  if (error)
		  {
			  LOG_Error(_T("%s: Could not select art %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
			  return MYSQL_ERROR;
		  }

		  res = mysql_store_result(&m_mysql);
		  if (res)
			  num_rows = mysql_num_rows(res);

		  if (!res || !num_rows) {
			  if (res) mysql_free_result(res);
			//  LOG_Debug(_T("New skill %d needs inserting!\n"), i);

			  //     _tprintf(_T("art needs inserting!\n"));
			  _stprintf(query, _T("INSERT INTO skill VALUES (%u, %u, %u)"), player_id, i, new_skill);
			  ////timer.Start();
			  error = mysql_query(&m_mysql, query);
			  ////timer.Stop();

			  if (error)
			  {
				  LOG_Error(_T("%s: Could not insert new skill for art %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
				  return MYSQL_ERROR;
			  }
			  continue;
		  }

		  row = mysql_fetch_row(res);
		  if (row[0])
			  old_skill = ATOI(row[0]);

		  mysql_free_result(res);

		  if (new_skill != old_skill)
		  { // update only if skill has changed
			  //_tprintf(_T("updating old value with new!\n"));
			  _stprintf(query, _T("UPDATE skill SET skill_level = %u WHERE player_id = %u AND skill = %u"), new_skill, player_id, i);
			  ////timer.Start();
			  error = mysql_query(&m_mysql, query);
			  ////timer.Stop();

			  if (error)
			  {
				  LOG_Error(_T("%s: Could not update skill for art %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
				  return MYSQL_ERROR;
			  }
		  }

	  }
  }

  // save guild ranks; skip unless we're forcing, or there was a change
  //if (force || player_record.GuildPlayerModified())
  {
	  this->SaveGuildRanks(player_id, (LmStats&)player_record.Stats());
  }

  // only save the goalbook on a forced save
  //if (force)
  {
	  // nuke goalbook before saving (ignore result code)
	  _stprintf(query, _T("DELETE FROM goalbook WHERE player_id = %u;"), player_id);

	  ////timer.Start();
	  mysql_query(&m_mysql, query);
	  ////timer.Stop();

	  for (i = 0; i< player_record.GoalBook().Size(); i++) {
		  //   _tprintf(_T("inserting new goal book entry!\n"));
		  _stprintf(query, _T("INSERT INTO goalbook (player_id, goal_id) VALUES (%u, %u)"), player_id, player_record.GoalBook().Member(i));
		  ////timer.Start();
		  int error = mysql_query(&m_mysql, query);
		  ////timer.Stop();

		  if (error)
		  {
			  LOG_Error(_T("%s: Could not update player goalbook entry %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
			  return MYSQL_ERROR;
		  }
	  }
  }

  //player_record.ClearModified();

  return 0;
}


    ////
    // Login
    ////

int LmPlayerDBC::Login(lyra_id_t player_id, int pmare_type, int pmare_billing, TCHAR* gamed_ip, unsigned int gamed_port, bool first_login)
{
  DEFMETHOD(LmPlayerDBC, Login);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[512];

  unsigned int unixtime = time(NULL);
  TCHAR escaped_ip[64];
  mysql_escape_string((TCHAR*)escaped_ip,  gamed_ip,_tcslen(gamed_ip));

  if (pmare_type && (pmare_type != Avatars::PMARE_RESUME)) {
    // starting a new pmare session
   _stprintf(query, _T("UPDATE player SET last_login = %u, last_login_date = CURDATE(),  logged_in = 1, num_logins = num_logins + 1, xp_bonus = 0,  xp_penalty = 0, pmare_session_start = %u, pmare_billing_type = %u, gamed_ip = '%s', gamed_port = %u WHERE player_id = %u;"),
		unixtime, unixtime, pmare_billing, escaped_ip, gamed_port, player_id);
  } else { // don't update pmare_session_start if not starting a pmare session
   _stprintf(query, _T("UPDATE player SET last_login = %u, last_login_date = CURDATE(),  logged_in = 1, num_logins = num_logins + 1, xp_bonus = 0,  xp_penalty = 0, gamed_ip = '%s', gamed_port = %u WHERE player_id = %u;"),
		unixtime, escaped_ip, gamed_port, player_id);
  }

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if(first_login) 
  {
    _stprintf(query, _T("UPDATE player SET first_login=CURDATE() WHERE player_id = %u;"), player_id);
    error = mysql_query(&m_mysql, query); 
  }
  if (error)
    {
      LOG_Error(_T("%s: Could not login player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}


////
// Logout
////

int LmPlayerDBC::Logout(lyra_id_t player_id, unsigned int timeonline)
{
  DEFMETHOD(LmPlayerDBC, Logout);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 if( timeonline > MIN_TIME_FOR_COOLOFF )
 {
    _stprintf( query, _T("UPDATE player SET time_online = time_online + %u, last_logout = NOW(), logged_in = 0, room_id = 0, level_id = 0 WHERE player_id = %u;"), timeonline, player_id);
    LOG_Debug( _T( "%s: Logging player out and setting last logout time! player: %u, timeonline: %u" ), method, player_id, timeonline );
 }
 else
    _stprintf(query, _T("UPDATE player SET time_online = time_online + %u, logged_in = 0, room_id = 0, level_id = 0 WHERE player_id = %u;"), timeonline, player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  LOG_Debug(_T("%s: Logging out player %u, timeonline was %u"), method, player_id, timeonline);


  if (error)
    {
      LOG_Error(_T("%s: Could not logout player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;


}

////
// DeleteArt
////

int LmPlayerDBC::DeleteArt(lyra_id_t playerid, lyra_id_t artid)
{
  DEFMETHOD(LmPlayerDBC, DeleteArt);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("DELETE FROM skill WHERE player_id = %u AND skill = %u;"), playerid, artid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not delete art %u for player %u; mysql error %s"), method, artid, playerid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;

}


////
// ModifyXPJournal
////

const int MAX_XPJOURNAL_RECURSIONS = 8;

int LmPlayerDBC::ModifyXPJournal(lyra_id_t player_id, int guild, int xp_change)
{
  DEFMETHOD(LmPlayerDBC, ModifyXPJournal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  MYSQL_RES *res;
  MYSQL_ROW row;
  int error,i,j;
  TCHAR query[256];
  unsigned int bonus;
  unsigned int penalty;
  lyra_id_t initiators[MAX_XPJOURNAL_RECURSIONS];

  for (i=0; i<MAX_XPJOURNAL_RECURSIONS; i++)
    initiators[i] = Lyra::ID_UNKNOWN;

  bonus = 0;
  penalty = 0;
  if (xp_change < 0) {
    penalty = -xp_change;
  }
  else {
    bonus = xp_change;
  }

  lyra_id_t last_initiator = player_id;

  for (i=0; i<MAX_XPJOURNAL_RECURSIONS; i++)
    {

   _stprintf(query, _T("UPDATE player SET xp_bonus = xp_bonus + %u, xp_penalty = xp_penalty + %u WHERE player_id = %u"), bonus, penalty, last_initiator);

    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("%s: Could not propagate xp journal for player %u; mysql error %s"), method, last_initiator, mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    // decay the journal values, bail if < 100
    bonus = bonus / 2;
    penalty = penalty / 2;
    if ((bonus < 100) && (penalty < 100))
      {
	//_tprintf(_T("exiting loop due to decay of bonus/penalty\n"));
	break;
      }

    // find next initiator
   _stprintf(query, _T("SELECT initiator FROM guildplayer WHERE player_id = %u AND guild_id = %u;"), last_initiator, guild);

    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("%s: Could not propagate xp journal for player %u; mysql error %s"), method, last_initiator, mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);

    if (!mysql_num_rows(res))     {
      mysql_free_result(res);
      //_tprintf(_T("exiting loop due to no result on initiator select\n"));
      break;
    }

    initiators[i] = last_initiator;

    row = mysql_fetch_row(res);

    if (row[0])
      last_initiator = ATOI(row[0]);

    mysql_free_result(res);

    if (!last_initiator)  {
      //_tprintf(_T("exiting loop due to no initiator\n"));
      break;
    }

    bool loop = false;
    for (j=0; j<i; j++)
      if (initiators[j] == last_initiator) {
	loop = true;
	break;
      }

    if (loop)
      break;

  }

  return 0;

}


////
// AddOfflineXP
////

int LmPlayerDBC::AddOfflineXP(lyra_id_t player_id, int xp_gain)
{
  DEFMETHOD(LmPlayerDBC, AddOfflineXP);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
 _stprintf(query, _T("UPDATE player SET xp_bonus = xp_bonus + %u WHERE player_id = %u;"), xp_gain, player_id);

  //_tprintf(_T("%s\n"), query);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not add offline xp for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}

////
// UpdateLocation
////

int LmPlayerDBC::UpdateLocation(lyra_id_t player_id, lyra_id_t level_id, lyra_id_t room_id)
{
  DEFMETHOD(LmPlayerDBC, UpdateLocation);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("UPDATE player SET room_id = %u, level_id = %u WHERE player_id = %u;"), room_id, level_id, player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not update location for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}

////
// SetKiller
////

int LmPlayerDBC::SetKiller(lyra_id_t player_id, lyra_id_t killer_id)
{
  DEFMETHOD(LmPlayerDBC, SetKiller);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("UPDATE player SET acct_type = 75, killer_id = %u WHERE player_id = %u;"), killer_id, player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not set killer for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}

////
// LockPlayerOut
////

int LmPlayerDBC::LockPlayerOut(lyra_id_t player_id, lyra_id_t locker_id)
{
  DEFMETHOD(LmPlayerDBC, LockPlayerOut);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("UPDATE player SET acct_type = 76, killer_id = %u WHERE player_id = %u"), locker_id, player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not lock out player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}


////
// SuspendPlayer
////

int LmPlayerDBC::SuspendPlayer(lyra_id_t player_id, lyra_id_t locker_id, int suspended_days)
{
  DEFMETHOD(LmPlayerDBC, SuspendPlayer);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("UPDATE player SET suspended_date = FROM_DAYS(TO_DAYS(CURDATE()) + %u) WHERE player_id = %u"), suspended_days, player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not suspend player %u for %u days; mysql error %s"), method, player_id, suspended_days, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}

////
// SetInitiator
////

int LmPlayerDBC::SetInitiator(lyra_id_t player_id, lyra_id_t initiator_id, lyra_id_t guild_id)
{
  DEFMETHOD(LmPlayerDBC, SetInitiator);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  unsigned int rank = db_INITIATE;

 _stprintf(query, _T("INSERT INTO guildplayer (player_id, guild_id, rank, initiator) VALUES (%u, %u, %u, %u)"), player_id, guild_id, rank, initiator_id);

  // _tprintf(_T("%s\n"), query);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not initiate player %u in guild %u from initiator %u; mysql error %s"), method, player_id, guild_id, initiator_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

   return 0;
}




////
// database selectors
////

////
// CanLogin
////

int LmPlayerDBC::CanLogin(lyra_id_t player_id, int* suspended_days, bool* first_login, int pmare_type)
{
  DEFMETHOD(LmPlayerDBC, CanLogin);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  unsigned int db_billing_id;
  int acct_type;
  int num_days;
  int logged_in = 0;
  int count = 0;
  int pmare_session_start;
  bool suspended = false;
  MYSQL_RES *res;
  MYSQL_ROW row;
  LmAvatar avatar;


  _stprintf(query, _T("SELECT acct_type, billing_id, (TO_DAYS(suspended_date) - TO_DAYS(CURDATE())), avatar, avatar2, logged_in, pmare_session_start, UNIX_TIMESTAMP(first_login) FROM player WHERE player_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not checkin CanLogin for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!res || !mysql_num_rows(res)) {
    if (res) mysql_free_result(res);
    return GMsg_LoginAck::LOGIN_USERNOTFOUND; // no record present, do not allow login
  }

  row = mysql_fetch_row(res);
  acct_type = ATOI(row[0]);
  db_billing_id = ATOI(row[1]);
  if (row[2]) {
    num_days = ATOI(row[2]);
    if (num_days > 0) {
      suspended = true;
      *suspended_days = num_days;
    }
  }
  avatar.SetAvatar1(ATOI(row[3]));
  avatar.SetAvatar2(ATOI(row[4]));

  int bt = avatar.AvatarType();

  logged_in = ATOI(row[5]);
  if (logged_in)
    return GMsg_LoginAck::LOGIN_ALREADYIN;

  if (row[6]) {
    pmare_session_start = ATOI(row[6]);
  }

  if(!row[7]) {
    *first_login = true;
  } else {
    *first_login = false;
  }

  mysql_free_result(res);

  if (acct_type == LmPlayerDB::ACCT_PMARE) {
    unsigned int unixtime = time(NULL);

    if (pmare_type == Avatars::PMARE_RESUME) {
      // resuming previous pmare session - check it was < 48 hours ago
      if ((unixtime - pmare_session_start) > (3600*48)) {
	LOG_Debug(_T("%s: pmare login rejected; now = %u, last = %u, diff = %u\n"), method, unixtime, pmare_session_start, (unixtime - pmare_session_start));
	return GMsg_LoginAck::LOGIN_PMARE_EXPIRED;
      }
    }

  }

  if (acct_type == LmPlayerDB::ACCT_KILLED) {
    return GMsg_LoginAck::LOGIN_KILLED;
  }

  if (acct_type == LmPlayerDB::ACCT_LOCKED) {
    return GMsg_LoginAck::LOGIN_TERMINATED;
  }

  if ((acct_type == LmPlayerDB::ACCT_PLAYER_EXPIRED) ||
      (acct_type == LmPlayerDB::ACCT_ADMIN_EXPIRED)) {
    return GMsg_LoginAck::LOGIN_EXPIRED;
  }

  if (suspended) {
    return GMsg_LoginAck::LOGIN_SUSPENDED;
  }

  // check pmare status

  if (acct_type == LmPlayerDB::ACCT_PMARE) {
    if ((pmare_type < Avatars::BOGROM) || (pmare_type > Avatars::PMARE_RESUME)) {
      LOG_Error(_T("%s: Illegal pmare type %u attempted at login for pmare player %u"), method, pmare_type, player_id);
      return GMsg_LoginAck::LOGIN_MISMATCH;
    }
    if ((avatar.AvatarType() < Avatars::BOGROM) || (avatar.AvatarType() > Avatars::HORRON)) {
      LOG_Error(_T("%s: Illegal pmare avatar type %u attempted at login for pmare player %u with avatar type %u"), method, pmare_type, player_id, avatar.AvatarType());
      return GMsg_LoginAck::LOGIN_MISMATCH;
    }

    //// PMARE GLOBAL LOCKOUT?
    _stprintf( query, _T( "SELECT count(*) FROM pmare_lock WHERE NOW() BETWEEN start_time AND end_time AND expired=0" ) );
    error = mysql_query( &m_mysql, query );
    if (error)
      {
	LOG_Error(_T("%s: Could not checkin CanLogin for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);

    row = mysql_fetch_row(res);
    count = ATOI(row[0]);
    mysql_free_result(res);
    if (count > 0)
      {
	return GMsg_LoginAck::LOGIN_PMARE_LOCK;
      }

  } else if (acct_type == LmPlayerDB::ACCT_PLAYER) {
    if (pmare_type != 0) {
      LOG_Error(_T("%s: Illegal pmare type %u attempted at player login for player %u"), method, pmare_type, player_id);
      return GMsg_LoginAck::LOGIN_MISMATCH;
    }
  }


  if ((db_billing_id > 0) && ((acct_type == LmPlayerDB::ACCT_PLAYER) ||
			      (acct_type == LmPlayerDB::ACCT_PMARE)))  {

   _stprintf(query, _T("SELECT count(*) FROM player WHERE billing_id = %u AND logged_in = 1"), db_billing_id);

    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("%s: Could not checkin CanLogin for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);

    row = mysql_fetch_row(res);
    count = ATOI(row[0]);
    mysql_free_result(res);
    if (count > 0)
      {
	//	LOG_Error(_T("%s: %u characters logged in for billing id %u; mysql error %s"), method, count, db_billing_id, mysql_error(&m_mysql));
	return GMsg_LoginAck::LOGIN_ALREADYIN;
      }

      if( acct_type == LmPlayerDB::ACCT_PLAYER )
      {
        _stprintf( query, _T( "SELECT count(*) FROM player WHERE billing_id=%u AND acct_type=%u AND player_id != %u AND last_logout + INTERVAL %u SECOND > NOW()" ),
            db_billing_id, LmPlayerDB::ACCT_PLAYER, player_id, COOLOFF_TIME );
        ////timer.Start();
        error = mysql_query(&m_mysql, query);
        ////timer.Stop();

        if (error)
          {
    	LOG_Error(_T("%s: Could not checkin CanLogin for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
    	return MYSQL_ERROR;
          }

        res = mysql_store_result(&m_mysql);

        row = mysql_fetch_row(res);
        count = ATOI(row[0]);
        mysql_free_result(res);
        if (count > 0)
          {
    	return GMsg_LoginAck::LOGIN_COOLOFF;
          }
      }
  }

  return GMsg_LoginAck::LOGIN_OK;
}


////
// GetLoginStatus
////

int LmPlayerDBC::GetLoginStatus(lyra_id_t player_id)
{
  DEFMETHOD(LmPlayerDBC, GetLoginStatus);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

  int logged_in = 0;

 _stprintf(query, _T("SELECT logged_in FROM player WHERE player_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check login status for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

  if (row[0])
    logged_in = ATOI(row[0]);

  mysql_free_result(res);

  return logged_in;
}

////
// GetLocation
////

int LmPlayerDBC::GetLocation(lyra_id_t player_id, lyra_id_t& level_id, lyra_id_t& room_id, int& acct_type, bool isGM) {
  DEFMETHOD(LmPlayerDBC, GetLocation);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  _stprintf(query, _T("SELECT room_id, level_id, acct_type, real_name FROM player WHERE player_id = %u"), player_id);
  TCHAR realName[128];
  int ret = 0;
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get location for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return MYSQL_ERROR; // no record present
  }

  row = mysql_fetch_row(res);

  if (row[0])
    room_id = ATOI(row[0]);
  if (row[1])
    level_id = ATOI(row[1]);
  acct_type = ATOI(row[2]);
  _tcscpy(realName, row[3]);
 

  if(!isGM && acct_type == LmPlayerDB::ACCT_ADMIN && ((NULL != _tcsstr(realName, _T("INVIS"))) || 
		(NULL != _tcsstr(realName, _T("invis"))) ||
		(NULL != _tcsstr(realName, _T("Invis")))) && level_id >= Lyra::HIDDEN_DELTA )
  {
    LOG_Debug(_T("%s: player %d is an INVIS GM, return ERROR (l:%d;r:%d)"), method, player_id, level_id, room_id);
    ret = MYSQL_ERROR;
  }
  //_tprintf(_T("acct type: %d\n"), acct_type);

  mysql_free_result(res);

  return ret;
}

int LmPlayerDBC::NewlyNeedsAnnounce(lyra_id_t player_id, bool* announce)
{
  DEFMETHOD(LmPlayerDBC, NewlyNeedsAnnounce);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  //  ////LmTimer timer(&sql_ms_);
  //
  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  _stprintf(query, _T("SELECT TO_DAYS(CURDATE()) - TO_DAYS(first_login),time_online,TIMESTAMPDIFF(MINUTE, last_logout, NOW()), xp from player where player_id=%u"), player_id);
  int ret = 0;
  int error = mysql_query(&m_mysql, query);

  if (error)
    {
      LOG_Error(_T("%s: Could not get newlyannounce for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
    res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return MYSQL_ERROR;
  }

  row = mysql_fetch_row(res);
  int days = 0;
  if(row[0])
  	days = ATOI(row[0]);
  int seconds_online = 0;
  if(row[1])
  	seconds_online = ATOI(row[1]);
  int minutes_since_last_logout = 30;
  if(row[2])
  	minutes_since_last_logout = ATOI(row[2]);
  int xp = 0;
  if(row[3])
	xp = ATOI(row[3]);
  LOG_Debug(_T("%s: player %d days=%d;seconds_online=%d;minutes_since_last_logout=%d;xp=%d"), method, player_id, days, seconds_online, minutes_since_last_logout, xp);
  if(xp > 10000 || minutes_since_last_logout < 30)
	*announce = false;
  else if(days < 10 || (seconds_online/60) < 300)
	*announce = true;
  else
	*announce = false;
  
  return ret;
}

////
// LoadPlayer
////

int LmPlayerDBC::LoadPlayer(lyra_id_t player_id, LmPlayerDB& player_record, int pmare_type)
{
  DEFMETHOD(LmPlayerDBC, LoadPlayer);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int i, num_rows;
  bool login_alert = false;
  TCHAR login_email[64];
  unsigned int ll_day = 0;
  unsigned int curr_day = 0;

 _stprintf(query, _T("SELECT player_name, password, focus_stat, avatar, avatar2, xp, acct_type, num_logins, time_online, TO_DAYS(last_login_date), real_name, email, xp_bonus, xp_penalty, billing_id, avatar_descrip, TO_DAYS(CURDATE()), login_alert, pmare_billing_type, time_online, x, y, last_level_id, quest_xp_pool, pps, pp_pool FROM player WHERE player_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not load player information for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }


  res = mysql_store_result(&m_mysql);
  num_rows = mysql_num_rows(res);
  if (!num_rows)   {
      mysql_free_result(res);
      LOG_Error(_T("%s: No player record present in databases for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_NODATA;
    }

  row = mysql_fetch_row(res);

  // Save elements into player object
  player_record.SetPlayerID(player_id);
  player_record.SetPlayerName(row[0]);
  player_record.SetPassword(row[1]);
  player_record.Stats().SetFocusStat(ATOI(row[2]));
  player_record.Avatar().SetAvatar1(ATOI(row[3]));
  player_record.Avatar().SetAvatar2(ATOI(row[4]));
  player_record.Avatar().SetHidden(0); // never hidden at login time
  player_record.SetAccountType(ATOI(row[6]));
  player_record.Stats().SetXP(ATOI(row[5]));

  if (row[7])
    player_record.SetNumLogins(ATOI(row[7]));
  if (row[8])
    player_record.SetTimeOnline(ATOI(row[8]));
  if (row[9])
    ll_day = ATOI(row[9]);
  player_record.SetLastLogin(ll_day);
  player_record.SetRealName(row[10]);
  if (row[11])
    player_record.SetEmail(row[11]);
  if (row[12])
    player_record.SetXPGained(ATOI(row[12]));
  if (row[13])
    player_record.SetXPLost(ATOI(row[13]));
  player_record.SetBillingID(ATOI(row[14]));
  if (row[15] == NULL)
    player_record.SetAvatarDescrip(_T("(none)"));
  else
    player_record.SetAvatarDescrip(row[15]);

  curr_day = ATOI(row[16]);

  if (row[17] && (_tcslen(row[17]) > 2)) {
    if (_tcsstr(row[17], _T("@"))) {
		_tcscpy(login_email, row[17]);
		login_alert = true;
    }
  }

  if (row[18]) {
    player_record.SetPMareBilling(ATOI(row[18]));
  }

  player_record.SetNewlyAwakened(0);
  if (row[19]) {
        // Added unsphered check for NewlyAlert - DiscoWay
        int xpna = player_record.Stats().XP();
	unsigned long time_online = ATOI(row[19]);
	if ((time_online < 72000) && (xpna < 10000)) {
		if ((player_record.AccountType() == LmPlayerDB::ACCT_PLAYER) ||
			(player_record.AccountType() == LmPlayerDB::ACCT_ADMIN))
		player_record.SetNewlyAwakened(1);
	}
  }

  if (row[20]) {
	  short x = ATOI(row[20]);
	  player_record.SetX(x);
  }

  if (row[21]) {
	  short y = ATOI(row[21]);
	  player_record.SetY(y);
  }

  if (row[22]) {
	  short level_id = ATOI(row[22]);
	  player_record.SetLevelID(level_id);
  }

  if (row[23]) {
	unsigned int quest_xp_pool = ATOI(row[23]);
	player_record.Stats().SetQuestPoolXP(quest_xp_pool);
  }

  if (row[24]) {
	unsigned int pp = ATOI(row[24]);
	player_record.Stats().SetPP(pp);
  }

  if (row[25]) {
	unsigned int pp_pool = ATOI(row[25]);
	player_record.Stats().SetPPPool(pp_pool);
  }

  mysql_free_result(res);

  // set the account type in the avatar record for other clients to use
  if (player_record.AccountType() == LmPlayerDB::ACCT_PLAYER)
    player_record.Avatar().SetAccountType(LmAvatar::ACCT_DREAMER);
  else if (player_record.AccountType() == LmPlayerDB::ACCT_PMARE)
    player_record.Avatar().SetAccountType(LmAvatar::ACCT_PMARE);
  else if (player_record.AccountType() == LmPlayerDB::ACCT_ADMIN) {
    if (player_record.Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
      player_record.Avatar().SetAccountType(LmAvatar::ACCT_DARKMARE);
    else
      player_record.Avatar().SetAccountType(LmAvatar::ACCT_ADMIN);
  }
  else if (player_record.AccountType() == LmPlayerDB::ACCT_MONSTER)
    player_record.Avatar().SetAccountType(LmAvatar::ACCT_NIGHTMARE);

  // now get the player stats

 _stprintf(query, _T("SELECT stat, curr_stat_level, max_stat_level  FROM stat WHERE player_id = %u"), player_id);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not load player stats for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);
  num_rows = mysql_num_rows(res);

  if (num_rows != NUM_PLAYER_STATS)    {
      mysql_free_result(res);
      LOG_Error(_T("%s: Could not load all player stats for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

    // Save elements into player object
  for (i = 0; i < num_rows; i++) {
    row = mysql_fetch_row(res);
    player_record.Stats().SetMaxStat(ATOI(row[0]), ATOI(row[2]));
    player_record.Stats().SetCurrentStat(ATOI(row[0]), ATOI(row[1]));
  }

  mysql_free_result(res);

  // Now get the player skills (arts)
 _stprintf(query, _T("SELECT skill, skill_level FROM skill WHERE player_id = %u AND skill_level > 0"), player_id);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not load player skills for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);
  num_rows = mysql_num_rows(res);

  player_record.Avatar().SetExtraDamage(0);

  // Save elements into player object
  for (i = 0; i < num_rows; i++) {
    row = mysql_fetch_row(res);
	int art = ATOI(row[0]);
	int skill = ATOI(row[1]);
	player_record.Arts().SetSkill(art, skill);
	//LOG_Debug(_T("%s: Player has art %d at rank %d"), method, player_id, art, skill);
	if ((art == Arts::GATEKEEPER) &&
	    (player_record.AccountType() != LmPlayerDB::ACCT_MONSTER))
	{ // GK's get +1 damage/plateua, max 9
		int extra_damage = skill/10;
		if (extra_damage > 9) extra_damage = 9;
		player_record.Avatar().SetExtraDamage(extra_damage);
        //LOG_Debug(_T("%s: Player %d has GK rank %d, extra damage %d/%d"), method, player_id, skill, extra_damage, player_record.Avatar().ExtraDamage());
	}
  }

  mysql_free_result(res);

  if (ll_day < curr_day) { // referesh ppoint pool
	player_record.Stats().SetPPPool(Lyra::PP_PER_DAY);
  }

  // if we're a teacher, we get newly awakened alerts
  if (player_record.Arts().Skill(Arts::TRAIN) > 0)
  {
	  player_record.SetNewlyAlert(1);
	  if (ll_day < curr_day)  { // reset Quest XP Pool if a new day
		player_record.Stats().SetQuestPoolXP(Lyra::QUEST_XP_POOL);
		player_record.Stats().SetPPPool(Lyra::PP_PER_DAY_TEACHER);
	  }
  }

  if (player_record.AccountType () == LmPlayerDB::ACCT_ADMIN)
  {
    player_record.Stats().SetPPPool(Lyra::PP_PER_DAY_ADMIN);
  }



 _stprintf(query, _T("SELECT guild_id, rank, initiator, xp_pool_curr FROM guildplayer  WHERE player_id = %u AND rank > 0;"), player_id);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not load player ranks for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);
  num_rows = mysql_num_rows(res);

// Save elements into player object

    time_t now = time(NULL);
    //    LOG_Debug(_T("%s: last login=%u now=%u"), method, last_login, now);

    for (i = 0; i < num_rows; i++) {
      row = mysql_fetch_row(res);
      int guild = ATOI(row[0]);
      int rank = ATOI(row[1]);
      int initiator = ATOI(row[2]);
      int pool = ATOI(row[3]);

      player_record.Stats().SetGuildRank(guild, rank);
	  if ((Guild::KNIGHT == rank) || (Guild::RULER == rank))
		  player_record.SetNewlyAlert(1);

      // replenish xp pool if last login was yesterday or earlier
      if (ll_day < curr_day) {
	if (rank == Guild::KNIGHT) {
	  //	  LOG_Debug(_T("%s: replenishing xp pool; player=%u, guild=%d, is knight"), method, player_id, guild);
	  player_record.Stats().SetPoolXP(guild, Lyra::KNIGHT_XP_POOL);
	  player_record.Stats().SetQuestPoolXP(Lyra::QUEST_XP_POOL);
	}
	else if (rank == Guild::RULER) {
	  //	  LOG_Debug(_T("%s: replenishing xp pool; player=%u, guild=%d, is ruler"), method, player_id, guild);
	  player_record.Stats().SetPoolXP(guild, Lyra::RULER_XP_POOL);
	  player_record.Stats().SetQuestPoolXP(Lyra::QUEST_XP_POOL);
	}
      }
      else { // otherwise use database value
	player_record.Stats().SetPoolXP(guild, pool);
      }
      player_record.SetInitiator(guild, initiator);
    }

    mysql_free_result(res);

    // now load the goalbook

   _stprintf(query, _T("SELECT goal_id FROM goalbook WHERE player_id = %u"), player_id);

    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("%s: Could not load player goals for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);
    num_rows = mysql_num_rows(res);

    // Save elements into player object
    for (i = 0; i < num_rows; i++) {
      row = mysql_fetch_row(res);
      player_record.AcceptGoal(ATOI(row[0]));
      }

    mysql_free_result(res);

    // start pmares with correct avatar, XP
    if (player_record.AccountType() == LmPlayerDB::ACCT_PMARE) {
		int pmare_xp = 0;
		int stat = 99;
		// only set dreamsoul resuming a pmare session
		if (pmare_type == Avatars::PMARE_RESUME) {
			if (player_record.Avatar().AvatarType() == Avatars::BOGROM)
				stat = 50;
		} else { // set XP, dreamsoul for new session
			player_record.Avatar().SetAvatarType(pmare_type);
			switch (pmare_type) {
			case Avatars::AGOKNIGHT:
				player_record.Stats().SetXP(5051);
				break;
			case Avatars::SHAMBLIX:
				player_record.Stats().SetXP(35360);
				break;
			case Avatars::HORRON:
				player_record.Stats().SetXP(163300);
				break;
			case Avatars::BOGROM:
			default: // bogrom
				stat = 50;
				player_record.Stats().SetXP(0);
				break;
			}
		}

		for (int i = 0; i < NUM_PLAYER_STATS; i++)
		{
			player_record.Stats().SetMaxStat(i, stat);
			player_record.Stats().SetCurrentStat(i, stat);
		}
	}

		// now, if there is a login alert set, send email
		// Added correct email - DiscoWay
		if (login_alert) {
			LmUtil::SendMail(_T("accounts@underlight.com"), login_email,
				_T("Underlight login alert"), _T("Underlight account %s, controlled by player %s, has just logged in. \n\n -gamed"), player_record.PlayerName(), player_record.RealName());

		}

	//player_record.ClearModified();
		// finished!
		return 0;
}

////
// GetPlayerID
////

int LmPlayerDBC::GetPlayerID(const TCHAR* playername, TCHAR* outputname)
{
  DEFMETHOD(LmPlayerDBC, GetPlayerID);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  lyra_id_t player_id;

  // arg check
  if (!playername || (_tcslen(playername) == 0)) {
    return Lyra::ID_UNKNOWN;
  }

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

  TCHAR escaped_upper_name[40]; // new TCHAR_tcslen(playername)*2+1];
  mysql_escape_string((TCHAR*)escaped_upper_name, playername,_tcslen(playername));

  // convert to uppercase
  int name_len =_tcslen(escaped_upper_name);
  //_tprintf(_T("converting string %d, %s\n"), name_len, escaped_upper_name);

  for (int i = 0; i < name_len; ++i) {
    escaped_upper_name[i] = _totupper(escaped_upper_name[i]);
  }

 _stprintf(query, _T("SELECT player_id, player_name FROM player WHERE upper_name = '%s'"), escaped_upper_name);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  //  delete escaped_upper_name;

  if (error)
    {
      LOG_Error(_T("%s: Could not select player ID for player %s; mysql error %s"), method, playername, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present, app needs to know!
  }

  row = mysql_fetch_row(res);

  player_id = ATOI(row[0]);
  if (outputname && row[1]) // second argument is optional
   _tcscpy(outputname, row[1]);

  mysql_free_result(res);

  return player_id;
}

////
// GetPlayerName
////

int LmPlayerDBC::GetPlayerName(lyra_id_t playerid, TCHAR* playername)
{
  DEFMETHOD(LmPlayerDBC, GetPlayerName);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _tcscpy(playername, _T("unknown"));

 _stprintf(query, _T("SELECT player_name FROM player WHERE player_id = %u"), playerid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not lookup player name for player %u; mysql error %s"), method, playerid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

 _tcscpy(playername, row[0]);

  mysql_free_result(res);

  return 0;

}

////
// GetBillingID
////

int LmPlayerDBC::GetBillingID(lyra_id_t player_id, lyra_id_t& billing_id)
{
  DEFMETHOD(LmPlayerDBC, GetBillingID);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT billing_id FROM player WHERE player_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get billing id for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

  billing_id = ATOI(row[0]);

  mysql_free_result(res);

  return 0;
}

////
// Locate Newly Awakened
////
int LmPlayerDBC::LocateNewlyAwakened(GMsg_LocateNewliesAck* pnewly_msg)
{

  DEFMETHOD(LmPlayerDBC, LocateNewlyAwakened);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  short room_id, level_id;

//  _stprintf(query, _T("SELECT player_name, level_id, room_id, time_online FROM player WHERE logged_in = 1 AND (acct_type = %u OR acct_type = %u) AND time_online < 72000 ORDER BY time_online ASC"),
// Prior fix_ghosted.pl script interferrence, fixed with this change; added check for only unsphered - DiscoWay
  _stprintf(query, _T("SELECT player_name, level_id, room_id, time_online FROM player WHERE (logged_in = 1 OR room_id != 0 OR level_id != 0) AND (acct_type = %u OR acct_type = %u) AND xp < 10000 AND time_online < 72000 ORDER BY time_online ASC"),
	  LmPlayerDB::ACCT_PLAYER, LmPlayerDB::ACCT_ADMIN);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get newly awakened locations; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  int num_players = mysql_num_rows(res);

  pnewly_msg->Init(num_players);

  if (!num_players) {
    mysql_free_result(res);
    return 0; // no record present
  }

  for (int i = 0; i<num_players; i++) {
	  row = mysql_fetch_row(res);
	  pnewly_msg->SetPlayerName(i, row[0]);
	  level_id = (short)(ATOI(row[1]));
	  room_id = (short)(ATOI(row[2]));
	  pnewly_msg->SetLocation(i, level_id, room_id);
  }

  mysql_free_result(res);
  return 0;
}


////
// Locate Mares
////
int LmPlayerDBC::LocateMares(GMsg_LocateMaresAck* pmare_msg)
{

  DEFMETHOD(LmPlayerDBC, LocateMares);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  short room_id, level_id;

  /*
  _stprintf(query, _T("SELECT player_name, level_id, room_id FROM player WHERE logged_in = 1 AND acct_type = %u) OR (acct_type = %u AND real_name like '%%DM-%%'))"),
	  LmPlayerDB::ACCT_PMARE, LmPlayerDB::ACCT_ADMIN);
	 */
  // Modified to only work for PMares. MDA 8/02/2005
  // _stprintf(query, _T("SELECT player_name, level_id, room_id FROM player WHERE logged_in = 1 AND acct_type = %u"),

  // Removed room_id, modified string to work with fix_ghosted script - DiscoWay
  _stprintf(query, _T("SELECT player_name, level_id FROM player WHERE (logged_in = 1 OR room_id != 0 OR level_id != 0) AND acct_type = %u"),
	 LmPlayerDB::ACCT_PMARE);
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get mare locations; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  int num_players = mysql_num_rows(res);

  pmare_msg->Init(num_players);

  if (!num_players) {
    mysql_free_result(res);
    return 0; // no record present
  }

  for (int i = 0; i<num_players; i++) {
	  row = mysql_fetch_row(res);
	  pmare_msg->SetPlayerName(i, row[0]);
	  level_id = (short)(ATOI(row[1]));
	  room_id = (short)(ATOI(row[2]));
	  pmare_msg->SetLocation(i, level_id, room_id);
  }

  mysql_free_result(res);
  return 0;
}


////
// FindHouseMembers
////

int LmPlayerDBC::FindHouseMembers(GMsg_LocateAvatarAck& locate_msg, lyra_id_t guild_id, bool gm, lyra_id_t player_id)
{
  DEFMETHOD(LmPlayerDBC, FindHouseMembers);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  lyra_id_t room_id, level_id;

//  _stprintf(query, _T("SELECT player_name, level_id, room_id FROM player, guildplayer WHERE logged_in = 1 AND (acct_type = %u OR acct_type = %u) AND player.player_id = guildplayer.player_id AND guildplayer.guild_id = %u AND guildplayer.rank > 0 AND player.player_id != %u LIMIT %u"),

// Same issue as LocateNewlyAwakened, applied same fix // Also applied NoGM display fix - DiscoWay
//  _stprintf(query, _T("SELECT player_name, level_id, room_id FROM player, guildplayer WHERE (logged_in = 1 OR room_id != 0 OR level_id != 0) AND (acct_type = %u OR acct_type = %u) AND player.player_id = guildplayer.player_id AND guildplayer.guild_id = %u AND guildplayer.rank > 0 AND player.player_id != %u LIMIT %u"),
//  LmPlayerDB::ACCT_PLAYER, LmPlayerDB::ACCT_ADMIN, guild_id, player_id, GMsg_LocateAvatarAck::MAX_PLAYERS);
  _stprintf(query, _T("SELECT player_name, level_id, room_id FROM player, guildplayer WHERE (logged_in = 1 OR room_id != 0 OR level_id != 0) AND acct_type = %u AND player.player_id = guildplayer.player_id AND guildplayer.guild_id = %u AND guildplayer.rank > 0 AND player.player_id != %u LIMIT %u"),
	  LmPlayerDB::ACCT_PLAYER, guild_id, player_id, GMsg_LocateAvatarAck::MAX_PLAYERS);


  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get House member locations; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  int num_players = mysql_num_rows(res);

  locate_msg.Init(num_players);

  for (int i = 0; i<num_players; i++) {
	  row = mysql_fetch_row(res);
	  locate_msg.SetPlayerName(i, row[0]);
	  level_id = (lyra_id_t)(ATOI(row[1]));
	  room_id = (lyra_id_t)(ATOI(row[2]));
	  int status = GMsg_LocateAvatarAck::LOCATE_FOUND;
	  // player hidden?
	  bool hidden = false;
	  if (level_id > Lyra::HIDDEN_DELTA) {
		  hidden = true;
		  level_id -= Lyra::HIDDEN_DELTA;
	  }
	  // if hidden, reset location, unless player is GM
	  if (hidden) {
		  if (gm)
			  status = GMsg_LocateAvatarAck::LOCATE_FOUND_HIDDEN;
		  else {
			  status = GMsg_LocateAvatarAck::LOCATE_HIDDEN;
			  level_id = room_id = Lyra::ID_UNKNOWN;
		  }
	  }
	  locate_msg.SetStatus(i, status);
	  locate_msg.SetLocation(i, level_id, room_id);
  }

  mysql_free_result(res);
  return 0;
}


////
// CheckPassword
////

int LmPlayerDBC::CheckPassword(lyra_id_t player_id, const MD5Hash_t* phash, const TCHAR* challenge)
{
  DEFMETHOD(LmPlayerDBC, CheckPassword);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT password FROM player WHERE player_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check password for player %u; mysql error %s"), method, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

  TCHAR db_password[Lyra::PASSWORD_MAX];

 _tcscpy(db_password, row[0]);

  //int l1 =_tcslen(db_password);
  //int l2 =_tcslen(password);

  //int r1 =_tcscmp(db_password, password);
  //int r2 =_tcsnccmp(db_password, password, l1);

  //  LOG_Debug(_T("%s: comparing passwords %s, %s"), db_password, password);

  mysql_free_result(res);

  // DEPRECATED: send password clear text, verify with string compare
//  if (_tcscmp(db_password, password) == 0)
//	return 1;
//  else
//    	return 0;

  // NEW: use password as shared secret to get md5 checksum from a challenge
  //

  	LnMD5 md5;
	MD5Hash_t server_hash;

	md5.Update((void*)challenge, _tcslen(challenge));
	md5.Update((void*)db_password, _tcslen(db_password));
	md5.Final(server_hash);
	if(0==memcmp(phash, &server_hash, sizeof(MD5Hash_t)))
	    return 1;
	else
	{

          LOG_Debug(_T("N Challenge length: %d text: %s"), _tcslen(challenge), challenge);
          LOG_Debug(_T("N Password length: %d text: %s"), _tcslen(db_password), db_password);
          LOG_Debug(_T("Incorrect hash! serverhash=%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d phash=%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"),
                       server_hash[0],server_hash[1],server_hash[2],server_hash[3],server_hash[4],server_hash[5],server_hash[6],
                       server_hash[7],server_hash[8],server_hash[9],server_hash[10],server_hash[11],server_hash[12],
                       server_hash[13],server_hash[14],server_hash[15],
                       *phash[0],*phash[1],*phash[2],*phash[3],*phash[4],*phash[5],*phash[6],
                       *phash[7],*phash[8],*phash[9],*phash[10],*phash[11],*phash[12],
                    *phash[13],*phash[14],*phash[15]);

		return 0;
	}

}

////
// SaveGuildRanks
////

int LmPlayerDBC::SaveGuildRanks(lyra_id_t player_id, LmStats& stats)
{
  DEFMETHOD(LmPlayerDBC, SaveGuildRanks);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

  MYSQL_RES *res;
  MYSQL_ROW row;
  int new_rank, new_pool, old_rank, old_pool;

  // Now save the player guild info

  for (int i = 0; i < NUM_GUILDS; i++) {
    new_rank = stats.GuildRank(i);
    new_pool = stats.PoolXP(i);

   _stprintf(query, _T("SELECT rank, xp_pool_curr FROM guildplayer WHERE player_id = %u AND guild_id = %u"), player_id, i);

    ////timer.Start();
    int error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error) {
      LOG_Error(_T("%s: Could not select rank %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

    res = mysql_store_result(&m_mysql);
    int num_rows = mysql_num_rows(res);

    if (!num_rows)
      continue;

    row = mysql_fetch_row(res);

    if (row[0])
    old_rank = ATOI(row[0]);
    if (row[1])
      old_pool = ATOI(row[1]);

    mysql_free_result(res);

    if ((new_rank != old_rank) || (new_pool != old_pool))
      { // update only if rank or pool has changed
	//_tprintf(_T("updating guild rank!\n"));
	if (new_rank > 0) {
	 _stprintf(query, _T("UPDATE guildplayer  SET rank = %u, xp_pool_curr = %u WHERE player_id = %u AND guild_id = %u;"), new_rank, new_pool, player_id, i);
	} else {
	 _stprintf(query, _T("DELETE FROM guildplayer WHERE player_id = %u AND guild_id = %u"), player_id, i);
	}

	////timer.Start();
	error = mysql_query(&m_mysql, query);
	////timer.Stop();

	if (error)
	  {
	    LOG_Error(_T("%s: Could not update guild rank for guild %u for player %u; mysql error %s"), method, i, player_id, mysql_error(&m_mysql));
	    return MYSQL_ERROR;
	  }
      }

  }

  return 0;
}


////
// UnGhost
////

int LmPlayerDBC::UnGhost(TCHAR* gamed_ip, unsigned int gamed_port, LmGlobalDB* globaldb_)
{
  DEFMETHOD(LmPlayerDBC, UnGhost);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

  MYSQL_RES *res;
  MYSQL_ROW row;
  lyra_id_t player_id;

  TCHAR escaped_ip[64];
  int error;

  mysql_escape_string((TCHAR*)escaped_ip,  gamed_ip,_tcslen(gamed_ip));

  // now check the database for a list of players who were logged into
  // the crashed game, and if local state files exist, parse them
  // and write out the information to the database. if port = 0,
  // it means that we are staring the servers, and so we need to recover
  // all players with a local state file

  if (gamed_port > 0)
  {
	 _stprintf(query, _T("SELECT player_id FROM player WHERE gamed_ip = '%s' AND gamed_port = %u"), escaped_ip, gamed_port);

	  ////timer.Start();
	  error = mysql_query(&m_mysql, query);
	  ////timer.Stop();

	  if (error) {
		 _stprintf(query, _T("Unghosting error: %s"),  mysql_error(&m_mysql));
		  return MYSQL_ERROR;
	  }

	  res = mysql_store_result(&m_mysql);
	  int num_rows = mysql_num_rows(res);

	  if (!num_rows)
	  {
		  mysql_free_result(res);
		  return 0;
	  }

	  for (int i=0; i<num_rows; i++) {
		  row = mysql_fetch_row(res);
		  player_id = ATOI(row[0]);
		  this->PutLocalFileInDB(player_id, globaldb_);
	  }
  } else {
#ifndef WIN32
	  // enumerate all existing state files on local machine,
	  // extract player IDs from file name, and write state files
	  // back to database
	  TCHAR dirName[FILENAME_MAX];
	  TCHAR playerID[FILENAME_MAX];
	  globaldb_->GetPlayerDir(dirName);
	  DIR* pDir = opendir(dirName);
	  if (!pDir)
		  return 0;
	  dirent* pDirEnt;
	  TCHAR* pUnderscore;
	  TCHAR* pDot;
	  while ((pDirEnt = readdir(pDir)) != NULL) {
		  // file name in pDirEnt->d_name is of the form
		  // player_xxx.db, where xxx is the player id.
		  // we need to extract this
		pUnderscore =_tcschr(pDirEnt->d_name, '_');
		if (!pUnderscore)
			continue;
		pDot =_tcschr(pDirEnt->d_name, '.');
	_tcsnccpy(playerID, pUnderscore+1, (pDot - pUnderscore -1));
		lyra_id_t player_id = ATOI(playerID);
		this->PutLocalFileInDB(player_id, globaldb_);
	  }


	  closedir(pDir);
#endif
  }


  // Now reset the logged_in field for all affected players

  if (gamed_port > 0)
	 _stprintf(query, _T("UPDATE player SET logged_in = 0 WHERE gamed_ip = '%s' AND gamed_port = %u"),
	  escaped_ip, gamed_port);
  else
	 _stprintf(query, _T("UPDATE player SET logged_in = 0 WHERE gamed_ip = '%s'"), escaped_ip);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
	 _stprintf(query, _T("Unghosting error: %s"),  mysql_error(&m_mysql));
	  return MYSQL_ERROR;
  }

  return 0;
}

////
// PutLocalFileInDB
////

int LmPlayerDBC::PutLocalFileInDB(lyra_id_t player_id, LmGlobalDB* globaldb_)
{
  DEFMETHOD(LmPlayerDBC, UnGhost);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR pfile[FILENAME_MAX];
  LmDatabase db;
  LmPlayerDB db_;

  globaldb_->GetPlayerFile(pfile, player_id);

  if (db.Open(pfile, GDBM_READER) < 0) {
	  _tunlink(pfile);
	  return -1;
  }
  if (LmPlayerDBF::LoadFromFile(db, db_) < 0) {
	  db.Close();
	  _tunlink(pfile);
	  return -1;
  }
  db.Close();

  if (SavePlayer(db_, true) < 0) {
	  return -1;
  }
  _tunlink(pfile);

  return 0;
}

int LmPlayerDBC::GrantPP(lyra_id_t granter, lyra_id_t grantee, TCHAR* why, int acct_type)
{
  DEFMETHOD(LmPlayerDBC, GrantPP);
  LmLocker mon(lock_); // lock object for method duration

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

  int error;

  /*CREATE TABLE pp_grant(
 granter_id                     INT UNSIGNED            NOT NULL,
 receiver_id                    INT UNSIGNED            NOT NULL,
 grant_date                     DATE                    NOT NULL,
 reason                         VARCHAR(255)               NULL
);
  );*/

  // check the database to be sure we didn't already try to grant pp's to the same
  // dreamer in the last 7 days

  //_stprintf(query, _T("SELECT count(*) FROM pp_grant WHERE granter_id = %d AND receiver_id = %d AND TO_DAYS(grant_date) > TO_DAYS(CURDATE())-30;"),
	//  granter, grantee);
  if (acct_type != LmPlayerDB::ACCT_ADMIN) {
    _stprintf(query, _T("SELECT count(*) FROM pp_grant WHERE granter_id = %d AND receiver_id = %d AND TO_DAYS(grant_date) > TO_DAYS(CURDATE())-7;"),
	    granter, grantee);


    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error) {
	    _stprintf(query, _T("GrantPP error: %s"),  mysql_error(&m_mysql));
	    return GMsg_PPointAck::UNKNOWN_ERR;
    }

    res = mysql_store_result(&m_mysql);

    int count = 0;

    row = mysql_fetch_row(res);

    if (row[0])
      count = ATOI(row[0]);

    mysql_free_result(res);

    if (count > 0)
    {
	    return GMsg_PPointAck::GRANT_REPEAT;
    }
  }

  // insert record, signal grant is OK

  TCHAR escaped_why[GMsg_GrantPPoint::MAX_WHY*2];
  mysql_escape_string((TCHAR*)escaped_why, why, _tcslen(why));

  _stprintf(query, _T("INSERT INTO pp_grant VALUES(%u,%u,CURDATE(),'%s')"),
  	  granter, grantee, escaped_why);

  LOG_Debug(_T("PPoint Grant: from %u to %u, reason = %s\n"), granter, grantee, why);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
	  _stprintf(query, _T("GrantPP error: %s"),  mysql_error(&m_mysql));
  	 return GMsg_PPointAck::UNKNOWN_ERR;
  }

  // everything is good! don't forget to decrement the pp_pool....
  return GMsg_PPointAck::GRANT_OK;

}

////
// Usepp
////

int LmPlayerDBC::UsePP(lyra_id_t player_id, int cost, int how, int var1, int var2, int var3)
{
  DEFMETHOD(LmPlayerDBC, UsePP);
  LmLocker mon(lock_); // lock object for method duration

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int error;

  TCHAR howstr[GMsg_GrantPPoint::MAX_WHY];
  TCHAR escaped_how[GMsg_GrantPPoint::MAX_WHY*2];

  switch (how) {
  case GMsg_UsePPoint::STAT_INCREASE:
	  _stprintf(howstr, _T("PPoint: raised state %d by %d, cost = %d"), var1, 1, cost);
	  break;
#if 0
  case GMsg_UsePPoint::BYPASS_TRAIN:
	  _stprintf(howstr, _T("PPoint: learned art %d to level %d, cost = %d"), var2, var3, cost);
	  break;
  case GMsg_UsePPoint::BYPASS_SPHERE:
	  _stprintf(howstr, _T("PPoint: bypassed sphere requirement, cost = %d"), cost);
	  break;
#endif
  case GMsg_UsePPoint::USE_ART:
	  _stprintf(howstr, _T("PPoint used an art %d I don't know at level %d, cost = %d"), var2, var3, cost);
	  break;
  case GMsg_UsePPoint::GAIN_XP:
	  _stprintf(howstr, _T("PPoints used to gain XP; cost = %d"), cost);
	  break;
  case GMsg_UsePPoint::BUY_PMARE_TIME:
	_stprintf(howstr, _T("PPoints used to buy %d mare credits, cost = %d"), var1, cost);
	break;
  default:
	  _stprintf(howstr, _T("Unknown grant reason! cost = %d how = %d, var1=%d, var2=%d, var3=%d\n"), cost, how, var1, var2, var3);
	  break;
  }

  LOG_Debug(howstr);

  mysql_escape_string((TCHAR*)escaped_how, howstr, _tcslen(howstr));


  /*CREATE TABLE pp_used(
 pp_id          INT UNSIGNED AUTO_INCREMENT NOT NULL PRIMARY KEY,
 player_id                       INT UNSIGNED           NOT NULL,
 date                            DATE                   NOT NULL,
 cost                            INT UNSIGNED  DEFAULT 0    NOT NULL,
 reason                          VARCHAR(255)              NULL
	*/

  // insert record

  _stprintf(query, _T("INSERT INTO pp_used VALUES(NULL, %u,CURDATE(), %u, %s)"),
	  player_id, cost, escaped_how);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
	  _stprintf(query, _T("GrantPP error: %s"),  mysql_error(&m_mysql));
	  return GMsg_PPointAck::UNKNOWN_ERR;
  }

  // everything is good! don't forget to decrement the pp_pool....
  return GMsg_PPointAck::GRANT_OK;

}

////
// LogQuest - pairs a training log with the used Quest item into a database log - Alex 10/22/2014
////
int LmPlayerDBC::LogQuest(lyra_id_t origin_id, lyra_id_t target_id, int art, int skill)
{
  DEFMETHOD(LmPlayerDBC, LogQuest);
  LmLocker mon(lock_); // lock object for method duration

  TCHAR query[512];

 _stprintf(query, _T("INSERT INTO ul_player.trainlog (item_name, item_descrip, origin_id, target_id, art_id, skill, created_time) SELECT item_name, item_descrip, %u, %u, %u, %u, created_time FROM ul_item.quest_active WHERE art_id = %u AND target_id = %u AND owner_id = %u"), origin_id, target_id, art, skill, art, target_id, origin_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not log train and quest for player %u training player %u in art %u to %u; mysql error %s"), method, origin_id, target_id, art, skill, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }


   return 0;
}
