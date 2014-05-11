// LmRoomItem.cpp  -*- C++ -*-
// $Id: LmRoomItem.cpp,v 1.2 1997-10-14 21:39:18-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmRoomItem.h"
#endif

#include "LmRoomItem.h"
#include "LmUtil.h"

#ifndef USE_INLINE
#include "LmRoomItem.i"
#endif

////
// Constructor
////

LmRoomItem::LmRoomItem()
{
  // nothing, see Init
}

////
// Init
////

void LmRoomItem::Init(const LmItem& item, const LmPosition& pos, int lifetime)
{
  item_ = item;
  pos_ = pos;
  SetLifetime(lifetime);
}

////
// Parse
////

int LmRoomItem::Parse(const TCHAR* str)
{
  TCHAR item_str[80], pos_str[80];
  int lifetime;
  if (_stscanf(str, _T("%s %s %d"), item_str, pos_str, &lifetime) != 3) {
    return -1;
  }
  if (item_.Parse(item_str) < 0) {
    return -1;
  }
  if (pos_.Parse(pos_str) < 0) {
    return -1;
  }
  SetLifetime(lifetime);
  return 0;
}

////
// UnParse
////

void LmRoomItem::UnParse(TCHAR* str, int /* len */) const
{
  TCHAR item_str[80], pos_str[80];
  int lifetime = LmUtil::TimeUntil(timetodie_);
  if (lifetime < 0) {
    lifetime = 10;
  }
  item_.UnParse(item_str, sizeof(item_str));
  pos_.UnParse(pos_str, sizeof(pos_str));
 _stprintf(str, _T("%s %s %d"), item_str, pos_str, lifetime);
}

////
// Dump
////

void LmRoomItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  TCHAR itemstr[100];
  item_.UnParse(itemstr, sizeof(itemstr));
 _ftprintf(f, _T("<LmRoomItem[%p,%d]: item=%s pos="), this, sizeof(LmRoomItem), itemstr);
  pos_.Dump1(f);
 _ftprintf(f, _T(" timetodie=%lu (%ld)>\n"), timetodie_, LmUtil::TimeUntil(timetodie_));
}
