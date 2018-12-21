// LmRoomItem.h  -*- C++ -*-
// $Id: LmRoomItem.h,v 1.1 1997-09-04 02:29:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// information on an item in a room

#ifndef INCLUDED_LmRoomItem
#define INCLUDED_LmRoomItem

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmItem.h"
#include "LmPosition.h"
#include <time.h>

// class forward declarations

// class declarations

class LmRoomItem {

public:

  LmRoomItem();

  void Init(const LmItem& item, const LmPosition& pos, int lifetime);

  // read/write from/to string rep
  int Parse(const TCHAR* str);
  void UnParse(TCHAR* str, int len) const;

  // selectors
  const LmItem& Item() const;
  const LmPosition& Position() const;
  time_t TimeToDie() const;

  // selectors returning non-const references
  LmItem& Item();
  LmPosition& Position();

  // mutators
  void SetLifetime(int lifetime);

  void Dump(FILE* f, int indent = 0) const;

private:

  // state
  LmItem item_;
  LmPosition pos_;
  time_t timetodie_;

};

#ifdef USE_INLINE
#include "LmRoomItem.i"
#endif

#endif /* INCLUDED_LmRoomItem */
