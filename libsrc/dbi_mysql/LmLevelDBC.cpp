// LmLevelDBC.pc  -*- C++ -*-
// $Id: LmLevelDBC.pc,v 1.17 1998-05-01 17:04:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <string.h>
#include <stdlib.h>

#include "LmLocker.h"
#include "LmLog.h"
#include "LmTimer.h"
#include "LmFuncTimer.h"
#include "LmRoomDB.h"
#include "LmLevelDBC.h"

#ifndef USE_INLINE
#include "LmLevelDBC.i"
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

// static class members
const TCHAR* LmLevelDBC::DEFAULT_LEVELNAME = _T("");

extern FILE* stdio;

////
// Constructor
////

LmLevelDBC::LmLevelDBC(const TCHAR *dbuser, const TCHAR* dbpassword, const TCHAR* dbserver, int dbport)
  : connected_(false),
    log_(0),
    num_calls_(0),
    last_sql_code_(0),
    num_ms_(0),
    sql_ms_(0),
    last_ms_(0),
    level_id_(DEFAULT_LEVELID),
    num_rooms_(0),
    rooms_(0),
	db_port_(dbport)
{
  lock_.Init();
  // copy to member variables
 _tcscpy(password_, dbpassword);
 _tcscpy(db_server_, dbserver); 
#ifdef Ul3D
 _tcscpy(dbname_, _T("ul3d_level"));
 _tcscpy(username_, _T("ul3d_level"));
#else
 _tcscpy(username_, _T("ul_level"));
 _tcscpy(dbname_, _T("ul_level"));
#endif
 _tcscpy(level_name_, DEFAULT_LEVELNAME);
  TRUNC(level_name_, sizeof(level_name_));
}

////
// Destructor
////

LmLevelDBC::~LmLevelDBC()
{
  Disconnect();
  LmDELETEARRAY(rooms_);
}

////
// Dump
////

void LmLevelDBC::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmLevelDBC[%p,%d] time=%lu/%lu last=%lu>\n"), this, sizeof(LmLevelDBC),
	  sql_ms_, num_ms_, last_ms_);
}

////
// Connect
////

int LmLevelDBC::Connect()
{

  DEFMETHOD(LmLevelDBC, Connect);
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
      LOG_Error(_T("%s: MYSQL ul_level init error %s\n"), method, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&mysql_, db_server_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))
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

void LmLevelDBC::Disconnect()
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

