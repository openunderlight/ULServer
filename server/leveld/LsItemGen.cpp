// LsItemGen.cpp  -*- C++ -*-
// $Id: LsItemGen.cpp,v 1.13 1997-09-09 02:58:41-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsItemGen.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "LsItemGen.h"
#include "LyraDefs.h"
#include "LmItemGenDB.h"
#include "LmRand.h"
#include "LmItemGen.h"

#ifndef USE_INLINE
#include "LsItemGen.i"
#endif

////
// Constructor
////

LsItemGen::LsItemGen()
  : last_gen_(0),
    next_gen_(0)
{
  // empty
}

////
// Init
////

void LsItemGen::Init(const LmItemGenDB* db)
{
  db_ = db;
  // bootstrap
  last_gen_ = time(NULL);
  next_gen_ = last_gen_ + LmRand::Generate(1, db_->VarDelay());
}

////
// Generate
////

const LmRoomItem& LsItemGen::Generate(int serial)
{
  // choose random item number
  // int itemnum = (int) rand_r(&seed_) % db_->NumItems();
  // next serial number, with wraparound
  //int delta = (serial_ - db_->SerialStart());
  //delta++;
  //delta = delta % (db_->SerialEnd() - db_->SerialStart());
  // update state
  last_gen_ = time(NULL);
  next_gen_ = last_gen_ + db_->BaseDelay() + LmRand::Generate(0, db_->VarDelay());
  // generate item
  LmItem it; // = db_->Item(itemnum);
  LmItemGen::GenerateItem(db_->GeneratorType(), LmItemGen::ITEM_ANY, it);
  it.Header().SetSerial(serial);
  int ttl = db_->ItemTTL() + LmRand::Generate(0, db_->VarTTL());
  item_.Init(it, db_->Position(), ttl);
  // return reference to data member
  return item_;
}

////
// Dump
////

void LsItemGen::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsItemGen[%p,%d]: db=%p last=%lu next=%lu>\n"), this, sizeof(LsItemGen),
	  db_, last_gen_, next_gen_);
}
