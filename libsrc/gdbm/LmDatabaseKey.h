// LmDatabaseKey.h  -*- C++ -*-
// $Id: LmDatabaseKey.h,v 1.6 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// database key class

#ifndef INCLUDED_LmDatabaseKey
#define INCLUDED_LmDatabaseKey

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

// the class

class LmDatabaseKey {

public:

  enum {
    // constants
    MAX_FIELDLEN = 32,
    MAX_TYPELEN = 32,
    MAX_KEYLEN = 80,

    // defaults
    DEFAULT_INST = 0,
    DEFAULT_SUBINST = 0
  };
  static const TCHAR* DEFAULT_KEYTYPE;
  static const TCHAR* DEFAULT_FIELD;

public:

  LmDatabaseKey();
  LmDatabaseKey(const TCHAR* type, const TCHAR* field, int instance, int subinstance);
  ~LmDatabaseKey();

  void Init(const TCHAR* type, const TCHAR* field, int instance, int subinstance);

  const TCHAR* KeyString() const;

  // selectors
  const TCHAR* Type() const;
  const TCHAR* Field() const;
  int Instance() const;
  int SubInstance() const;

  // mutators
  void SetType(const TCHAR* type);
  void SetField(const TCHAR* field);
  void SetInstance(int instance);
  void SetSubInstance(int subinstance);

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  TCHAR type_[MAX_TYPELEN];    // eg. "Room"
  TCHAR field_[MAX_FIELDLEN];  // eg. "ID"
  int instance_;              // eg. 1
  int subinstance_;           // eg. 2    => "Room_1_2.ID"

  TCHAR realkey_[MAX_KEYLEN];  // string rep of above members

};

#ifdef USE_INLINE
#include "LmDatabaseKey.i"
#endif

#endif /* INCLUDED_LmDatabaseKey */
