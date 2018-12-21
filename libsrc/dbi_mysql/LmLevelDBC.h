// LmLevelDBC.h  -*- C++ -*-
// $Id: LmLevelDBC.h,v 1.9 1998-02-20 20:25:45-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item database connection class

#ifndef INCLUDED_LmLevelDBC
#define INCLUDED_LmLevelDBC

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"
#include <mysql.h>

#include "PThMutex.h"
#include "../../libsrc/shared/SharedConstants.h"

// local types


// forward declarations

class LmRoomDB;
class LmLog;

// the class

class LmLevelDBC {

public:

  enum {
    // constants
    MAX_NUMROOMS = Lyra::MAX_LEVELROOMS,
    // defaults
    DEFAULT_NUMROOMS = 0,
    DEFAULT_LEVELID = Lyra::ID_UNKNOWN, 
    // return codes
    MYSQL_ERROR = -1 // bad things, man
  };
  static const TCHAR* DEFAULT_LEVELNAME;

public:

  LmLevelDBC(const TCHAR *dbuser, const TCHAR *dbpassword, const TCHAR* dbserver, int dbport);
  ~LmLevelDBC();

  int Connect();
  void Disconnect();

  // load values for this level, prepare for retrieval
  int Load(lyra_id_t level_id, bool load_gens);

  // database selectors 
  // selectors
  lyra_id_t LevelID() const;
  const TCHAR* LevelName() const;
  int NumRooms() const;
  int MaxPlayers() const;
  const LmRoomDB& Room(int index) const;

  bool ContainsRoom(lyra_id_t roomid) const;
  const LmRoomDB& RoomDB(lyra_id_t roomid) const;

  long LastSQLCode() const;

  void SetLog(LmLog* log);

  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmLog* Log() const;

  int room_index(lyra_id_t roomid) const;

private:

  // not implemented
  LmLevelDBC(const LmLevelDBC&);
  //operator=(const LmLevelDBC&);

  // mysql object
  MYSQL mysql_;

  lyra_id_t level_id_;
  TCHAR level_name_[Lyra::LEVELNAME_MAX];
  int num_rooms_;
  int max_players_;
  int room_ids_[Lyra::MAX_LEVELROOMS];
  LmRoomDB* rooms_;

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
#include "LmLevelDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmLevelDBC */
