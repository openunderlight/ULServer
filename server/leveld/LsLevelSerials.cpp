// LsLevelSerials.cpp  -*- C++ -*-
// $Id: LsLevelSerials.cpp,v 1.7 1998-05-11 11:11:43-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include "LsLevelSerials.h"
#include "LsMain.h"
#include "LmDatabase.h"
#include "LmDatabaseKey.h"
#include "LmLocker.h"
#include "LmLevelDBC.h"
#include "LmItemDBC.h"
#include "LmLogFile.h"
#include "LsUtil.h"
#ifndef USE_INLINE
#include "LsLevelSerials.i"
#endif

////
// Constructor
////

LsLevelSerials::LsLevelSerials(LsMain* lsmain)
  : main_(lsmain)
{
  lock_.Init();
}

////
// Destructor
////

LsLevelSerials::~LsLevelSerials()
{
  // empty
}

// support for loading and saving level data to/from the disk has been removed; 
// the supporting code has been commented out below. it is likely that the
// only reason to put the code that in would be to use support an additional 
// backup mechanism to save level state to disk when the database is not available, 
// which would also entail adding logic to recover from such a failure.  

////
// LoadFromDisk
////

#define DBFETCH_I(f, v)  { key.SetField(f); if (db.Exists(key)) db.Fetch(key, &v); }

void LsLevelSerials::LoadFromDisk(LmDatabase& db)
{
  LmLocker mon(lock_); // lock object during method duration
  // fields: NumSerials, Serial_*
  LmDatabaseKey key(_T("Level"), _T(""), main_->LevelDBC()->LevelID(), 0);
  // get number of serials
  int num_serials;
  DBFETCH_I(_T("NumSerials"), num_serials);
  // load serial numbers
  for (int i = 0; i < num_serials; ++i) {
    char tmpkey[32];
   _stprintf(tmpkey, _T("Serial_%d"), i + 1);
    key.SetField(tmpkey);
    int tmpserial = 0;
    db.Fetch(key, &tmpserial);
    if (tmpserial != 0) {
      serials_.push_back(tmpserial);
    }
  }
}

#undef DBFETCH_I

////
// SaveToDisk
////

#define DBSTORE(f, v)  { key.SetField(f); db.Store(key, v, 1); }

void LsLevelSerials::SaveToDisk(LmDatabase& db)
{
// const int NULL = 0;
  LmLocker mon(lock_); // lock object during method duration
  // fields: NumSerials, Serial_*
  LmDatabaseKey key(_T("Level"), _T(""), main_->LevelDBC()->LevelID(), 0 );
// store simple fields
  DBSTORE(_T("NumSerials"), (long int)serials_.size());
  // save serial numbers
  int j = 1;
  for (std::list<int>::iterator li = serials_.begin(); (bool)!(li == serials_.end()); ++li) {
    char tmpkey[32];
   _stprintf(tmpkey, _T("Serial_%d"), j);
    ++j;
    key.SetField(tmpkey);
    db.Store(key, *li, 1);
  }
}

#undef DBSTORE

 

////
// LoadFromDB
////

int LsLevelSerials::LoadFromDB()
{
  DEFMETHOD(LsLevelSerials, LoadFromDB);
  LmLocker mon(lock_); // lock object during method duration
  // get free serials
  int serials[Lyra::MAX_ROOMITEMS];
  int rc = main_->ItemDBC()->GetLevelItems(main_->LevelDBC()->LevelID(), serials);
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::GetLevelItems took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Warning(_T("%s: could not get free serials; rc=%d, sql=%d"), method, rc, sqlcode);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return -1;
  }
  // copy into serial list
  for (int i = 0; i < Lyra::MAX_ROOMITEMS; ++i) {
    if (serials[i] != Lyra::ID_UNKNOWN) {
      serials_.push_back(serials[i]); // add to list
    }
  }
  // if there aren't enough free serials, allocate some more
  if (serials_.size() < SERIAL_HIGH) {
    int num_to_get = SERIAL_HIGH - serials_.size();
    if (allocate_serials(num_to_get) < 0) {
      return -1;
    }
  }
  return 0;
}

////
// SaveToDB
////

