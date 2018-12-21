// LmItemGenDB.h  -*- C++ -*-
// $Id: LmItemGenDB.h,v 1.8 1997-09-09 02:52:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item generator properties

#ifndef INCLUDED_LmItemGenDB
#define INCLUDED_LmItemGenDB

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmPosition.h"
#include "../../libsrc/shared/LmItem.h"

// forward declarations

// the class

class LmItemGenDB {

public:

  enum {
    // constants
    MAX_NUMITEMS = 32,

    // defaults
    DEFAULT_GENTYPE = 1,
    DEFAULT_BASEDELAY = 0,
    DEFAULT_VARDELAY = 0,
    DEFAULT_ITEMTTL = 0,
    DEFAULT_VARTTL = 15
  };

public:

  LmItemGenDB();
  ~LmItemGenDB();

  // selectors
  const LmPosition& Position() const;
  int GeneratorType() const;
  int BaseDelay() const;
  int VarDelay() const;
  int ItemTTL() const;
  int VarTTL() const;

  void Init(int d_base, int d_var, int l_base, int l_var, int x, int y, int z, int angle, int gen_type);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmItemGenDB(const LmItemGenDB&);
  //operator=(const LmItemGenDB&);

  LmPosition pos_;
  int gen_type_;
  int base_delay_;
  int var_delay_;
  int item_ttl_;
  int var_ttl_;

};

#ifdef USE_INLINE
#include "LmItemGenDB.i"
#endif

#endif /* INCLUDED_LmItemGenDB */
