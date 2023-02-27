// LmItemDBC.pc  -*- C++ -*-
// $Id: LmItemDBC.pc,v 1.17 1998-05-01 17:04:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <string.h>
#include <stdlib.h>

#include "LmItemDBC.h"
#include "LmLocker.h"
#include "LmLog.h"
#include "LmItem.h"
#include "LmRoomItem.h"
#include "LmRoomItemList.h"
#include "LmInventory.h"
#include "LmTimer.h"
#include "LmFuncTimer.h"
#include "LmPlayerDBC.h"
#include "GMsg_SenseDreamersAck.h"

#ifndef USE_INLINE
#include "LmItemDBC.i"
#endif /* !USE_INLINE */


inline unsigned int ATOI(char* value)
{
  if (!value)
    return 0;
  else
    return ((unsigned int)(atof(value)));
}

 
//
// NB: what the database refers to as the item_id is the serial number, not the header!
//   (which LmItemHdr calls the itemid, sigh)
// 

// constants for the precompiler
#define db_INVENTORY_MAX 50    /* Lyra::INVENTORY_MAX */
#define db_MAX_ROOMITEMS 64    /* Lyra::MAX_ROOMITEMS */

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

LmItemDBC::LmItemDBC(const TCHAR *dbuser, const TCHAR* dbpassword, const TCHAR* dbserver, int dbport)
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
  _tcscpy(dbname_, _T("ul3d_item"));
  _tcscpy(username_, _T("ul3d_item"));
#else
  _tcscpy(username_, _T("ul_item"));
  _tcscpy(dbname_, _T("ul_item"));
#endif

}

////
// Destructor
////

LmItemDBC::~LmItemDBC()
{
  Disconnect();
}

////
// Dump
////

void LmItemDBC::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmItemDBC[%p,%d] time=%lu/%lu last=%lu>\n"), this, sizeof(LmItemDBC),
	  sql_ms_, num_ms_, last_ms_);
}

////
// Connect
////

