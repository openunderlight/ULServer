// LmItemGenDB.cpp  -*- C++ -*-
// $Id: LmItemGenDB.cpp,v 1.9 1997-09-09 02:52:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmItemGenDB.h"
#endif

#include <stdio.h>

#include "LmItemGenDB.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmItemGenDB.i"
#endif

////
// Constructor
////

LmItemGenDB::LmItemGenDB()
  : pos_(LmPosition::DEFAULT_INSTANCE),
    gen_type_(DEFAULT_GENTYPE),
    base_delay_(DEFAULT_BASEDELAY),
    var_delay_(DEFAULT_VARDELAY),
    item_ttl_(DEFAULT_ITEMTTL),
    var_ttl_(DEFAULT_VARTTL)
{
  int j = 1;
  // empty
}

////
// Destructor
////

LmItemGenDB::~LmItemGenDB()
{
  // empty
}

////
// Init
////

void LmItemGenDB::Init(int d_base, int d_var, int l_base, int l_var, int x, int y, int z, int angle, int gen_type)
{
  gen_type_ = gen_type;
  base_delay_ = d_base;
  var_delay_ = d_var;
  item_ttl_ = l_base;
  var_ttl_ = l_var;
  pos_.Init(x, y, z, angle);
}

////
// Dump
////

void LmItemGenDB::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmItemGenDB[%p,%d]: type=%d pos="), this, sizeof(LmItemGenDB), GeneratorType());
  Position().Dump1(f);
 _ftprintf(f, _T(" delay=%d+%d ttl=%d+%d>\n"), BaseDelay(), VarDelay(), ItemTTL(), VarTTL());
}
