// LmItemDBC.h  -*- C++ -*-
// $Id: LmItemDBC.h,v 1.9 1998-02-20 20:25:45-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item database connection class

#ifndef INCLUDED_LmItemDBC
#define INCLUDED_LmItemDBC

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"
#include <mysql.h>
#include "PThMutex.h"
#include "../../libsrc/shared/SharedConstants.h" 
// local types

typedef TCHAR lyra_item_name[20];
typedef TCHAR lyra_item_descrip[512];

// forward declarations

class LmLog;
class LmItem;
class LmRoomItem;
class LmRoomItemList;
class LmInventory;

// the class

class LmItemDBC {

public:

  enum {
    // owner types
    OWNER_ROOM = 0,    // item owned by room
    OWNER_PLAYER = 1,  // item owned by player

    // return codes
    MYSQL_ERROR = -1, // bad things, man
  };

public:

  LmItemDBC(const TCHAR *dbuser, const TCHAR *dbpassword, const TCHAR* dbserver, int dbport);
  ~LmItemDBC();

  int Connect();
  void Disconnect();

  // database mutators
  int AllocateLevelItems(lyra_id_t level_id, int number_requested, int* item_ids);
  int CreateItem(lyra_id_t owner, const LmItem& item, int& item_id, const TCHAR* description);
  int DeleteItem(int item_id);
  int UpdateItemOwnership(int item_id, int owner_type, lyra_id_t owner_id, lyra_id_t owner_sub_id);
  int UpdateItemState(const LmItem& item, int x = 0);
  int UpdateItemFullState(const LmItem& item);
  int UpdateRoomItem(const LmRoomItem& roomitem);
  int SavePlayerInventory(lyra_id_t owner, LmInventory& inventory);
  int SaveRoomItems(lyra_id_t level_id, lyra_id_t room_id, const LmRoomItemList& room_items);
  int SetItemDescription(int item_id, const TCHAR* description);
  int SetItemOwners(int item_id, lyra_id_t owner_id, lyra_id_t subowner_id, int owner_type);
  int SetNumDreamers(lyra_id_t level_id, int count);
  int ChangeNumDreamers(lyra_id_t level_id, int change);

  // database selectors
  int GetPlayerInventory(lyra_id_t owner, LmInventory& inventory);
  int GetLevelItems(lyra_id_t level_id, int* item_ids);
  int GetRoomItems(lyra_id_t level_id, lyra_id_t room_id, LmRoomItemList& room_items);
  int GetItemDescription(int item_id, TCHAR* description); 
  int GetDreamerLocations(lyra_id_t* level_ids, int accountType);
  
  long LastSQLCode() const;

  void SetLog(LmLog* log);

  long LastCallTime() const;
  long TotalCallTime() const;
  long TotalSQLTime() const;

  void Dump(FILE* f, int indent = 0) const;

protected:

  LmLog* Log() const;

private:

  // not implemented
  LmItemDBC(const LmItemDBC&);
  //operator=(const LmItemDBC&);

  // mysql object
  MYSQL m_mysql;

  bool connected_;
  PThMutex lock_;
  long last_sql_code_;
  LmLog* log_;

  TCHAR username_[40]; 
  TCHAR password_[40];   
  TCHAR db_server_[80];
  TCHAR dbname_[20];
  int db_port_;

  void* ctx_;

  int num_calls_;          // total number of calls to class
  unsigned long last_ms_;  // time of last call (milliseconds)
  unsigned long num_ms_;   // total time of all calls
  unsigned long sql_ms_;   // total time spent in "EXEC SQL" blocks

};

#ifdef USE_INLINE
#include "LmItemDBC.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmItemDBC */
