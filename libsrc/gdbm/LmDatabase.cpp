// LmDatabase.cpp  -*- C++ -*-
// $Id: LmDatabase.cpp,v 1.6 1997-06-17 23:14:51-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmDatabase.h"
#endif

#include <string.h>
#include <stdio.h>

#include "LmDatabase.h"
#include "LmDatabaseKey.h"

#ifndef USE_INLINE
#include "LmDatabase.i"
#endif

////
// Constructor
////

LmDatabase::LmDatabase()
{
  // empty
}

////
// Destructor
////

LmDatabase::~LmDatabase()
{
  // empty
}

////
// Store
////

int LmDatabase::Store(const LmDatabaseKey& key, const TCHAR* value, int flag)
{
  return Gdbm::Store((TCHAR*)key.KeyString(), value, flag);
}

int LmDatabase::Store(const LmDatabaseKey& key, int value, int flag)
{
  return Gdbm::Store((TCHAR*)key.KeyString(), value, flag);
}

int LmDatabase::Store(const LmDatabaseKey& key, unsigned int value, int flag)
{
  return Gdbm::Store((TCHAR*)key.KeyString(), (int) value, flag);
}

int LmDatabase::Store(const LmDatabaseKey& key, long int value, int flag) 
{
  return Gdbm::Store((TCHAR*)key.KeyString(), (int) value, flag);
}

////
// Fetch
////

int LmDatabase::Fetch(const LmDatabaseKey& key, TCHAR* value, int value_size) const
{
  return Gdbm::Fetch(key.KeyString(), value, value_size);
}

int LmDatabase::Fetch(const LmDatabaseKey& key, int* value) const
{
  return Gdbm::Fetch(key.KeyString(), value);
}

int LmDatabase::Fetch(const LmDatabaseKey& key, unsigned int* value) const
{
  return Gdbm::Fetch(key.KeyString(), (int*) value);
}

int LmDatabase::Fetch(const LmDatabaseKey& key, long int* value) const
{
  return Gdbm::Fetch(key.KeyString(), (int*) value);
}

////
// Exists
////

bool LmDatabase::Exists(const LmDatabaseKey& key) const
{
  return Gdbm::Exists(key.KeyString());
}

////
// Delete
////

int LmDatabase::Delete(const LmDatabaseKey& key)
{
  return Gdbm::Delete(key.KeyString());
}
