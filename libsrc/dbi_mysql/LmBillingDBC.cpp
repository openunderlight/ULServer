// LmBillingDBC.pc  -*- C++ -*-
// $Id: LmBillingDBC.pc,v 1.17 1998-05-01 17:04:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <string.h>
#include <stdlib.h>

#include "LmPlayerDB.h"
#include "GMsg_LoginAck.h"
#include "LmLocker.h"
#include "LmLog.h"
#include "LmTimer.h"
#include "LmFuncTimer.h"
#include "LmRand.h"
#include "LmRoomDB.h"
#include "LmSockAddrInet.h"
#include "LmBillingDBC.h"

#ifndef USE_INLINE
#include "LmBillingDBC.i"
#endif /* !USE_INLINE */

inline unsigned int ATOI(char* value)
{
  if (!value)
    return 0;
  else
    return ((unsigned int)(atof(value)));
}

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

LmBillingDBC::LmBillingDBC(const TCHAR *dbuser, const TCHAR* dbpassword, const TCHAR* dbserver, int dbport)
  : connected_(false),
    log_(0),
    num_calls_(0),
    last_sql_code_(0),
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
 _tcscpy(dbname_, _T("ul3d_billing"));
 _tcscpy(username_, _T("ul3d_billing"));
#else
 _tcscpy(username_, _T("ul_billing"));
 _tcscpy(dbname_, _T("ul_billing"));
#endif

}

////
// Destructor
////

LmBillingDBC::~LmBillingDBC()
{
  Disconnect();
}


////
// Dump
////

void LmBillingDBC::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmBillingDBC[%p,%d]:>\n"), this, sizeof(LmBillingDBC));
  INDENT(indent + 1, f);
}



////
// Connect
////

