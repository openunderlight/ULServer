// LsItemGen.h  -*- C++ -*-
// $Id: LsItemGen.h,v 1.9 1997-09-09 02:58:41-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server item generator

#ifndef INCLUDED_LsItemGen
#define INCLUDED_LsItemGen

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmRoomItem.h"

// class forward declarations

class LmItemGenDB;

// class declarations

class LsItemGen {

public:

  LsItemGen();

  void Init(const LmItemGenDB* db);

  const LmItemGenDB* DB() const;
  bool ReadyToGenerate() const;
  const LmRoomItem& Generate(int serial);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsItemGen(const LsItemGen&);
  //operator=(const LsItemGen&);

  // generator info
  const LmItemGenDB* db_;

  // current state
  time_t last_gen_;
  time_t next_gen_;
  LmRoomItem item_;

};

#ifdef USE_INLINE
#include "LsItemGen.i"
#endif

#endif /* INCLUDED_LsItemGen */