int LmItemDBC::Connect()
{

  DEFMETHOD(LmItemDBC, Connect);
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
      LOG_Error(_T("%s: MYSQL ul_item init error %s\n"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  if (!mysql_real_connect(&m_mysql, db_server_, username_, password_, dbname_, db_port_, _T("/tmp/mysql.sock"), 0))
  //if (!mysql_real_connect(&m_mysql, db_server_, _T("ul_item"), password_, _T("ul_item"), db_port_, _T("/var/lib/mysql/mysql.sock"), 0))
    {
      LOG_Error(_T("%s: MYSQL connect error %s\n"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  ////timer.Stop();

  connected_ = true;
  return 0;
}

////
// Disconnect
////

void LmItemDBC::Disconnect()
{
 
  LmLocker mon(lock_); // lock object for method duration
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
// AllocateLevelItems -- allocate serial numbers for level server to use for generating items
////

int LmItemDBC::AllocateLevelItems(lyra_id_t level_id, int number_requested, int* item_ids)
{
  DEFMETHOD(LmItemDBC, AllocateLevelItems);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  int error;
  my_ulonglong value;
  TCHAR query[256];

  if (number_requested > 100)
    number_requested = 100;

  // zero out return values
  int i;
  for (i = 0; i < number_requested; ++i) {
    item_ids[i] = Lyra::ID_UNKNOWN;
  }
    
  int counter = 0;

  while (counter < number_requested)
    {
     _stprintf(query, _T("INSERT INTO item  (ITEM_ID, ITEM_HDR, ITEM_STATE1, ITEM_STATE2, ITEM_STATE3, ITEM_NAME, OWNER_TYPE, OWNER_ID, OWNER_SUBID, X, Y) VALUES (NULL, 0, 0, 0, 0, 'level item', 0, %d, 0, 0, 0);"),
	      level_id);
      ////timer.Start();
      error = mysql_query(&m_mysql, query);
      ////timer.Stop();
      if (error)
	{
	  LOG_Error(_T("%s: Could not allocate new item ids; mysql error %s"), method, mysql_error(&m_mysql));
	  return MYSQL_ERROR;
	}
      
      value = mysql_insert_id(&m_mysql);
      
      item_ids[counter] = value;
      
      counter++;
    }
  
  if (counter == number_requested)
    return 0;
  
  LOG_Error(_T("%s: Requested %d new item ids for level, could allocate only %d"), method, number_requested, counter);
  return MYSQL_ERROR;
}


////
// CreateItem - create item for player; return id in item_id
////

int LmItemDBC::CreateItem(lyra_id_t owner, const LmItem& item, int& item_id, const TCHAR* description)
{
  DEFMETHOD(LmItemDBC, CreateItem);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[1024];

  TCHAR escaped_name[40];
  TCHAR escaped_descrip[768];

  mysql_escape_string((TCHAR*)escaped_name, item.Name(),_tcslen(item.Name()));
  mysql_escape_string((TCHAR*)escaped_descrip, description,_tcslen(description));

  /* //Debug code
  TCHAR* grah = new TCHAR[_tcslen(escaped_descrip)-150];
  for (int i=150;i_tcslen(escaped_descrip);i++)
    grah[i-150]=escaped_descrip[i];
  int blah =_tcslen(escaped_descrip);
  */

 _stprintf(query, _T("INSERT INTO item (ITEM_ID, ITEM_HDR, ITEM_HDR_2, ITEM_STATE1, ITEM_STATE2, ITEM_STATE3, ITEM_NAME, OWNER_TYPE, OWNER_ID, OWNER_SUBID, X, Y, ITEM_DESCRIP) VALUES (NULL, %u, %u, %u, %u, %u, '%s', 1, %u, 0, 0, 0, '%s');"), 
	  item.Header().ItemHdr1(), item.Header().ItemHdr2(), item.State1(), item.State2(), item.State3(), 
	  escaped_name,  owner, escaped_descrip);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop(); 

  //  delete escaped_name;
  //delete escaped_descrip;

  if (error)
    {
      LOG_Error(_T("%s: Could not create new item; mysql error %s"), method, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  item_id = mysql_insert_id(&m_mysql);
	
  return 0;
}

////
// DeleteItem - remove item from database
////

int LmItemDBC::DeleteItem(int item_id)
{
  DEFMETHOD(LmItemDBC, DeleteItem);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  TCHAR query[256];

 _stprintf(query, _T("DELETE FROM item WHERE item_id = %u;"), item_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not create new item; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;

}


////
// UpdateItemOwnership - change item ownership information
////

int LmItemDBC::UpdateItemOwnership(int item_id, int owner_type, lyra_id_t owner_id, lyra_id_t owner_sub_id)
{
  DEFMETHOD(LmItemDBC, UpdateItemOwnership);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

 _stprintf(query, _T(" UPDATE item SET owner_type = %u, owner_id = %u, owner_subid = %u, x = 0, y = 0 WHERE item_id = %u;"), owner_type, owner_id, owner_sub_id, item_id);
      
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not update item ownership; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
     
  return 0;

}


////
// UpdateItemState - change item information: state only
////

int LmItemDBC::UpdateItemState(const LmItem& item, int x)
{
  DEFMETHOD(LmItemDBC, UpdateItemState);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

 _stprintf(query, _T("UPDATE item SET item_state1 = %u, item_state2 = %u, item_state3 = %u, x = %d  WHERE item_id = %u;"), item.State1(), item.State2(), item.State3(), x, item.Serial());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not update item state; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }
     
  return 0;
}


////
// UpdateItemFullState - change item information: state, header, and name
////

int LmItemDBC::UpdateItemFullState(const LmItem& item)
{
  DEFMETHOD(LmItemDBC, UpdateItemFullState);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

  TCHAR escaped_name[40];
  mysql_escape_string((TCHAR*)escaped_name, item.Name(),_tcslen(item.Name()));

 _stprintf(query, _T("UPDATE item SET item_state1 = %u, item_state2 = %u, item_state3 = %u, item_hdr = %u, item_hdr_2 = %u, item_name = '%s' WHERE item_id = %u;"), item.State1(), item.State2(), item.State3(), item.Header().ItemHdr1(), item.Header().ItemHdr2(), escaped_name, item.Serial());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  //  delete escaped_name;

  if (error)
    {
      LOG_Error(_T("Could not update item full state; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

     
  return 0;

}

int LmItemDBC::PutItemInPersonalVault(lyra_id_t playerId, const LmRoomItem& item)
{
	DEFMETHOD(LmItemDBC, PutItemInPersonalVault);
	LmLocker mon(lock_);
	TCHAR query[256];
	TCHAR escaped_name[40];

  	mysql_escape_string((TCHAR*)escaped_name, item.Item().Name(),_tcslen(item.Item().Name()));

	 _stprintf(query, _T("UPDATE item SET owner_type=%u, owner_id=%u, owner_subid=0, x=%d, y=%d WHERE item_id = %u;"), OWNER_PERSONAL_VAULT, playerId, item.Position().X(), item.Position().Y(), item.Item().Serial());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  //  delete escaped_name;

  if (error)
    {
      LOG_Error(_T("Could not update item full state; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

     
  return 0;
}

 
////
// UpdateRoomItem - update state of an item in a room (serial, state, name, x, y)
////

int LmItemDBC::UpdateRoomItem(const LmRoomItem& roomitem)
{
  DEFMETHOD(LmItemDBC, UpdateRoomItem);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

  TCHAR escaped_name[40];
  mysql_escape_string((TCHAR*)escaped_name, roomitem.Item().Name(),_tcslen(roomitem.Item().Name()));

 _stprintf(query, _T("UPDATE item SET item_state1 = %u, item_state2 = %u, item_state3 = %u, item_hdr = %u, item_hdr_2 = %u, item_name = '%s', x = %d, y = %d  WHERE item_id = %u;"), roomitem.Item().State1(), roomitem.Item().State2(), roomitem.Item().State3(), roomitem.Item().Header().ItemHdr1(), roomitem.Item().Header().ItemHdr2(), escaped_name, roomitem.Position().X(), roomitem.Position().Y(), roomitem.Item().Serial());

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  //  delete escaped_name;

  if (error)
    {
      LOG_Error(_T("Could not update room item statee; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}


////
// SavePlayerInventory - save items belonging to a player
////

int LmItemDBC::SavePlayerInventory(lyra_id_t owner ,  LmInventory& inventory)
{
  DEFMETHOD(LmItemDBC, SavePlayerInventory);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

  for (int i=0; i<inventory.NumItems(); i++)
    {
     _stprintf(query, _T("UPDATE item SET item_state1 = %u, item_state2 = %u, item_state3 = %u, x = %d WHERE item_id = %u;"), 
		  inventory.ItemByIndex(i).State1(), inventory.ItemByIndex(i).State2(), inventory.ItemByIndex(i).State3(),  inventory.ItemX(i), inventory.ItemByIndex(i).Serial());

      ////timer.Start();
      int error = mysql_query(&m_mysql, query);
      ////timer.Stop();  

      if (error)
	{
	  LOG_Error(_T("Could not update player inventory; mysql error %s"), mysql_error(&m_mysql));
	  return MYSQL_ERROR;
	}
    }
  return 0;
}


////
// SaveRoomItems - save items residing in a room
////

int LmItemDBC::SaveRoomItems(lyra_id_t level_id, lyra_id_t room_id, const LmRoomItemList& room_items)
{
  DEFMETHOD(LmItemDBC, SaveRoomItems);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

  if (room_items.size() == 0) {
    return 0;
  }
 
  for (LmRoomItemList::const_iterator it = room_items.begin(); !(bool)(it == room_items.end()); ++it) {
    const LmRoomItem& ritem = *it;

    TCHAR* escaped_name = LmNEW(TCHAR[_tcslen(ritem.Item().Name())*2+1]);
    mysql_escape_string(escaped_name, ritem.Item().Name(),_tcslen(ritem.Item().Name()));
   
   _stprintf(query, _T("UPDATE item SET owner_subid = %u, item_hdr = %u, item_hdr_2 = %u, item_state1 = %u, item_state2 = %u,  item_state3 = %u, item_name = '%s',  x = %d, y = %d WHERE item_id = %u;"), room_id, ritem.Item().Header().ItemHdr1(), ritem.Item().Header().ItemHdr2(), ritem.Item().State1(), ritem.Item().State2(), ritem.Item().State3(), escaped_name, ritem.Position().X(), ritem.Position().Y(), ritem.Item().Serial()); 

    ////timer.Start();
    int error = mysql_query(&m_mysql, query);
    ////timer.Stop();  

   	LmDELETEARRAY(escaped_name);

    if (error)
      {
	LOG_Error(_T("Could not save room items; mysql error %s"), mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }
  }

  return 0;
}

////
// SetItemDescription
////

int LmItemDBC::SetItemDescription(int item_id, const TCHAR* description)
{
  DEFMETHOD(LmItemDBC, SetItemDescription);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[1024];

  TCHAR escaped_descrip[768];
  mysql_escape_string((TCHAR*)escaped_descrip, description, _tcslen(description));

 _stprintf(query, _T("UPDATE item SET item_descrip = '%s' WHERE item_id = %u;"), escaped_descrip, item_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  //  delete escaped_descrip;

  if (error)
    {
      LOG_Error(_T("Could not update item description; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}

////
// SetItemOwners
////

int LmItemDBC::SetItemOwners(int item_id, lyra_id_t owner_id, lyra_id_t owner_subid, int owner_type)
{
  DEFMETHOD(LmItemDBC, SetItemOwners);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

 _stprintf(query, _T("UPDATE item SET owner_id = %u, owner_subid = %u, owner_type = %u' WHERE item_id = %u;"), 
	  owner_id, owner_subid, owner_type);
	  
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  if (error)
    {
      LOG_Error(_T("Could not update item owner for item %u; mysql error %s"), item_id,  mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;
}


////
// SetNumDreamers
////

int LmItemDBC::SetNumDreamers(lyra_id_t level_id, int count)
{
  DEFMETHOD(LmItemDBC, SetNumDreamers);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

 _stprintf(query, _T("UPDATE locations SET num_dreamers = %d WHERE level_id = %u;"), 
	  count, level_id);
	  
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  if (error)
    {
      LOG_Error(_T("Could not update dreamer count for level %u; mysql error %s"), level_id,  mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;

}

////
// ChangeNumDreamers
////

int LmItemDBC::ChangeNumDreamers(lyra_id_t level_id, int change)
{
  DEFMETHOD(LmItemDBC, ChangeNumDreamers);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  TCHAR query[256];

 _stprintf(query, _T("UPDATE locations SET num_dreamers = num_dreamers + %d WHERE level_id = %u;"), 
	  change, level_id);
	  
  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();  

  if (error)
    {
      LOG_Error(_T("Could not update dreamer count for level %u; mysql error %s"), level_id,  mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  return 0;

}


//// 
// database selectors
////

////
// GetPlayerInventory - get items that belong to player
////

int LmItemDBC::GetPlayerInventory(lyra_id_t owner, LmInventory& inventory)
{
  DEFMETHOD(LmItemDBC, GetPlayerInventory);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements

  TCHAR query[256];
  MYSQL_RES* res;
  MYSQL_ROW row;
  int num_items = 0;

  // Lyra::INVENTORY_MAX = 50

 _stprintf(query, _T("SELECT item_id, item_hdr, item_state1, item_state2, item_state3, item_name, x, item_hdr_2  FROM item  WHERE owner_id = %u AND owner_type = %u;"), owner, OWNER_PLAYER);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get inventory items for player %u; mysql error %s"), owner, mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  // _tprintf(_T("got %d rows\n"), num_items);

  for (int i = 0; i < num_items; ++i) {
    row = mysql_fetch_row(res);

    // create and initialize new room item
    LmItem item; 
    //   _tprintf(_T("about to create a new player item!\n"));
    item.Init(ATOI(row[0]), ATOI(row[1]), ATOI(row[7]), row[5], ATOI(row[2]), ATOI(row[3]), ATOI(row[4]));
    float s1 = atof(row[2]);
    inventory.AddItem(item);
    inventory.SetItemX(ATOI(row[0]), ATOI(row[6]));
  }

  mysql_free_result(res);

  // done
  return 0;
}

int LmItemDBC::GetRoomItems(lyra_id_t level_id, lyra_id_t room_id, LmRoomItemList& room_items) {

  DEFMETHOD(LmItemDBC, GetRoomItems);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  MYSQL_RES* res;
  MYSQL_ROW row;
  int i, num_items = 0;
  TCHAR query[256];
 
  // zero out result variables (only zero out id's, rest are invalid if id is unknown)

  for (LmRoomItemList::iterator it = room_items.begin(); !(bool)(it == room_items.end()); ++it) {
    LmRoomItem& ritem = *it;
    ritem.Item().Header().SetSerial(Lyra::ID_UNKNOWN);
  }

 _stprintf(query, _T("SELECT count(item_id) FROM item WHERE owner_id = %u AND owner_subid = %u AND owner_type = %u;"), level_id, room_id, OWNER_ROOM);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  if (error)
    {
      LOG_Error(_T("Could not get room item count; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  mysql_free_result(res);

  if (num_items > Lyra::MAX_ROOMITEMS) {
    // room has more items than server can handle -- burn 'em!

    LOG_Warning(_T("%s: deleting items from level %d, room %d (DB has %d)"), method, level_id, room_id, num_items);

   _stprintf(query, _T("DELETE FROM item WHERE owner_id = %u AND owner_subid = %u AND owner_type = %u AND (item_name LIKE '%s' OR item_name LIKE '%s' OR item_name LIKE '%s' OR item_name LIKE '%s'OR item_name LIKE '%s')"), level_id, room_id, OWNER_ROOM, _T("\%Essence"), _T("\%Elemen"), _T("\%Alteror"), _T("\%Charm"), _T("\%Chakram"));

    ////timer.Start();
    int error = mysql_query(&m_mysql, query);
    ////timer.Stop();
    if (error)
      {
	LOG_Error(_T("Could not get room item count; mysql error %s"), mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);

    int num_deleted_items = mysql_affected_rows(&m_mysql);

    mysql_free_result(res);

    if ((num_items - num_deleted_items) > Lyra::MAX_ROOMITEMS)
      {
	LOG_Error(_T("%s: deleted %d room items, but still %d remaining - can not start\n"), method, num_deleted_items, num_items);
	return MYSQL_ERROR;
      }

  } // end if too many items in DB for server to handle


  // else: not too many items, load their ids

 _stprintf(query, _T("SELECT item_id, item_hdr, item_state1, item_state2, item_state3, item_name, x, y, item_hdr_2 FROM item WHERE owner_id = %u AND owner_subid = %u AND owner_type = %u;"), level_id, room_id, OWNER_ROOM);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get room items; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  for (i = 0; i < num_items; ++i) {

    row = mysql_fetch_row(res);

    // if returned id is 0, then we're done
    if (ATOI(row[0]) == Lyra::ID_UNKNOWN) {
      break;
    }

    // create and initialize new room item
    LmRoomItem ritem; 
    ritem.Item().Init(ATOI(row[0]), ATOI(row[1]), ATOI(row[8]), row[5], ATOI(row[2]), ATOI(row[3]), ATOI(row[4]));
    ritem.Position().Init(_ttoi(row[6]), _ttoi(row[7]), 0, 0); // height/angle not stored
    ritem.SetLifetime(60); // live for a minute after being loaded?
    // append to list
    room_items.push_back(ritem);
  }

  mysql_free_result(res);

  // done
  return 0;

}


int LmItemDBC::GetPersonalVaultItems(lyra_id_t player_id, LmRoomItemList& room_items) {

  DEFMETHOD(LmItemDBC, GetRoomItems);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  MYSQL_RES* res;
  MYSQL_ROW row;
  int i, num_items = 0;
  TCHAR query[256];
 
  // zero out result variables (only zero out id's, rest are invalid if id is unknown)

  for (LmRoomItemList::iterator it = room_items.begin(); !(bool)(it == room_items.end()); ++it) {
    LmRoomItem& ritem = *it;
    ritem.Item().Header().SetSerial(Lyra::ID_UNKNOWN);
  }

 _stprintf(query, _T("SELECT count(item_id) FROM item WHERE owner_id = %u AND owner_type = %u;"), player_id, OWNER_PERSONAL_VAULT);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  if (error)
    {
      LOG_Error(_T("Could not get room item count; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  mysql_free_result(res);

  if (num_items > Lyra::MAX_ROOMITEMS) {
    // room has more items than server can handle -- burn 'em!

    LOG_Warning(_T("%s: deleting items from player %d, (DB has %d)"), method, player_id, num_items);

   _stprintf(query, _T("DELETE FROM item WHERE owner_id = %u AND owner_type = %u AND (item_name LIKE '%s' OR item_name LIKE '%s' OR item_name LIKE '%s' OR item_name LIKE '%s'OR item_name LIKE '%s')"), player_id, OWNER_PERSONAL_VAULT, _T("\%Essence"), _T("\%Elemen"), _T("\%Alteror"), _T("\%Charm"), _T("\%Chakram"));

    ////timer.Start();
    int error = mysql_query(&m_mysql, query);
    ////timer.Stop();
    if (error)
      {
	LOG_Error(_T("Could not get room item count; mysql error %s"), mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

    res = mysql_store_result(&m_mysql);

    int num_deleted_items = mysql_affected_rows(&m_mysql);

    mysql_free_result(res);

    if ((num_items - num_deleted_items) > Lyra::MAX_ROOMITEMS)
      {
	LOG_Error(_T("%s: deleted %d room items, but still %d remaining - can not start\n"), method, num_deleted_items, num_items);
	return MYSQL_ERROR;
      }

  } // end if too many items in DB for server to handle


  // else: not too many items, load their ids

 _stprintf(query, _T("SELECT item_id, item_hdr, item_state1, item_state2, item_state3, item_name, x, y, item_hdr_2 FROM item WHERE owner_id = %u AND owner_type = %u;"), player_id, OWNER_PERSONAL_VAULT);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get room items; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  for (i = 0; i < num_items; ++i) {

    row = mysql_fetch_row(res);

    // if returned id is 0, then we're done
    if (ATOI(row[0]) == Lyra::ID_UNKNOWN) {
      break;
    }

    // create and initialize new room item
    LmRoomItem ritem; 
    ritem.Item().Init(ATOI(row[0]), ATOI(row[1]), ATOI(row[8]), row[5], ATOI(row[2]), ATOI(row[3]), ATOI(row[4]));
    ritem.Position().Init(_ttoi(row[6]), _ttoi(row[7]), 0, 0); // height/angle not stored
    ritem.SetLifetime(60); // live for a minute after being loaded?
    // append to list
    room_items.push_back(ritem);
  }

  mysql_free_result(res);

  // done
  return 0;

}

////
// GetLevelItems - get item ids that were allocated for a given level, but have not been used
////

int LmItemDBC::GetLevelItems(lyra_id_t level_id, int* item_ids)
{
  DEFMETHOD(LmItemDBC, GetLevelItems);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  MYSQL_RES* res;
  MYSQL_ROW row;
  int i, num_items = 0;
  TCHAR query[256];

  // zero out result variables (only zero out id's, rest are invalid if id is unknown)
  for (i = 0; i < Lyra::MAX_ROOMITEMS; ++i) {
    item_ids[i] = Lyra::ID_UNKNOWN;
  }

 _stprintf(query, _T(" SELECT count(item_id) FROM item WHERE owner_id = %u AND owner_subid = 0 AND owner_type = %u;"), level_id, OWNER_ROOM);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not get level items; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  row = mysql_fetch_row(res);

  num_items = ATOI(row[0]);

  mysql_free_result(res);

  LOG_Debug(_T("%s: level %d has %d cached items"), method, level_id, num_items);

  if (num_items > Lyra::MAX_ROOMITEMS) {
    // DB has more items in cache than server can handle -- burn 'em!
    
    LOG_Warning(_T("%s: deleting items from level %d (DB has %d)"), method, level_id, num_items);

   _stprintf(query, _T("DELETE FROM item WHERE owner_id = %u AND owner_subid = 0 AND owner_type = %u"), level_id,  OWNER_ROOM);

    ////timer.Start();
    error = mysql_query(&m_mysql, query);
    ////timer.Stop();

    if (error)
      {
	LOG_Error(_T("Could not delete level items; mysql error %s"), mysql_error(&m_mysql));
	return MYSQL_ERROR;
      }

  } // end if too many items in DB for server to handle

  // else: not too many items, load their ids

 _stprintf(query, _T("SELECT item_id  FROM item WHERE owner_id = %u AND owner_subid = 0 AND owner_type = %u;"), level_id, OWNER_ROOM);

  ////timer.Start();
  error = mysql_query(&m_mysql, query);
  ////timer.Stop();

  if (error)
    {
      LOG_Error(_T("Could not select level items; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  num_items = mysql_num_rows(res);

  for (i = 0; i < num_items; ++i) {

    row = mysql_fetch_row(res);

    item_ids[i] = ATOI(row[0]);
  }

  mysql_free_result(res);

  // done
  return 0;

}


////
// GetItemDescription
////

int LmItemDBC::GetItemDescription(int item_id, TCHAR* description)
{
  DEFMETHOD(LmItemDBC, GetItemDescription);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  MYSQL_RES* res;
  MYSQL_ROW row;
  TCHAR query[256];

 _stprintf(query, _T("SELECT item_descrip FROM item WHERE item_id = %u;"), item_id);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  if (error)
    {
      LOG_Error(_T("Could not get item description; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  row = mysql_fetch_row(res);

  int num_items = mysql_num_rows(res);
  if ((num_items > 0) && (row[0]))
   _tcscpy(description, row[0]);

  mysql_free_result(res);

  return 0;

}

int LmItemDBC::GetTotalNumDreamers(unsigned int* total)
{
	DEFMETHOD(LmItemDBC, GetTotalNumDreamers);
	LmLocker mon(lock_);
	
  	MYSQL_RES* res;
  	MYSQL_ROW row;
  	TCHAR query[256];
	
	_stprintf(query, _T("SELECT SUM(num_dreamers) FROM locations;"));
	int error = mysql_query(&m_mysql, query);
  	if (error)
    	{
      		LOG_Error(_T("Could not get dreamer concentrations; mysql error %s"), mysql_error(&m_mysql));
      		return MYSQL_ERROR;
    	}	

	res = mysql_store_result(&m_mysql);
	row = mysql_fetch_row(res);
	*total = ATOI(row[0]);
  	mysql_free_result(res);

	  return 0;

}


////
// GetDreamerLocations
////

int LmItemDBC::GetDreamerLocations(lyra_id_t* level_ids, int accountType)
{
  DEFMETHOD(LmItemDBC, GetDreamerLocations);
  LmLocker mon(lock_); // lock object for method duration
  //LmFuncTimer( timernum_calls_, num_ms_, last_ms_); // time function
  ////LmTimer timer(&sql_ms_); // timer for SQL statements
  MYSQL_RES* res;
  MYSQL_ROW row;
  TCHAR query[256];

	if (accountType == LmPlayerDB::ACCT_PMARE) 
		_stprintf(query, _T("SELECT level_id, num_dreamers FROM locations ORDER BY num_dreamers DESC LIMIT %d;"), PLANES_SENSED_COUNT+5);
	else
		_stprintf(query, _T("SELECT level_id, num_dreamers FROM locations ORDER BY num_dreamers DESC LIMIT %d;"), PLANES_SENSED_COUNT);

  ////timer.Start();
  int error = mysql_query(&m_mysql, query);
  ////timer.Stop();
  if (error)
    {
      LOG_Error(_T("Could not get dreamer concentrations; mysql error %s"), mysql_error(&m_mysql));
      return MYSQL_ERROR;
    }

  res = mysql_store_result(&m_mysql);

  int num_planes = mysql_num_rows(res);
  int planes_added = 0;
  int i;
  int numAdded = 0;
  for (i=0; i<num_planes; i++) {
    row = mysql_fetch_row(res);
    level_ids[i] = 0;
    if (row[0]) {
		if (ATOI(row[1]) != 0) {
			level_ids[numAdded++] = ATOI(row[0]);
		}
		else
			level_ids[numAdded++] = 0;
		if (PLANES_SENSED_COUNT	== numAdded)
			break;
    }
  }

  mysql_free_result(res);

  return 0;

}


