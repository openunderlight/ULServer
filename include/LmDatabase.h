// LmDatabase.h  -*- C++ -*-
// $Id: LmDatabase.h,v 1.7 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// database interface class

#ifndef INCLUDED_LmDatabase
#define INCLUDED_LmDatabase

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"
#include "Gdbm.h"

// forward declarations

class LmDatabaseKey;

// the class

class LmDatabase : public Gdbm {

public:

  enum {
    // constants
    MAX_VALUELEN = 256
  };

public:

  LmDatabase();
  ~LmDatabase();

  // additional "OO" database accessors
  int Store(const LmDatabaseKey& key, const TCHAR* value, int flag = GDBM_REPLACE);
  int Store(const LmDatabaseKey& key, int value, int flag = GDBM_REPLACE);
  int Store(const LmDatabaseKey& key, unsigned int value, int flag = GDBM_REPLACE);
  int Store(const LmDatabaseKey& key, long int value, int flag = GDBM_REPLACE);
  int Fetch(const LmDatabaseKey& key, TCHAR* value, int value_size) const;
  int Fetch(const LmDatabaseKey& key, int* value) const;
  int Fetch(const LmDatabaseKey& key, unsigned int* value) const;
  int Fetch(const LmDatabaseKey& key, long int* value) const;
  bool Exists(const LmDatabaseKey& key) const;
  int Delete(const LmDatabaseKey& key);

private:

  // not implemented
  LmDatabase(const LmDatabase&);
  //operator=(const LmDatabase&);

};

#ifdef USE_INLINE
#include "LmDatabase.i"
#endif

#endif /* INCLUDED_LmDatabase */
