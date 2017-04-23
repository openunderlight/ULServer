// LmGuildDBC.pc  -*- C++ -*-
// $Id: LmGuildDBC.pc,v 1.20 1998-02-12 15:32:31-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LmGuildDBC.h"
#include "LmLocker.h"
#include "LmLog.h"
#include "LmPlayerDB.h"
#include "GMsg_All.h"
#include "LmGoalInfo.h"
#include "LmTimer.h"
#include "LmFuncTimer.h"

#ifndef USE_INLINE
#include "LmGuildDBC.i"
#endif /* !USE_INLINE */

inline unsigned int ATOI(char* value)
{
  if (!value)
    return 0;
  else
    return ((unsigned int)(atof(value)));
}

// check connection macro
#define CHECK_CONNECT   if (!connected_ && (connect() < 0)) { return MYSQL_ERROR; }
// #define CHECK_CONNECT   /* nothing */

// some log macros (from LmThread.h)
#define LOG_Log       if (Log()) Log()->Log
#define LOG_Warning   if (Log()) Log()->Warning
#define LOG_Debug     if (Log()) Log()->Debug
#define LOG_Error     if (Log()) Log()->Error
#define LOG_Security  if (Log()) Log()->Security

//#define LOG_Log       printf
//#define LOG_Warning   printf
//#define LOG_Debug     printf
//#define LOG_Error     printf
//#define LOG_Security  printf

////
// Constructor
////

LmGuildDBC::LmGuildDBC(const TCHAR *dbuser, const TCHAR* dbpassword, const TCHAR* dbserver, int dbport)
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
 _tcscpy(db_server_, dbserver);
#ifdef Ul3D
 _tcscpy(dbname_, _T("ul3d_guild"));
 _tcscpy(username_, _T("ul3d_guild"));
#else
 _tcscpy(dbname_, _T("ul_guild"));
 _tcscpy(username_, _T("ul_guild"));
#endif

}

////
// Destructor
////

LmGuildDBC::~LmGuildDBC()
{
  Disconnect();
}

////
// Dump
////

void LmGuildDBC::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmGuildDBC[%p,%d] connected=%d calls=%d time=%lu/%lu last=%lu>\n"), this, sizeof(LmGuildDBC),
	  connected_, num_calls_, sql_ms_, num_ms_, last_ms_);
}



////
// Connect
////

int LmGuildDBC::Connect()
{
  // static const TCHAR* method = "LmGuildDBC::Connect";
  LmLocker mon(lock_); // lock object for method duration
  if (connected_) {
    // don't connect twice
    return 0; // return success
  }

  return this->connect();

}

////
// connect -- non-locking helper method, does not check connected_ flag
////

