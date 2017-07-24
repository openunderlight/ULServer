// LmItemGen.h  -*- C++ -*-
// $Id: LmItemGen.h,v 1.2 1998-02-20 20:26:19-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item generator

#ifndef INCLUDED_LmItemGen
#define INCLUDED_LmItemGen

#include "LmItem.h"
#include "LmItemDefs.h"

// the class

class LmItemGen {

public:

  enum {
    // generator type range
    MIN_GENTYPE = 1,
    MAX_GENTYPE = 4,
    CODEX_GENERATOR = 5,

    // kind of item to generate
    ITEM_ANY = 1,
    ITEM_CHANGESTAT,
    ITEM_ARMOR,
    ITEM_EFFECTPLAYER,
    ITEM_MISSILE,
	ITEM_TOKEN,
    ITEM_CODEX
  };

  static void GenerateItem(int generator_type, int item_type, LmItem& item);

private:

  // utility class, not ever instantiated
  LmItemGen();
  ~LmItemGen();

};

#ifdef USE_INLINE
#include "LmItemGen.i"
#endif

#endif /* INCLUDED_LmItemGen */