int LmLevelDBC::Load(lyra_id_t level_id, bool load_gens)
{
  DEFMETHOD(LmLevelDBC, Load);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  lyra_id_t room_id;
  lyra_id_t room_ids[MAX_NUMROOMS];
  level_id_ = level_id;
  int num_portals;
  int num_gens, i, j;

  TCHAR query[256];
  MYSQL_RES* res;
  MYSQL_ROW row;

 _stprintf(query, _T("SELECT name FROM levl WHERE level_id = %u"), level_id);

  ////timer.Start();
  int error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)   {
    LOG_Error(_T("Could not get level info for level %u; mysql error %s"), level_id, mysql_error(&mysql_));
    return MYSQL_ERROR;
  }

  res = mysql_store_result(&mysql_);

  row = mysql_fetch_row(res);

  if (!row) {
    mysql_free_result(res);
    LOG_Error(_T("Could not get level info for level %u; mysql error %s"), level_id, mysql_error(&mysql_));
    return MYSQL_ERROR;
  }

 _tcscpy(level_name_, row[0]);

  mysql_free_result(res);

  // now query to see how many rooms exist for this level
 _stprintf(query, _T("SELECT room_id FROM room WHERE level_id = %u"), level_id);

  ////timer.Start();
  error = mysql_query(&mysql_, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get room count for level %u; mysql error %s"), level_id, mysql_error(&mysql_));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&mysql_);

  num_rooms_ = mysql_num_rows(res);

  if (num_rooms_ > MAX_NUMROOMS) {
    num_rooms_ = 0;
  } else if (!num_rooms_) {
    mysql_free_result(res);
    return 0;
  }

  rooms_ = LmNEW(LmRoomDB[num_rooms_]);

  for (i=0; i<num_rooms_; i++) {
    row = mysql_fetch_row(res);
    room_ids[i] = ATOI(row[0]);
  }
  
  mysql_free_result(res);

  // now load all the rooms
  for (i=0; i<num_rooms_; i++) {
    room_id = room_ids[i];
    
   _stprintf(query, _T("SELECT name, start_point, max_players, no_reap FROM room WHERE level_id = %u AND room_id = %u"), level_id, room_id);

    ////timer.Start();
    int error = mysql_query(&mysql_, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("Could not get room info for room %u in level %u; mysql error %s"), room_id, level_id, mysql_error(&mysql_));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&mysql_);

    row = mysql_fetch_row(res);

    rooms_[i].Init(room_id, row[0], ATOI(row[1]), ATOI(row[2]), ATOI(row[3]));

    mysql_free_result(res);

  }

  // now go back and, for each room, load its portals and gens
  for (i=0; i<num_rooms_; i++) {

    room_id = rooms_[i].RoomID();
  
   _stprintf(query, _T("SELECT adj_level_id, adj_room_id FROM adj_room WHERE level_id = %u AND room_id = %u"), level_id, room_id);

    ////timer.Start();
    error = mysql_query(&mysql_, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("Could not get portal info for level %u; mysql error %s"), level_id, mysql_error(&mysql_));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&mysql_);

    num_portals = mysql_num_rows(res);

    rooms_[i].SetNumPortals(num_portals);

    for (j=0; j<num_portals; j++) {
      row = mysql_fetch_row(res);
      rooms_[i].InitPortal(j, ATOI(row[0]), ATOI(row[1]));
    }
    mysql_free_result(res);


    // compute maximum number of players
    max_players_ = 0;
    for (int i = 0; i < num_rooms_; ++i) {
      max_players_ += rooms_[i].MaxPlayers();
    }
    if (max_players_ > Lyra::MAX_LEVELPEOPLE) {
      // can't exceed this, no matter what
      max_players_ = Lyra::MAX_LEVELPEOPLE;
    }
    
    // now load the gens
    if (!load_gens) // game server doesn't care about gens
      continue;

   _stprintf(query, _T("SELECT delay_base, delay_var, live_base, live_var, x, y, z, angle, gen_type_id FROM item_gen WHERE level_id = %u AND room_id = %u"), level_id, room_id);

    ////timer.Start();
    error = mysql_query(&mysql_, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("Could not get item gen info for level %u, room %u; mysql error %s"), level_id, room_id, mysql_error(&mysql_));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&mysql_);

    num_gens = mysql_num_rows(res);

    rooms_[i].SetNumGens(num_gens);
    
    for (j=0; j<num_gens; j++) {
      row = mysql_fetch_row(res);
	  LmItemGenDB& gen = (LmItemGenDB&)(rooms_[i].Generator(j));
	  gen.Init(ATOI(row[0]), ATOI(row[1]), ATOI(row[2]), ATOI(row[3]), _ttoi(row[4]), _ttoi(row[5]), _ttoi(row[6]), ATOI(row[7]), ATOI(row[8]));
    }
    mysql_free_result(res);
  }


  // done
  return 0;
}

////
// RoomDescription
////

int LmLevelDBC::RoomDescription(short levelid, short roomid, TCHAR* description) const
{
	DEFMETHOD(LmLevelDBC, RoomDescription);
	LmLocker mon(lock_); // lock object for method duration
						 //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
						 ////LmTimer timer(&sql_ms_); // timer for SQL statements
	MYSQL_RES* res;
	MYSQL_ROW row;
	TCHAR query[256];

	_stprintf(query, _T("SELECT description FROM room_desc WHERE level_id = %u AND room_id = %u AND description !='';"), levelid, roomid);

	////timer.Start();
	int error = mysql_query(&m_mysql, query);
	////timer.Stop();
	if (error)
	{
		LOG_Error(_T("Could not get room description; mysql error %s"), mysql_error(&m_mysql));
		return MYSQL_ERROR;
	}

	res = mysql_store_result(&m_mysql);

	row = mysql_fetch_row(res);

	int num_rooms = mysql_num_rows(res);
	if ((num_rooms > 0) && (row[0]))
		_tcscpy(description, row[0]);

	mysql_free_result(res);

	return 0;

}

////
// room_index: returns index into rooms_ of given room, or -1 if not found
////

int LmLevelDBC::room_index(lyra_id_t roomid) const
{
  // linear search
  for (int i = 0; i < NumRooms(); ++i) {
    if (rooms_[i].RoomID() == roomid) {
      return i;
    }
  }
  return -1;
}


const LmRoomDB& LmLevelDBC::Room(int index) const
{
  return rooms_[index];
}

bool LmLevelDBC::ContainsRoom(lyra_id_t roomid) const
{
  return (room_index(roomid) != -1);
}

const LmRoomDB& LmLevelDBC::RoomDB(lyra_id_t roomid) const
{
  return rooms_[room_index(roomid)];
}