int LmGuildDBC::connect()
{
  DEFMETHOD(LmGuildDBC, connect);
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  ////timer.Start();
  if (!mysql_init(&m_mysql))
    {
      LOG_Error(_T("%s: MYSQL init error %s\n"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&m_mysql, db_server_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))    {
  //if (!mysql_real_connect(&m_mysql, db_server_, _T("ul_guild"), password_, _T("ul_guild"), db_port_, _T("/var/lib/mysql/mysql.sock"), 0))    {
    LOG_Error(_T("%s: MYSQL ul_guild connect error %s\n"), method, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  ////timer.Stop();
  connected_ = true;

  return 0;
}

////
// Disconnect
////

void LmGuildDBC::Disconnect()
{
  LmLocker mon(lock_); // lock object for method duration
  if (!connected_) {
    return;
  }
  disconnect();
}

////
// disconnect - non-locking, does not check connected_ flag
////

void LmGuildDBC::disconnect()
{
  DEFMETHOD(LmGuildDBC, disconnect);
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (!connected_) {
    return;
  }

  ////timer.Start();
  mysql_close(&m_mysql);
  ////timer.Stop();

  connected_ = false;
}


////
// GetGoalInfo
////

int LmGuildDBC::GetGoalInfo(lyra_id_t goalid, LmGoalInfo& goalinfo)
{
  DEFMETHOD(LmGuildDBC, GetGoalInfo);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT (TO_DAYS(expiration_time) - TO_DAYS(CURDATE())), guild, rank, max_accepts, number_accepted, status, creator, (TO_DAYS(vote_expiration_time) - TO_DAYS(CURDATE())) , yes_votes, no_votes, flags FROM goal WHERE goal_id = %u"), goalid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goal info goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return MYSQL_NODATA; // no record present
  }

  row = mysql_fetch_row(res);

  goalinfo.Init();
  goalinfo.SetGoalID(goalid);
  int value = 0;
  if (row[0] != NULL)    
    value = ATOI(row[0]);
  goalinfo.SetExpirationTime(value);
  goalinfo.SetGuild(ATOI(row[1]));
  goalinfo.SetRank(ATOI(row[2]));
  goalinfo.SetMaxAccepts(ATOI(row[3]));
  goalinfo.SetNumAccepted(ATOI(row[4]));
  goalinfo.SetStatus(ATOI(row[5]));
  goalinfo.SetCreatorID(ATOI(row[6]));
  value = 0;
  if (row[7] != NULL)    
    value = ATOI(row[7]);
  goalinfo.SetVoteExpiration(value);
  goalinfo.SetFlags(ATOI(row[10]));
  goalinfo.SetNumberNo(ATOI(row[9]));
  goalinfo.SetNumberYes(ATOI(row[8]));
  mysql_free_result(res);

  goalinfo.SetNumAccepted(GetNumberAccepted(goalid));

 
  return 0;

}

////
// GetNumberAccepted - returns the number of players who have accepted a goal
////

int LmGuildDBC::GetNumberAccepted(lyra_id_t goalid)
{
  DEFMETHOD(LmGuildDBC, GetNumberAccepted);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT COUNT(*) FROM accept WHERE goal_id = %d"), 
	 goalid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goal info goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  int number_accepted = 0;

  if (mysql_num_rows(res) > 0) {
	row = mysql_fetch_row(res);
	number_accepted = ATOI(row[0]);
    mysql_free_result(res);
  }

  return number_accepted;
}


////
// InAcceptees - returns true if requestor has accepted given goal, false if not; return < 0
//   in the event of any database error
////

int LmGuildDBC::InAcceptees(lyra_id_t goalid, lyra_id_t requestor)
{
  DEFMETHOD(LmGuildDBC, InAcceptees);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;

 _stprintf(query, _T("SELECT player_id FROM accept WHERE goal_id = %u AND player_id = %u"), goalid, requestor);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check goal acceptance for player %u; mysql error %s"), method, requestor, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  int retval  = 1;

  if (!mysql_num_rows(res)) 
    retval = 0;

  mysql_free_result(res);
    
  return retval;
 
}

////
// InVoters - returns true if requestor has voted on given goal, false if not; return < 0
//   in the event of any database error
////

int LmGuildDBC::InVoters(lyra_id_t goalid, lyra_id_t requestor)
{
  DEFMETHOD(LmGuildDBC, InVoters);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;

 _stprintf(query, _T("SELECT player_id FROM vote WHERE goal_id = %u AND player_id = %u"), goalid, requestor);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check in voters for player %u; mysql error %s"), method, requestor, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  int retval  = 1;

  if (!mysql_num_rows(res)) 
    retval = 0;

  mysql_free_result(res);
    
  return retval;
   
}


////
// PostMission
////

int LmGuildDBC::PostMission(lyra_id_t creator, GMsg_PostGoal& msg)
{
  DEFMETHOD(LmGuildDBC, PostMission);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  // be sure we have big enough buffers to work with
  TCHAR query[Lyra::MAX_GOAL_LENGTH + 2*Lyra::GOAL_SUMMARY_LENGTH + 1024];
  TCHAR escaped_summary[Lyra::GOAL_SUMMARY_LENGTH*2+1];
  TCHAR escaped_keywords[Lyra::QUEST_KEYWORDS_LENGTH*2+1];
  TCHAR escaped_text[Lyra::MAX_GOAL_LENGTH + 512];
  //TCHAR* escaped_summary = LmNEW(TCHAR[_tcslen(msg.Summary())*2+1]);
  //int goal_len =_tcslen(msg.GoalText());
  //TCHAR* escaped_text = LmNEW(TCHAR[goal_len*2+1]);

  mysql_escape_string(escaped_summary, msg.Summary(),_tcslen(msg.Summary()));
  mysql_escape_string(escaped_text, msg.GoalText(),_tcslen(msg.GoalText()));
  mysql_escape_string(escaped_keywords, msg.Keywords(),_tcslen(msg.Keywords()));

  int max_accepts = msg.MaxAccepted();
  if (max_accepts> Lyra::MAX_ACCEPTS) 
    max_accepts = Lyra::MAX_ACCEPTS;

  _stprintf(query, _T("INSERT INTO goal (goal_id, creation_time, expiration_time, creator, rank, guild, sug_sphere, sug_stat, summary, message, max_accepts, status, vote_expiration_time, flags, graphic, charges, color1, color2, item_type, field1, field2, field3, quest_xp, keywords)  VALUES (NULL, CURDATE(), DATE_ADD(CURDATE(), INTERVAL %u DAY), %u, %u, %u, %u, %u, '%s', '%s', %u, %u, CURDATE(), %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, '%s')"),
	 msg.ExpirationTime(), creator, msg.Level(), msg.Guild(),  msg.SugSphere(), msg.SugStat(), escaped_summary, escaped_text, max_accepts, Guild::GOAL_ACTIVE, msg.Flags(), 
	 msg.Graphic(), msg.Charges(), msg.Color1(), msg.Color2(), msg.ItemType(), msg.Field1(), msg.Field2(), msg.Field3(), msg.QuestXP(), msg.Keywords());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  //LmDELETEARRAY(escaped_summary);
  //LmDELETEARRAY(escaped_text);
  
  if (error) {
    LOG_Error(_T("%s: Could not post mission for player %u ; mysql error %s"), method, creator, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  unsigned int goal_id = (unsigned int)mysql_insert_id(&m_mysql);

  if (!goal_id)
    return MYSQL_ERROR; 

  msg.SetGoalID(goal_id);

  return 0; 
}

////
// PostGoal
////

int LmGuildDBC::PostGoal(lyra_id_t creator, GMsg_PostGoal& msg)
{
  DEFMETHOD(LmGuildDBC, PostGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[Lyra::MAX_GOAL_LENGTH + 2*Lyra::GOAL_SUMMARY_LENGTH + 1024];
  TCHAR escaped_summary[Lyra::GOAL_SUMMARY_LENGTH*2+1];
  TCHAR escaped_text[Lyra::MAX_GOAL_LENGTH + 512];
  
  int status;

  if (msg.SugSphere() == Guild::RULER_ISSUE) {
    status = Guild::GOAL_RULER_VOTE;
  }
  else {
    status = Guild::GOAL_PENDING_VOTE;
  }

  int max_accepts = msg.MaxAccepted();
  if (max_accepts> Lyra::MAX_ACCEPTS) 
    max_accepts = Lyra::MAX_ACCEPTS;

//  TCHAR* escaped_summary = LmNEW(TCHAR[_tcslen(msg.Summary())*2+1]);
//  TCHAR* escaped_text = LmNEW(TCHAR[_tcslen(msg.GoalText())*2+1]);

  mysql_escape_string(escaped_summary, msg.Summary(),_tcslen(msg.Summary()));
  mysql_escape_string(escaped_text, msg.GoalText(),_tcslen(msg.GoalText()));

 _stprintf(query, _T("INSERT INTO goal (goal_id, creation_time, expiration_time, creator, rank, guild, sug_sphere, sug_stat, summary, message, max_accepts, status, vote_expiration_time)  VALUES (NULL, CURDATE(), DATE_ADD(CURDATE(), INTERVAL %u DAY), %u, %u, %u, %u, %u, '%s', '%s', %u, %u, DATE_ADD(CURDATE(), INTERVAL %u DAY))"), msg.ExpirationTime(), creator, msg.Level(), msg.Guild(),  msg.SugSphere(), msg.SugStat(), escaped_summary, escaped_text, max_accepts, status, Guild::VOTE_EXPIRATION_DAYS);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  //LmDELETEARRAY(escaped_summary);
  //LmDELETEARRAY(escaped_text);
  
  if (error) {
    LOG_Error(_T("%s: Could not post goal  for player %u ; mysql error %s"), method, creator, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  unsigned int goal_id = (unsigned int)mysql_insert_id(&m_mysql);

  if (!goal_id)
    return MYSQL_ERROR; 

  msg.SetGoalID(goal_id);

  return 0; 

}

////
// UpdateMission
////

int LmGuildDBC::UpdateMission(lyra_id_t creator, const GMsg_PostGoal& msg)
{
  DEFMETHOD(LmGuildDBC, UpdateMission);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[Lyra::MAX_GOAL_LENGTH + 2*Lyra::GOAL_SUMMARY_LENGTH + 1024];
  TCHAR escaped_summary[Lyra::GOAL_SUMMARY_LENGTH*2+1];
  TCHAR escaped_text[Lyra::MAX_GOAL_LENGTH + 512];

  int max_accepts = msg.MaxAccepted();
  if (max_accepts> Lyra::MAX_ACCEPTS) 
    max_accepts = Lyra::MAX_ACCEPTS;
 
//  TCHAR* escaped_summary = LmNEW(TCHAR[_tcslen(msg.Summary())*2+1]);
//  TCHAR* escaped_text = LmNEW(TCHAR[_tcslen(msg.GoalText())*2+1]);

  mysql_escape_string(escaped_summary, msg.Summary(),_tcslen(msg.Summary()));
  mysql_escape_string(escaped_text, msg.GoalText(),_tcslen(msg.GoalText()));
  
 _stprintf(query, _T("UPDATE goal SET expiration_time = DATE_ADD(CURDATE(), INTERVAL %u DAY), sug_sphere = %u, sug_stat = %u, summary = '%s', message = '%s', max_accepts = %u, status = %d, flags = %u WHERE goal_id = %u"), msg.ExpirationTime(), msg.SugSphere(), msg.SugStat(), escaped_summary, escaped_text, max_accepts, Guild::GOAL_ACTIVE, msg.Flags(), msg.GoalID());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  //LmDELETEARRAY(escaped_summary);
  //LmDELETEARRAY(escaped_text);
    
  if (error) {
    LOG_Error(_T("%s: Could not update mission for goal %d; mysql error %s"), method, msg.GoalID(), mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0; 
}


////
// PostReport
////

int LmGuildDBC::PostReport(lyra_id_t creator, lyra_id_t recipient, const GMsg_PostReport& msg)
{
  DEFMETHOD(LmGuildDBC, PostReport);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[Lyra::MAX_REPORT_LENGTH + 2*Lyra::GOAL_SUMMARY_LENGTH + 1024];
  TCHAR escaped_summary[Lyra::GOAL_SUMMARY_LENGTH*2+1];
  TCHAR escaped_text[Lyra::MAX_REPORT_LENGTH + 512];

//  TCHAR* escaped_summary = LmNEW(TCHAR[_tcslen(msg.Summary())*2+1]);
  //TCHAR* escaped_text = LmNEW(TCHAR[_tcslen(msg.ReportText())*2+1]);

  mysql_escape_string(escaped_summary, msg.Summary(),_tcslen(msg.Summary()));
  mysql_escape_string(escaped_text, msg.ReportText(),_tcslen(msg.ReportText()));
  
 _stprintf(query, _T("INSERT INTO report (report_id, goal_id, creation_time, creator, recipient, summary, message, xp_award) VALUES (NULL, %u, CURDATE(), %u, %u, '%s', '%s', %u)"),  msg.GoalID(), creator, recipient, escaped_summary, escaped_text, msg.AwardXP());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  //LmDELETEARRAY(escaped_summary);
  //LmDELETEARRAY(escaped_text);

  
  if (error) {
    LOG_Error(_T("%s: Could not post report for player %u; mysql error %s"), method, creator, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  unsigned int report_id = (unsigned int)mysql_insert_id(&m_mysql);

  if (!report_id)
    return MYSQL_ERROR; 

  //  msg.SetReportID(report_id);

  return 0;

}


////
// SetReportFlags
////

int LmGuildDBC::SetReportFlags(lyra_id_t report, int flags)
{
  DEFMETHOD(LmGuildDBC, SetReportFlags);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

 _stprintf(query, _T("UPDATE report SET flags = %u WHERE report_id = %u"), flags, report);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not update report id %u ; mysql error %s"), method, report, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;

}


////
// AcceptGoal
////

int LmGuildDBC::AcceptGoal(lyra_id_t acceptee, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, AcceptGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

  // delete existing entries for this player and this goal
  if (this->RemoveGoal(acceptee, msg) < 0)
    return MYSQL_ERROR;

 _stprintf(query, _T("INSERT INTO accept (goal_id, player_id) VALUES (%u, %u)"), msg.ID(), acceptee); 

  int error = mysql_query(&m_mysql, query);
  
  if (error) {
    LOG_Error(_T("%s: Could not add goal %u acceptance for player %u ; mysql error %s"), method, msg.ID(), acceptee, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

//   _stprintf(query, _T("UPDATE goal SET number_accepted = number_accepted + 1 WHERE goal_id = %u"), msg.ID());

//  error = mysql_query(&m_mysql, query);
  
//  if (error) {
//   LOG_Error(_T("%s: Could not update number of acceptees for goal %ue for player %u ; mysql error %s"), method, msg.ID(), acceptee, mysql_error(&m_mysql));
//   return MYSQL_ERROR;
//  }

  return 0;


}


////
// RemoveGoal
////

int LmGuildDBC::RemoveGoal(lyra_id_t acceptee, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, RemoveGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  MYSQL_RES *res;
  MYSQL_ROW row;
  TCHAR query[256];

    // first, pull all the goal info, and log it
  _stprintf(query, _T("SELECT * FROM goal WHERE goal_id = %u"), msg.ID());
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
    LOG_Error(_T("%s: Could not load info for goal %d for deletion! mysql error %s"), method, msg.ID(), mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // nothing to do - it doesn't exist
  }

  row = mysql_fetch_row(res);

  LOG_Log(_T("Accept deleted: goal id %u, player id %u"), 
	 	ATOI(row[0]), ATOI(row[1]));
 
  mysql_free_result(res);

  _stprintf(query, _T("DELETE FROM accept WHERE goal_id = %u AND player_id = %u"), msg.ID(), acceptee); 

  error = mysql_query(&m_mysql, query);
  
  if (error) {
    LOG_Error(_T("%s: Could not remove goal %u acceptance for player %u ; mysql error %s"), method, msg.ID(), acceptee, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

//  _stprintf(query, _T("UPDATE goal SET number_accepted = number_accepted -1 WHERE goal_id = %u"), msg.ID()); 

//  error = mysql_query(&m_mysql, query);
  
//  if (error) {
  //  LOG_Error(_T("%s: Could not modify # of acceptees for goal %u ; mysql error %s"), method, msg.ID(), mysql_error(&m_mysql));
//    return MYSQL_ERROR;
//  }

  return 0;
}


////
// ExpireGoal
////

int LmGuildDBC::ExpireGoal(lyra_id_t expiror, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, ExpireGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function

  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  MYSQL_RES *res;
  MYSQL_ROW row;
  TCHAR query[256];

      // first, pull all the goal info, and log it
  _stprintf(query, _T("SELECT * FROM goal WHERE goal_id = %u"), msg.ID());
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
    LOG_Error(_T("%s: Could not load info for goal %d for deletion! mysql error %s"), method, msg.ID(), mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // nothing to do - it doesn't exist
  }

  row = mysql_fetch_row(res);

  LOG_Log(_T("Goal expired: goal id %u, creation time %s, expire time %s,\
	  creator %u, rank %u, guild %u, sug sphere %u, sug stat %u,\
	  max accepts %u, number_accepted %u, status %u, vote expiration time %s,\
	  yes votes %u, no votes %u, flags %u, summary %s, message %s"),
	 	ATOI(row[0]), row[1], row [2], 
		ATOI(row[3]), ATOI(row[4]), ATOI(row[5]), ATOI(row[6]), ATOI(row[7]),
		ATOI(row[10]), ATOI(row[11]), ATOI(row[12]), row[13], 
		ATOI(row[14]), ATOI(row[15]), ATOI(row[16]), row[8], row[9]);

  mysql_free_result(res);

  // now nuke it
 _stprintf(query, _T("DELETE FROM goal WHERE goal_id = %u"), msg.ID()); 
  
  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not expire goal %u by %u ; mysql error %s"), method, msg.ID(), expiror, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;
}


////
// DeleteReport
////

int LmGuildDBC::DeleteReport(lyra_id_t expiror, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, DeleteReport);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  MYSQL_RES *res;
  MYSQL_ROW row;
  TCHAR query[256];

  // first, pull all the goal info, and log it
  _stprintf(query, _T("SELECT * FROM report WHERE report_id = %u"), msg.ID());
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error) {
    LOG_Error(_T("%s: Could not load info for report %d, by %d, for deletion! mysql error %s"), method, msg.ID(), expiror, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // nothing to do - it doesn't exist
  }

  row = mysql_fetch_row(res);

  LOG_Log(_T("Report deleted: report id %u, goal id %u, creation time %s,\
	  creator %u, recipient %u, xp_award %u, flags, %u, summary %s, message %s"),
		ATOI(row[0]), ATOI(row[1]), row[2], ATOI(row[3]), ATOI(row[4]),
		ATOI(row[7]), ATOI(row[8]), row[5], row[6]);

  mysql_free_result(res);

  // now, nuke it
  _stprintf(query, _T("DELETE FROM report WHERE report_id = %u AND (recipient = %u OR creator = %u)"), msg.ID(), expiror, expiror); 

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not delete report %d by %d; mysql error %s"), method, msg.ID(), expiror, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;
}


////
// VoteGoal
////

int LmGuildDBC::VoteGoal(lyra_id_t voter, int vote, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, VoteGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

 _stprintf(query, _T("INSERT INTO vote (goal_id, player_id, vote) VALUES (%u, %u, %u)"), msg.ID(), voter, vote);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not vote goal for %s ; mysql error %s"), method, voter, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  if (vote == 0)
   _stprintf(query, _T("UPDATE goal SET no_votes = no_votes + 1 WHERE goal_id = %u"), msg.ID());
  else
   _stprintf(query, _T("UPDATE goal SET yes_votes = yes_votes + 1 WHERE goal_id = %u"), msg.ID());

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not update goal record for voter %u ; mysql error %s"), method, voter, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  
  return 0;
}

////
// RemoveXP
////

int LmGuildDBC::RemoveReportXP(lyra_id_t reportid)
{
  DEFMETHOD(LmGuildDBC, RemoveReportXP);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

 _stprintf(query, _T("UPDATE report SET xp_award = 0 WHERE report_id = %u"), reportid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not remove report xp for report %u; mysql error %s"), method, reportid, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;
   
}

////
// Complete Goal
////

int LmGuildDBC::CompleteGoal(lyra_id_t completor, const GMsg_Goal& msg)
{
  DEFMETHOD(LmGuildDBC, CompleteGoal);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

 _stprintf(query, _T("UPDATE goal SET status = 1 WHERE goal_id = %u AND creator = %u AND status < 4"), msg.ID(), completor);
  
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not complete goal %u for %u ; mysql error %s"), method, msg.ID(), completor, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;
}

////
// Complete Quest
////

int LmGuildDBC::CompleteQuest(lyra_id_t completor, lyra_id_t quest_id)
{
  DEFMETHOD(LmGuildDBC, CompleteQuest);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];

 _stprintf(query, _T("REPLACE INTO finished_quest (goal_id, player_id) VALUES (%u,%u)"), quest_id, completor);
  
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error) {
    LOG_Error(_T("%s: Could not complete quest %u for %u ; mysql error %s"), method, quest_id, completor, mysql_error(&m_mysql));
    return MYSQL_ERROR;
  }

  return 0;
}




////
// GetGoalText
////

int LmGuildDBC::GetGoalText(lyra_id_t goalid, GMsg_RcvGoalText& msg)
{
  DEFMETHOD(LmGuildDBC, GetGoalText);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT sug_sphere, sug_stat, creator, message FROM goal WHERE goal_id = %u"), goalid);
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goal text for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

  msg.SetGoalID(goalid);
  msg.SetSugSphere(ATOI(row[0]));
  msg.SetSugStat(ATOI(row[1]));
  msg.SetCreatorID(ATOI(row[2]));
  msg.SetGoalText(row[3]);

  mysql_free_result(res);
  
  return 0;
}


////
// GetGoalDetails
////

int LmGuildDBC::GetGoalDetails(lyra_id_t goalid, GMsg_RcvGoalDetails& msg)
{
  DEFMETHOD(LmGuildDBC, GetGoalDetails);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  unsigned int num_votes, num_rows, status;

 _stprintf(query, _T("SELECT TO_DAYS(expiration_time)-TO_DAYS(CURDATE()), rank, max_accepts, number_accepted, status, TO_DAYS(vote_expiration_time)-TO_DAYS(CURDATE()), yes_votes, no_votes, flags, charges, color1, color2, item_type, field1, field2, field3, quest_xp, keywords, graphic FROM goal WHERE goal_id = %u"), goalid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goal details for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return MYSQL_NODATA; // no record present
  }

  row = mysql_fetch_row(res);
  
  // NOTE: this is incorrect and should be ignored!
  //  num_accepted = ATOI(row[3]);
	 
  status = ATOI(row[4]);

  msg.SetGoalID(goalid);
  msg.SetMaxAcceptances(ATOI(row[2]));
  msg.SetLevel(ATOI(row[1]));
  int value = 0;
  if (row[0] != NULL)    
    value = ATOI(row[0]);
  msg.SetExpirationTime(value);
  value = 0;
  if (row[5] != NULL)    
    value = ATOI(row[5]);
  msg.SetVoteExpiration(value);
  msg.SetStatusFlags(status);
  //  msg.SetNumAcceptees(num_accepted);
  msg.SetOtherFlags(ATOI(row[8]));
  msg.SetNumberYes(ATOI(row[6]));
  msg.SetNumberNo(ATOI(row[7]));

  num_votes = ATOI(row[7]) + ATOI(row[6]);

  msg.SetCharges(ATOI(row[9]));
  msg.SetColor1(ATOI(row[10]));
  msg.SetColor2(ATOI(row[11]));
  msg.SetItemType(ATOI(row[12]));
  msg.SetField1(ATOI(row[13]));
  msg.SetField2(ATOI(row[14]));
  msg.SetField3(ATOI(row[15]));
  msg.SetQuestXP(ATOI(row[16]));
  msg.SetKeywords(row[17]);
  msg.SetGraphic(ATOI(row[18]));

  mysql_free_result(res);

  // if there are no acceptees, we're done
  //  if (((num_accepted == 0) && (status < Guild::GOAL_PENDING_VOTE)) ||
  if ((num_votes == 0) && (status >= Guild::GOAL_PENDING_VOTE))
    {
      return 0;
    }

  // special bonus section -- check acceptees
  
  if (status < Guild::GOAL_PENDING_VOTE)
   _stprintf(query, _T("SELECT DISTINCT player_id FROM accept WHERE goal_id = %u"), goalid);
  else
   _stprintf(query, _T("SELECT DISTINCT player_id FROM vote WHERE goal_id = %u"), goalid);

  // _tprintf(_T("%s\n"), query);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not get accepting player ids for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);
  
  num_rows = (unsigned int)mysql_num_rows(res);
  msg.SetNumAcceptees(num_rows);

  unsigned int j=0;
  for (j=0; j<num_rows; j++) {
    row = mysql_fetch_row(res);
    msg.SetAccepteeID(j, ATOI(row[0]));
  }
  
  mysql_free_result(res);

  // special bonus section -- check completees for quests

  int num_acceptees = num_rows;
  if (msg.Level() != Guild::QUEST)
	  return 0;

  // add completees into acceptees section, up to the maximum allowed

  // special bonus section -- check acceptees

  int max_values  = Lyra::MAX_ACCEPTS - num_acceptees; // completees and acceptees share the same field; don't overflow it!
  
   _stprintf(query, _T("SELECT DISTINCT player_id FROM finished_quest WHERE goal_id = %u LIMIT %u"), goalid, max_values);

  // _tprintf(_T("%s\n"), query);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not get completed player ids for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);
  
  num_rows = (unsigned int)mysql_num_rows(res); // num_completees
  msg.SetNumCompletees(num_rows);

  for (j=num_acceptees; j<num_acceptees+num_rows; j++) {
    row = mysql_fetch_row(res);
    msg.SetAccepteeID(j, ATOI(row[0]));
  }
  
  mysql_free_result(res);



  return 0;
}

////
// GetReportText
////

int LmGuildDBC::GetReportText(lyra_id_t reportid, GMsg_RcvReportText& msg)
{
  DEFMETHOD(LmGuildDBC, GetReportText);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT goal_id, creator, message, recipient, xp_award, flags FROM report WHERE report_id = %u"), reportid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get report text for report %u; mysql error %s"), method, reportid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return 0; // no record present
  }

  row = mysql_fetch_row(res);

  msg.SetReportID(reportid);
  msg.SetGoalID(ATOI(row[0]));
  msg.SetAwardXP(ATOI(row[4]));
  msg.SetFlags(ATOI(row[5]));
  msg.SetCreatorID(ATOI(row[1])); // gameserver will fill in real name
  msg.SetRecipientID(ATOI(row[3])); // ditto
  msg.SetReportText((row[2]));

  mysql_free_result(res);
  
  return 0;
   
}

////
// GetGoalbookHeader
////

int LmGuildDBC::GetGoalbookHeader(lyra_id_t goalid, GMsg_RcvGoalbookHdr& msg)
{
  DEFMETHOD(LmGuildDBC, GetGoalbookHeader);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT guild, rank, summary FROM goal WHERE goal_id = %u"), goalid);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goalbook header for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return MYSQL_NODATA; // no record present - app expects error code!
  }

  row = mysql_fetch_row(res);

  msg.SetGoalID(goalid);
  msg.SetGuild(ATOI(row[0]));
  msg.SetRank(ATOI(row[1]));
  msg.SetSummary(row[2]); 
  
  mysql_free_result(res);
  
  return 0;
   
}


////
// GoalHeaders
////

int LmGuildDBC::GoalHeaders(const GMsg_GetGoalHdrs& msg, int sphere, int stat,
			    lyra_id_t* goal_ids, lyra_guild_sum* summaries, lyra_id_t player_id,
				const LmIdSet& goalbook)
{
  DEFMETHOD(LmGuildDBC, GetGoalHeaders);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[1024];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int num_rows;

    // note - status field used to need to be zero
  if (msg.LevelNum() == Guild::KNIGHT)
    _stprintf(query, _T("SELECT goal_id, summary  FROM goal WHERE guild = %u AND rank = %u AND (sug_sphere <= %u OR sug_sphere = %u ) AND (sug_stat = %u OR sug_stat = %u) AND (TO_DAYS(vote_expiration_time) <= TO_DAYS(CURDATE()))  AND yes_votes > no_votes ORDER BY goal_id"), msg.Guild(), msg.LevelNum(), sphere, Guild::SPHERE_ANY, Stats::NO_STAT, stat);
  else if (msg.LevelNum() == Guild::QUEST) {

  // we need to be able to retrieve ALL quest headers that we posted, even if we don't qualify!
  // also, we need to be able to retrieve all quest headers that are in our goalbook, even
  // if the goal is marked completed!

	// first, fill a buffer with SQL to match against the goals in our goalbook
	  TCHAR gbookquery[512] = _T("");
	  TCHAR buffer[64];
	  TCHAR buffer2[64];
	  for (int k=0; k<goalbook.Size(); k++)
	  {
		_stprintf(buffer, _T("OR (status = 1 AND goal_id = %d)"), goalbook.Member(k));
		_tcscat(gbookquery, buffer);
	  }
	  // we only want codex quests (denoted by charges = 0) available to sphere 0, 1
	  // dreamers
	  if (sphere < 2)
		 _stprintf(buffer2, _T(""));
	  else
		  _stprintf(buffer2, _T("AND charges > 0"));
	  _stprintf(query, _T("SELECT goal_id, summary FROM goal WHERE (((rank = %u AND guild = %u AND (sug_sphere <= %u OR sug_sphere = %u ) AND (sug_stat = %u OR sug_stat = %u) %s) OR (creator = %d AND rank = %d)) AND (TO_DAYS(expiration_time) > TO_DAYS(CURDATE())) AND status = 0) %s ORDER BY creation_time"), 
		  msg.LevelNum(), msg.Guild(), sphere, Guild::SPHERE_ANY, Stats::NO_STAT, stat, buffer2, player_id, msg.LevelNum(), gbookquery);
  }
  else 
	_stprintf(query, _T("SELECT goal_id, summary FROM goal WHERE guild = %u AND rank = %u AND (sug_sphere <= %u OR sug_sphere = %u ) AND (sug_stat = %u OR sug_stat = %u) AND (TO_DAYS(expiration_time) > TO_DAYS(CURDATE())) AND status = 0 ORDER BY creation_time"), msg.Guild(), msg.LevelNum(), sphere, Guild::SPHERE_ANY, Stats::NO_STAT, stat);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get goal headers; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  num_rows = (unsigned int)mysql_num_rows(res);

//  if (num_rows > 512)
//    num_rows = 512;

  int j = 0;

  if (msg.LastGoal())
    j = -1;

  for (int i = 0; i< num_rows; i++) {
    row = mysql_fetch_row(res);
    if (j >= 0)
      {
	goal_ids[j] = ATOI(row[0]);
    _tcscpy(summaries[j], row[1]);
	j++;
	if (j == 10)
	  break;
      }
    else if (ATOI(row[0]) == msg.LastGoal())
      j = 0;
  }

  if (j == -1) // no new ones found
    j = 0;

  mysql_free_result(res);
  
  return j;
}

////
// HirankGoalHeaders
////

int LmGuildDBC::HirankGoalHeaders(const GMsg_GetGoalHdrs& msg, lyra_id_t requestor,  lyra_id_t* goal_ids, lyra_guild_sum* summaries,   unsigned int* status, int hirank)
{
  DEFMETHOD(LmGuildDBC, HirankGoalHeaders);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int num_rows;

  //  if (msg.LevelNum() == Guild::KNIGHT) // ruler, get everything
   _stprintf(query, _T("SELECT goal_id, summary, status FROM goal WHERE guild = %u AND rank = %u ORDER BY goal_id"), msg.Guild(), msg.LevelNum(), requestor, requestor);
    //  else  //getting initiate missions
    //   _stprintf(query, _T("SELECT goal_id, summary, status FROM goal WHERE guild = %u AND rank = %u AND (creator = %u OR MOD(flags, 2) = 1) ORDER BY creation_time"), msg.Guild(), msg.LevelNum(), requestor);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not get Hirank goal headers; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  num_rows = (unsigned int)mysql_num_rows(res);

  //  if (num_rows > 512)
  //    num_rows = 512; 

  int j = 0;

  if (msg.LastGoal())
    j = -1;

  for (int i = 0; i< num_rows; i++) {
    row = mysql_fetch_row(res);
    if (j >= 0)
      {
	goal_ids[j] = ATOI(row[0]);
	status[j] = ATOI(row[2]);
_tcscpy(summaries[j], row[1]);
	j++;
	if (j == 10)
	  break;
      }
    else if (ATOI(row[0]) == msg.LastGoal())
      j = 0;
  }

  if (j == -1) // no new ones found
    j = 0;

  mysql_free_result(res);

  //  lyra_id_t dgh_goal_ids[10];
  //  this->DetailGoalHeaders(msg, requestor, dgh_goal_ids, hirank);

  return j;
}

////
// ReportHeaders
////

int LmGuildDBC::ReportHeaders(const GMsg_GetReportHdrs& msg, lyra_id_t requestor, lyra_id_t* report_ids, lyra_id_t* goal_ids, lyra_guild_sum* summaries, unsigned short* flags, int hirank)
{
  DEFMETHOD(LmGuildDBC, ReportHeaders);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[512];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int num_rows;

  int goalid = msg.GoalID();
  int guild = msg.Guild();
  int rank = msg.LevelNum();

  if (goalid == 0) { // get new reports 
    //    if (hirank == 2) // ruler
      //     _stprintf(query, _T("SELECT report.report_id, report.goal_id, report.summary, report.flags FROM report, goal WHERE goal.goal_id = report.goal_id AND goal.guild = %u AND goal.rank = %u AND MOD(report.flags,2) = 0 ORDER BY report.report_id"), guild, rank);
    //    else if (hirank == 1) // guardian looking for initiate reports
    if (hirank > 0) // guardian looking for initiate reports
     _stprintf(query, _T("SELECT report.report_id, report.goal_id, report.summary, report.flags FROM report, goal WHERE report.goal_id = goal.goal_id AND goal.guild = %u AND goal.rank = %u AND MOD(report.flags,2) = 0 AND (goal.creator = %u OR report.recipient = %u) ORDER BY report.creation_time DESC"), guild, rank, requestor, requestor);
    else // initiate, show only ours 
     _stprintf(query, _T("SELECT report.report_id, report.goal_id, report.summary, report.flags FROM report, goal WHERE report.goal_id = goal.goal_id AND goal.guild = %u AND goal.rank = %u AND MOD(report.flags,2) = 0 AND report.recipient = %u  ORDER BY creation_time DESC"),  guild, rank, requestor);

  } else { // select a single goal
      if (hirank > 0)
		_stprintf(query, _T("SELECT report_id, goal_id, summary, flags FROM report WHERE goal_id = %u ORDER BY report.creation_time"),  goalid);
      else _stprintf(query, _T("SELECT report_id, goal_id, summary, flags FROM report WHERE goal_id = %u  AND (report.recipient = %u OR report.creator = %u) ORDER BY report_id"),  goalid, requestor, requestor);
  }
 

 
////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not get report headers for goal %u; mysql error %s"), method, goalid, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  num_rows = (unsigned int)mysql_num_rows(res);

  //  if (num_rows > 512)
  //    num_rows = 512;

  int j = 0;

  if (msg.LastReport())
    j = -1;

  for (int i = 0; i< num_rows; i++) {
    row = mysql_fetch_row(res);
    if (j >= 0)
      {
	report_ids[j] = ATOI(row[0]);
	goal_ids[j] = ATOI(row[1]);
	_tcscpy(summaries[j], row[2]);
	flags[j] = ATOI(row[3]);
	j++;
	if (j == 10)
	  break;
      }
    else if (ATOI(row[0]) == msg.LastReport())
      j = 0;
  }

  if (j == -1) // no new ones found
    j = 0;

  mysql_free_result(res);
  
  return j;
}



////
// DetailGoalHeaders: returns the array of goals that the player can modify
////

int LmGuildDBC::DetailGoalHeaders(int guild, int rank, int last_seen, lyra_id_t requestor, lyra_id_t* goal_ids, int hirank)
{
  DEFMETHOD(LmGuildDBC, DetailGoalHeaders);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  int num_rows;

  if (hirank == 2) // ruler
   _stprintf(query, _T("SELECT goal_id FROM goal WHERE guild = %u AND rank = %u ORDER BY goal.goal_id"), guild, rank);
  else if (hirank == 1) // guardian looking for initiate reports
   _stprintf(query, _T("SELECT goal_id FROM goal WHERE guild = %u AND rank = %u  AND (creator = %u  OR MOD(goal.flags, 2) = 1) ORDER BY goal.goal_id"), guild, rank, requestor);
  else 
     _stprintf(query, _T("SELECT goal.goal_id FROM report, goal WHERE report.goal_id = goal.goal_id AND goal.guild = %u AND goal.rank = %u AND report.recipient = %u ORDER BY goal.goal_id;"),  guild, rank, requestor);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not get detail goal headers; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  num_rows = (unsigned int)mysql_num_rows(res);

  //  if (num_rows > 10)
  //  num_rows = 512; // artificial limit 

  int j = 0;
  if (last_seen)
    j = -1;

  for (int i = 0; i< num_rows; i++) {
    row = mysql_fetch_row(res);
    if (j >= 0) {
      goal_ids[j] = ATOI(row[0]);
      j++;
      if (j == 10)
	break;
    } else if (ATOI(row[0]) == last_seen)
      j = 0;
  }

  if (j == -1) // no new ones found
    j = 0;
  
  mysql_free_result(res);
  
  return j;
}


int LmGuildDBC::HasAcceptedQuest(lyra_id_t player_id, lyra_id_t quest_id)
{
  DEFMETHOD(LmGuildDBC, HasAcceptedQuest);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  CHECK_CONNECT;

  TCHAR query[256];
  MYSQL_RES *res;
  int num_rows;

   _stprintf(query, _T("SELECT * FROM finished_quest WHERE player_id= %u AND goal_id = %u"), 
	   player_id, quest_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not check if player %u has accepted goal %u; mysql error %s"), 
		  method, player_id, quest_id, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&m_mysql);

  num_rows = (unsigned int)mysql_num_rows(res);
  
  mysql_free_result(res);
  
  return num_rows;
}