int LmBillingDBC::Connect()
{
 
  DEFMETHOD(LmBillingDBC, Connect);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (connected_) {
    // don't connect twice (disconnect, perhaps?)
    return 0; // fake successful result
  }

  ////timer.Start();

  if (!mysql_init(&mysql_))
    {
      LOG_Error(_T("%s: MYSQL ul_billing init error %s\n"), method, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&mysql_, db_server_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))
  //if (!mysql_real_connect(&mysql_, db_server_, _T("ul_billing"), password_, _T("ul_billing"), db_port_, _T("/var/lib/mysql/mysql.sock"), 0))
    {
      LOG_Error(_T("%s: MYSQL connect error %s\n"), method, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  ////timer.Stop();

  connected_ = true;
  return 0;
}

////
// Disconnect
////

void LmBillingDBC::Disconnect()
{
 
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  if (!connected_) {
    return;
  }

  ////timer.Start();
  mysql_close(&mysql_);
  ////timer.Stop();

  connected_ = false;
}


////
// GetBillingStatus
////

int LmBillingDBC::GetBillingStatus(lyra_id_t player_id, int acct_type, 
	int pmare_type, short* max_minutes_online, short* session_minutes, 
	unsigned char* gamesite, lyra_id_t* gamesite_id)
{
  DEFMETHOD(LmBillingDBC, GetBillingStatus);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[1024];
  lyra_id_t billing_id;

  MYSQL_RES *res;
  MYSQL_ROW row;
  short billing_minutes, minutes, other_session_minutes;
  float total_charges = 0.0f; 
  bool first_block = false;
  float cash, pmare_credit, block_rate, minute_rate;
  unsigned int i_total_charges, i_cash, i_pmare_credit, i_block_rate, i_minute_rate;
  bool artificial_limit = false;

  // note that we convert all float prices to int's in pennies to 
  // avoid a myriad of rounding issues working with floats


  // default max time online = 0 (infinite)
  *max_minutes_online = 0;

  // for pmares, also check the account itself
  //if (acct_type == LmPlayerDB::ACCT_PMARE) 
    // uncomment this to set ***PMARE BILLING DISABLED***
   // return GMsg_LoginAck::LOGIN_OK;

  _stprintf(query, _T("SELECT status, (TO_DAYS(paid_date) - TO_DAYS(CURDATE())), billing_id FROM underlight WHERE underlight_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check billing status for player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&mysql_);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return GMsg_LoginAck::LOGIN_NO_BILLING;
  }

  row = mysql_fetch_row(res);

  billing_id = ATOI(row[2]);

  if (_tcscmp(_T("ACTIVE"),row[0]) != 0)
  {
    mysql_free_result(res);
    return GMsg_LoginAck::LOGIN_NO_BILLING;
  }

  // for ACTIVE accounts, count the # of days before expiration
  if (!row[1])
  {
	  mysql_free_result(res);
      return GMsg_LoginAck::LOGIN_NO_BILLING;
  }

  int num_days = ATOI(row[1]);
  mysql_free_result(res);

  if ((num_days < 0) && (acct_type != LmPlayerDB::ACCT_PMARE))
  {
    return GMsg_LoginAck::LOGIN_NO_BILLING;
  }

  // for pmares, also check the account itself
  if (acct_type == LmPlayerDB::ACCT_PMARE) {

	  // first, check to see how many session minutes are already accrued
	  // for today. we need to check for ALL starting types, in case they
	  // have multiple sessions
	  
	  for (int j = Avatars::BOGROM; j< Avatars::HORRON; j++) {
		  
		  _stprintf(query, _T("SELECT SUM(minutes) FROM pmare_logins WHERE underlight_id = %u AND status = 'UNAGGREGATED' AND avatar = %d"), player_id, j);
		  error = mysql_query(&mysql_, query);
		  
		  if (error)
		  {
			  LOG_Error(_T("%s: Could not check billing account status for pmare player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
			  return MYSQL_ERROR;
		  }
		  
		  res = mysql_store_result(&mysql_);
		  if (!mysql_num_rows(res))
		  {
			  mysql_free_result(res);
			  other_session_minutes = 0;
		  } else {
			  row = mysql_fetch_row(res);
			  other_session_minutes = (short)(ATOI(row[0]));
			  mysql_free_result(res);
		  }
		  billing_minutes = other_session_minutes;

		// for the current pmare type, we want to allow them to play
		// out their 15 minute block, even if they are out of cash
		  if (j == pmare_type) {
			  *session_minutes = other_session_minutes;
			  // finishing the current block won't increase the charge
			  // if it's the first block played
			  if (billing_minutes < 15) {
				  first_block = true;
				  billing_minutes = 0;
			  } 
		  } else if ((billing_minutes > 0) && (billing_minutes < 15))
			  billing_minutes = 15;
		  
		  if (j == Avatars::BOGROM)
			total_charges += ((float)billing_minutes)*BOG_PRICE/15;
		  else if (j == Avatars::AGOKNIGHT)
			total_charges += ((float)billing_minutes)*AGO_PRICE/15;
		  else if (j == Avatars::SHAMBLIX)
			total_charges += ((float)billing_minutes)*SHAM_PRICE/15;
		  i_total_charges = (int)(100*total_charges);

	  }

	if (i_total_charges >= 4000)
		return GMsg_LoginAck::LOGIN_MAX_PMARE;
  
   _stprintf(query, _T("SELECT status, cash_credit, pmare_only_credit FROM accounts WHERE billing_id = %u"), billing_id);
    error = mysql_query(&mysql_, query);
    
    if (error)
      {
	LOG_Error(_T("%s: Could not check billing account status for pmare player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
	return MYSQL_ERROR;
      }
    
    res = mysql_store_result(&mysql_);
    if (!mysql_num_rows(res))
	{
		mysql_free_result(res);
		return GMsg_LoginAck::LOGIN_NO_PMARE;
    }
  
    row = mysql_fetch_row(res);

	// else check cash balance
	cash = atof(row[1]);
	i_cash = (int)(100*cash); 

	pmare_credit = atof(row[2]);
	i_pmare_credit = (int)(100*pmare_credit);
	if (i_pmare_credit > 1) {
		cash += pmare_credit;
		i_cash += i_pmare_credit;
	}

	// if using a credit card, or have > $40 in cash, 
	// set the limit to be $40 per day
    if ((_tcscmp(_T("CREDIT"),row[0]) == 0) ||
		(i_cash > 4000)) {
      i_cash = 4000;
	  cash = 40.0f;
	  artificial_limit = true;
	}

	if (i_cash <= i_total_charges)
	{
	    mysql_free_result(res);
		return GMsg_LoginAck::LOGIN_NO_PMARE;
	}

	 mysql_free_result(res);

	if (i_cash <= i_total_charges)
		return GMsg_LoginAck::LOGIN_MAX_PMARE;

	cash -= total_charges;
	i_cash -= i_total_charges; // subtract away existing charges
	
	block_rate = BOG_PRICE;
	if (pmare_type == Avatars::AGOKNIGHT)
		block_rate = AGO_PRICE;
	else if (pmare_type == Avatars::SHAMBLIX)
		block_rate = SHAM_PRICE;
	i_block_rate = (int)(100*block_rate);
	minute_rate = block_rate/15;
	i_minute_rate = (int)(100*minute_rate);

	if (first_block && (0 == *session_minutes) && (i_cash < i_block_rate))
		return GMsg_LoginAck::LOGIN_NO_PMARE;

	// total_charges is used here as a temporary variable
	// don't ask about the casting!
	//total_charges = (float)((int)(i_cash/i_minute_rate)); 
	total_charges = (float)(cash/minute_rate); 

	if (first_block)
//		minutes = (short)(i_cash/i_minute_rate - *session_minutes);
		minutes = (short)(cash/minute_rate - *session_minutes);
	else 
//		minutes = (short)(i_cash/i_minute_rate);
		minutes = (short)(cash/minute_rate);

	*max_minutes_online = minutes;

	if (0 == minutes) {
		if (artificial_limit)
			return GMsg_LoginAck::LOGIN_MAX_PMARE;
		else
			return GMsg_LoginAck::LOGIN_NO_PMARE;
	}
			
  }
  // now look up the game service partner and ID

  if (0 == billing_id) {
	    *gamesite = 0; // game service
	    *gamesite_id = 0; // game service ID
  } else {

  _stprintf(query, _T("SELECT game_service, game_service_id FROM accounts WHERE billing_id = %u"), billing_id);

  ////timer.Start();
  error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("%s: Could not check billing game partner status for player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }
  
  res = mysql_store_result(&mysql_);

  if (!mysql_num_rows(res)) {
    mysql_free_result(res);
    return GMsg_LoginAck::LOGIN_NO_BILLING;
  }

  row = mysql_fetch_row(res);

  *gamesite = row[0][0]; // game service
  *gamesite_id = ATOI(row[1]); // game service ID

//  LOG_Debug(_T("%s: game site %d, id %d; mysql error %s"), method, gamesite, gamesite_id, mysql_error(&mysql_));


  mysql_free_result(res);

  }

  return GMsg_LoginAck::LOGIN_OK;

}

#if 0 // deprecated 11/03 BMP
bool LmBillingDBC::CanRedeemToken(lyra_id_t player_id, int maturity_days)
{
  DEFMETHOD(LmBillingDBC, CanRedeemToken);

  TCHAR query[256];
  MYSQL_RES *res;
  MYSQL_ROW row;
  
  _stprintf(query, _T("SELECT status, (TO_DAYS(CURDATE())-TO_DAYS('2000-01-01')) FROM underlight WHERE underlight_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not check status on redemption token created by player %u, %d maturity; mysql error %s"), 
		  method, player_id, maturity_days, mysql_error(&mysql_));
      return false;
    }

  res = mysql_store_result(&mysql_);
  row = mysql_fetch_row(res);
  bool active = false;
  if (0 == _tcscmp(row[0], _T("ACTIVE")))
	  active = true;
  int curr_days = ATOI(row[1]);
  mysql_free_result(res);
  if (curr_days < maturity_days)  // token not mature yet
	return false;
  if (!active)
	  return false; // account not active

  return true;

}
#endif

////
// LogoutPMare
////

int LmBillingDBC::LogoutPMare(lyra_id_t player_id, unsigned int num_seconds_online, int pmare_type, lyra_id_t billing_id)
{
  DEFMETHOD(LmBillingDBC, GetBillingStatus);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  // no charge for super short sessions
  if (num_seconds_online < 15)
		return 1;

  TCHAR query[1024];

#if 0 // disable code to directly generate pmare bills
	  // instead, we now just write an entry to the pmare_bills table,
	  // and let nightlyupdate.pl aggregate them into pmare bills

  MYSQL_RES *res;
  MYSQL_ROW row;
  int adjusted_seconds_online = num_seconds_online;

  // minimum time online for billing is 60 seconds
  if (adjusted_seconds_online < 60)
	  adjusted_seconds_online = 60;
 

  int blocks = (int)(adjusted_seconds_online/900);
  int extra_minutes = (int)((adjusted_seconds_online%900)/60);
  
  float block_rate = (float)0.95111;
  if (pmare_type == Avatars::AGOKNIGHT)
	  block_rate = AGO_RATE
  else if (pmare_type == Avatars::SHAMBLIX)
	  block_rate = (float)3.95;
  float minute_rate = block_rate/15;
  
  float amount = (blocks * block_rate) + (extra_minutes * minute_rate);
  float cash = 0.0f;
  
  // no charge, no bill
  if (amount < 0.01f)
	  return 1;
  
  TCHAR bill_status[10] = _T("UNPAID");
  
  // determine if we should deduce from the cash_credit, or create an unpaid bill
  
  _stprintf(query, _T("SELECT cash_credit FROM accounts WHERE billing_id = %u"), billing_id);
  int error = mysql_query(&mysql_, query);
  
  if (error)
  {
	  LOG_Error(_T("%s: Could not check billing account cash_credit for pmare player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
	  return MYSQL_ERROR;
  }
  
  res = mysql_store_result(&mysql_);
  if (!mysql_num_rows(res))
  {
	  mysql_free_result(res);
  } else {
	  
	  row = mysql_fetch_row(res);
	  cash = atof(row[0]);
	  mysql_free_result(res);
	  if (cash > amount) { // debit cash on account
		  cash -= amount;
		  _stprintf(query, _T("UPDATE accounts SET cash_credit =%.2f WHERE billing_id = %u"), cash, billing_id);
		  error = mysql_query(&mysql_, query);
		  
		  if (error)
		  {
			  LOG_Error(_T("%s: Could not update billing account cash_credit for pmare player %u, amount %.2f; mysql error %s"), method, player_id, amount, mysql_error(&mysql_));
			  return MYSQL_ERROR;
		  } else {
			_tcscpy(bill_status, _T("PAID"));
		  }
	  }
  }
  
  _stprintf(query, _T("INSERT INTO bills VALUES (NULL, 10, %u, %u, CURDATE(), %.2f, '%s')"), billing_id, player_id, amount, bill_status);

#endif

  if (num_seconds_online < 10)
	  return 1;

  int minutes_online = (int)((num_seconds_online + 50)/60);

  TCHAR bill_status[20] = _T("UNAGGREGATED");
    
  _stprintf(query, _T("INSERT INTO pmare_logins VALUES (NULL, %u, %u, CURDATE(), %d, %d, '%s')"), 
	  billing_id, player_id, minutes_online, pmare_type, bill_status);

  ////timer.Start()
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();
  
  if (error)
  {
      LOG_Error(_T("%s: Could not create bill for player mare session; player %u, billing id %u, seconds online %u; mysql error %s"), method, player_id, billing_id, num_seconds_online, mysql_error(&mysql_));
      return MYSQL_ERROR;
  }
  
#if 0 // this logging is unnecessary, because it's all in pmare_bills
  TCHAR descrip[128];
  TCHAR escaped_descrip[164];
  _stprintf(descrip, _T("PMare session, type = %u, duration = %u seconds (%u blocks)"), pmare_type, num_seconds_online, blocks);
  mysql_escape_string((TCHAR*)escaped_descrip, descrip,_tcslen(descrip));
  
  _stprintf(query, _T("INSERT INTO billing_log VALUES (NULL, %u, %u, CURDATE(), '%s')"), billing_id, player_id, escaped_descrip);
  
  ////timer.Start();
  error = mysql_query(&mysql_, query);
  ////timer.Stop();
  
  if (error)
  {
      LOG_Error(_T("%s: Could not create billing log entry for player mare session; player %u, billing id %u, seconds online %u; mysql error %s"), method, player_id, billing_id, num_seconds_online, mysql_error(&mysql_));
      return MYSQL_ERROR;
  }
#endif
  
  return 1;

}


////
// DisablePlayer
////

int LmBillingDBC::DisablePlayer(lyra_id_t player_id)
{
  DEFMETHOD(LmBillingDBC, DisablePlayer);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_);

  TCHAR query[256];

 _stprintf(query, _T("UPDATE underlight SET status = 'DISABLED' WHERE underlight_id = %u"), player_id);

  ////timer.Start();
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();
  
  if (error)
    {
      LOG_Error(_T("%s: Could not lock out player %u; mysql error %s"), method, player_id, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  return 0;
}


