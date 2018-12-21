// LmGuildDBC.h  -*- C++ -*-
// $Id: LmGuildDBC.h,v 1.15 1998-02-04 19:03:32-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// guild database connection class

#ifndef INCLUDED_LmGuildDBC
#define INCLUDED_LmGuildDBC

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"
#include "mysql.h"
#include "PThMutex.h"
#include "GMsg_All.h"
#include "../../libsrc/shared/SharedConstants.h"

// local types

typedef TCHAR lyra_guild_sum[64];
typedef TCHAR lyra_guild_msg[2048];
typedef TCHAR lyra_guild_rep[1024];

// forward declarations

class LmLog;
class LmGoalInfo;
class LmIdSet;

// the class

class LmGuildDBC {

public:

  enum {
    // return codes
    MYSQL_ERROR = -1, // bad things, man
    MYSQL_NODATA = -2 // empty result
  };

public:

  LmGuildDBC(const TCHAR *dbuser, const TCHAR *dbpassword, const TCHAR* dbserver, int dbport);
  ~LmGuildDBC();

  int Connect();
  void Disconnect();

  // database mutators
  int PostMission(lyra_id_t creator, GMsg_PostGoal& msg);
  int PostGoal(lyra_id_t creator, GMsg_PostGoal& msg);
  int UpdateMission(lyra_id_t creator, const GMsg_PostGoal& msg);
  int PostReport(lyra_id_t creator, lyra_id_t recipient, const GMsg_PostReport& msg);
  int SetReportFlags(lyra_id_t report, int flags);
  int AcceptGoal(lyra_id_t acceptee, const GMsg_Goal& msg);
  int RemoveGoal(lyra_id_t acceptee, const GMsg_Goal& msg);
  int ExpireGoal(lyra_id_t expiror, const GMsg_Goal& msg);
  int VoteGoal(lyra_id_t voter, int vote, const GMsg_Goal& msg);  
  int RemoveReportXP(lyra_id_t reportid);
  int CompleteGoal(lyra_id_t completor, const GMsg_Goal& msg);
  int CompleteQuest(lyra_id_t completor, lyra_id_t quest_id);
  int DeleteReport(lyra_id_t expiror, const GMsg_Goal& msg);
  int GetNumberAccepted(lyra_id_t goalid);

  // database selectors
  int GetGoalInfo(lyra_id_t goalid, LmGoalInfo& goalinfo);
  int InAcceptees(lyra_id_t goalid, lyra_id_t requestor);
  int InVoters(lyra_id_t goalid, lyra_id_t requestor);
  int GetGoalText(lyra_id_t goalid, GMsg_RcvGoalText& msg);
  int GetGoalDetails(lyra_id_t goalid, GMsg_RcvGoalDetails& msg);
  int GetReportText(lyra_id_t reportid, GMsg_RcvReportText& msg);
  int GetGoalbookHeader(lyra_id_t goalid, GMsg_RcvGoalbookHdr& msg);
  int GoalHeaders(const GMsg_GetGoalHdrs& msg, int sphere, int stat, lyra_id_t* goal_ids,  lyra_guild_sum* summaries, lyra_id_t player_id, const LmIdSet& goalbook);
  int HirankGoalHeaders(const GMsg_GetGoalHdrs& msg, lyra_id_t requestor, lyra_id_t* goal_ids, lyra_guild_sum* summaries, unsigned int* status, int hirank);
  int ReportHeaders(const GMsg_GetReportHdrs& msg, lyra_id_t requestor, lyra_id_t* report_ids, lyra_id_t* goal_ids, lyra_guild_sum *summaries, unsigned short *flags, int hirank = 0);
  int DetailGoalHeaders(int guild, int rank, int last_seen, lyra_id_t requestor, lyra_id_t* goal_ids, int hirank);
  int HasAcceptedQuest(lyra_id_t player_id, lyra_id_t quest_id);

  int NumCalls() const;
  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void SetLog(LmLog* log);

  long LastSQLCode() const;

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmLog* Log() const;

private: 

  // mysql object
  MYSQL m_mysql;

  // not implemented
  LmGuildDBC(const LmGuildDBC&);
  //operator=(const LmGuildDBC&);

  int connect();
  void disconnect();

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
#include "LmGuildDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmGuildDBC */
