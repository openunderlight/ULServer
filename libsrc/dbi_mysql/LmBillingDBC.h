// LmBillingDBC.h  -*- C++ -*-
// $Id: LmBillingDBC.h,v 1.9 1998-02-20 20:25:45-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item database connection class

#ifndef INCLUDED_LmBillingDBC
#define INCLUDED_LmBillingDBC

#include <stdio.h>
#include "LyraDefs.h"
#include "mysql.h"
#include "PThMutex.h"
#include "SharedConstants.h"

// local types

// forward declarations

class LmLog;

// the class

class LmBillingDBC {

public:

  enum {
    
    // return codes
    MYSQL_ERROR = -1 // bad things, man
  };

public:

  LmBillingDBC(const TCHAR *dbuser, const TCHAR *dbpassword, const TCHAR* dbserver, int dbport);
  ~LmBillingDBC();

  int Connect();
  void Disconnect();


  // database selectors 
  int GetBillingStatus(lyra_id_t player_id, int acct_type, int pmare_type, 
	  short* max_minutes_online, short* session_minutes,
	  unsigned char* gamesite, lyra_id_t* gamesite_id);  
  //bool CanRedeemToken(lyra_id_t player_id, int maturity_days);

  // database mutators
  int LogoutPMare(lyra_id_t player_id, unsigned int num_seconds_online, 
	  int pmare_type, lyra_id_t billing_id);
  int DisablePlayer(lyra_id_t player_id);
  int AddPMareCredit(lyra_id_t player_id, unsigned int amount);
  long LastSQLCode() const;
  void SetLog(LmLog* log);
  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void Dump(FILE* f, int indent = 0) const;

  static const TCHAR* ProgramName(int servertype);

protected:

  LmLog* Log() const;

  int room_index(lyra_id_t roomid) const;

private:

  // not implemented
  LmBillingDBC(const LmBillingDBC&);
  //operator=(const LmBillingDBC&);

  // mysql object
  MYSQL mysql_;

  bool connected_;
  PThMutex lock_;
  long last_sql_code_;
  LmLog* log_;

  TCHAR username_[20]; 
  TCHAR password_[20];
  TCHAR db_server_[80];
  TCHAR dbname_[20];
  int db_port_;
  
  int num_calls_;          // total number of calls to class
  unsigned long last_ms_;  // time of last call (milliseconds)
  unsigned long num_ms_;   // total time of all calls
  unsigned long sql_ms_;   // total time spent in "EXEC SQL" blocks

};

#ifdef USE_INLINE
#include "LmBillingDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmBillingDBC */
