// LmDatabaseKey.cpp  -*- C++ -*-
// $Id: LmDatabaseKey.cpp,v 1.7 1997-08-20 01:34:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmDatabaseKey.h"
#endif

#include <stdio.h>
#include <string.h>

#include "LmDatabaseKey.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmDatabaseKey.i"
#endif

// static members
const char* LmDatabaseKey::DEFAULT_KEYTYPE = "";
const char* LmDatabaseKey::DEFAULT_FIELD = "";

////
// Constructor
////

LmDatabaseKey::LmDatabaseKey()
{
  Init(DEFAULT_KEYTYPE, DEFAULT_FIELD, DEFAULT_INST, DEFAULT_SUBINST);
}

LmDatabaseKey::LmDatabaseKey(const char* type, const char* field, int instance, int subinstance)
{
  Init(type, field, instance, subinstance);
}

////
// Destructor
////

LmDatabaseKey::~LmDatabaseKey()
{
  // empty
}

////
// Init
////

void LmDatabaseKey::Init(const char* type, const char* field, int instance, int subinstance)
{
  SetType(type);
  SetField(field);
  SetInstance(instance);
  SetSubInstance(subinstance);
}

void LmDatabaseKey::SetType(const char* type)
{
  strcpy(type_, type);
  TRUNC(type_, sizeof(type_));
}

void LmDatabaseKey::SetField(const char* field)
{
  strcpy(field_, field);
  TRUNC(field_, sizeof(field_));
}

const char* LmDatabaseKey::KeyString() const
{
  // possible valid keys:
  // 1 type = "T", field = "F", instance = "1", subinstance = "2" --> T_1_2.F
  // 2 type = "T", field = "F", instance = "1", subinstance = "0" --> T_1.F
  // 3 type = "T", field = "F", instance = "0", subinstance = "0" --> T.F
  // 4 type = "",  field = "F", instance = "0", subinstance = "0" --> F
  // (note: field always used)

  // case 1
  if ((strlen(type_) > 0) && (instance_ != 0) && (subinstance_ != 0)) {
   sprintf((char*) realkey_, _T("%s_%d_%d.%s"), type_, instance_, subinstance_, field_);
  }
  // case 2
  else if ((strlen(type_) > 0) && (instance_ != 0)) {
   sprintf((char*) realkey_, _T("%s_%d.%s"), type_, instance_, field_);
  }
  // case 3
  else if (strlen(type_) > 0) {
   sprintf((char*) realkey_, _T("%s.%s"), type_, field_);
  }
  // case 4
  else {
   sprintf((char*) realkey_, _T("%s"), field_);
  }
  return realkey_;
}

////
// Dump
////

void LmDatabaseKey::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmDatabaseKey[%p,%d]: type='%s' field='%s' instance=%d sub=%d>\n"),
	  this, sizeof(LmDatabaseKey),
	  Type(), Field(), Instance(), SubInstance());
}

////
// Dump1
////

void LmDatabaseKey::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%s]"), KeyString());
}