int LsLevelSerials::SaveToDB()
{
  DEFMETHOD(LsLevelSerials, SaveToDB);
  LmLocker mon(lock_); // lock object during method duration
  // if there are more than max free serial numbers, delete any extra
  if (serials_.size() > SERIAL_MAX) {
    int num_to_delete = serials_.size() - SERIAL_MAX;
    for (int i = 0; i < num_to_delete; ++i) {
      int serial = serials_.front();
      serials_.pop_front();
      int rc = main_->ItemDBC()->DeleteItem(serial);
      int sqlcode = main_->ItemDBC()->LastSQLCode();
      // int lt = main_->ItemDBC()->LastCallTime();
      // main_->Log()->Debug(_T("%s: LmItemDBC::DeleteItem took %d ms"), method, lt);
      if (rc < 0) {
	main_->Log()->Error(_T("%s: could not delete item %u; rc=%d, sql=%d"), method, serial, rc, sqlcode);
	LsUtil::HandleItemError(main_, method, rc, sqlcode);
	// return -1;
      }
    }
  }
  // save free serials (room 0)
  for (std::list<int>::iterator i = serials_.begin(); !(bool)(i == serials_.end()); ++i) {
    int serial = *i;
    int rc = main_->ItemDBC()->UpdateItemOwnership(serial, LmItemDBC::OWNER_ROOM, main_->LevelDBC()->LevelID(), 0);
    int sqlcode = main_->ItemDBC()->LastSQLCode();
    // int lt = main_->ItemDBC()->LastCallTime();
    // main_->Log()->Debug(_T("%s: LmItemDBC::UpdateItemOwnership took %d ms"), method, lt);
    if (rc < 0) {
      main_->Log()->Error(_T("%s: could not set item %u owner; rc=%d, sql=%d"), method, serial, rc, sqlcode);
      LsUtil::HandleItemError(main_, method, rc, sqlcode);
      return -1;
    }
  }
  return 0;
}

////
// FreeSerial
////

void LsLevelSerials::FreeSerial(int serialnum)
{
  LmLocker mon(lock_); // lock object for method duration
  serials_.push_back(serialnum);
}

////
// GetNextSerial
////

int LsLevelSerials::GetNextSerial()
{
  DEFMETHOD(LsLevelSerials, GetNextSerial);
  LmLocker mon(lock_); // lock object during method
  int retval = 0;
  // if the free serial list is "too small", get some from the database
  if (serials_.size() < SERIAL_LOW) {
    int num_to_get = SERIAL_HIGH - serials_.size();
    if (allocate_serials(num_to_get) < 0) {
      main_->Log()->Error(_T("%s: could not get %d serial numbers"), method, num_to_get);
      // TODO: set some shutdown flag or something?
      return 0; // doh!
    }
  }
  // if the free serial list is not empty, return one from it
  if (serials_.size() > 0) {
    retval = serials_.front();
    serials_.pop_front();
  }
  return retval;
}

////
// Dump
////

void LsLevelSerials::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object during method
  INDENT(indent, f);
 _ftprintf(f, _T("<LsLevelSerials[%p,%d]: main=[%p] serials=%d>\n"),
	  this, sizeof(LsLevelSerials), main_, serials_.size());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("serials: "));
  for (std::list<int>::const_iterator i = serials_.begin(); !(bool)(i == serials_.end()); ++i) {
   _ftprintf(f, _T("%d "), *i);
  }
 _ftprintf(f, _T("\n"));
}

////
// allocate_serials - gets free serial numbers from db, saves in list; return 0 if ok, -1 if not;
//   assumes lock is held
////

int LsLevelSerials::allocate_serials(int num_serials)
{
  DEFMETHOD(LsLevelSerials, allocate_serials);
  int serials[SERIAL_MAX];
  // get from database
  if (num_serials > SERIAL_MAX) {
    num_serials = SERIAL_MAX;
  }
  int rc = main_->ItemDBC()->AllocateLevelItems(main_->LevelDBC()->LevelID(), num_serials, serials);
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::AllocateLevelItems took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Warning(_T("%s: could not allocate %d new serials; rc=%d, sql=%d"), method, num_serials, rc, sqlcode);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return -1;
  }
  // copy into list
  for (int i = 0; i < num_serials; ++i) {
    if (serials[i] != Lyra::ID_UNKNOWN) {
      serials_.push_back(serials[i]);
    }
  }
  main_->Log()->Debug(_T("%s: total number of serials is now %d"), method, serials_.size());
  return 0;
}
