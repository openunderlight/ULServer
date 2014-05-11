// LmIdSet.cpp  -*- C++ -*-
// $Id: LmIdSet.cpp,v 1.10 1997-08-21 15:43:06-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmIdSet.h"
#endif

#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include "LmIdSet.h"

#ifndef USE_INLINE
#include "LmIdSet.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LmIdSet::LmIdSet(int max_size)
  : max_ids_(max_size),
    num_ids_(0),
    ids_(max_size, Lyra::ID_UNKNOWN)
{
  DECLARE_TheLineNum;
  // empty
}

////
// Copy Constructor
////

LmIdSet::LmIdSet(const LmIdSet& idset)
{
  DECLARE_TheLineNum;
  num_ids_ = idset.num_ids_;
  max_ids_ = idset.max_ids_;
  ids_ = idset.ids_;
}

////
// Destructor
////

LmIdSet::~LmIdSet()
{
  DECLARE_TheLineNum;
  // empty
}

////
// Empty
////

void LmIdSet::Empty()
{
  DECLARE_TheLineNum;
  num_ids_ = 0;
  for (int i = 0; i < max_ids_; ++i) {
    ids_[i] = Lyra::ID_UNKNOWN;
  }
}

////
// Add
////

int LmIdSet::Add(lyra_id_t id)
{
  DECLARE_TheLineNum;
  if (IsFull()) {
    return -1;
  }
  // if already there, act as if successful
  if (HasMember(id)) {
    return 0;
  }
  // add
  ids_[num_ids_] = id;
  num_ids_++;
  return 0;
}

////
// Remove
////

int LmIdSet::Remove(lyra_id_t id)
{
  DECLARE_TheLineNum;
  int i = index_of(id);
  if (i == -1) {
    return -1;
  }
  // remove by swapping with last item
  num_ids_--;
  ids_[i] = ids_[num_ids_];
  // clear out last item (necessary if only one element)
  ids_[num_ids_] = Lyra::ID_UNKNOWN;
  return 0;
}

////
// Member
////

lyra_id_t LmIdSet::Member(int index) const
{
  DECLARE_TheLineNum;
  if ((index < 0) || (index >= max_ids_)) {
    return Lyra::ID_UNKNOWN;
  }
  return ids_[index];
}

////
// Dump
////

void LmIdSet::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmIdSet[%p,%d]: size=%d/%d members=["), this, sizeof(LmIdSet), Size(), MaxSize());
  for (int i = 0; i < num_ids_; ++i) {
   _ftprintf(f, _T("%u"), ids_[i]);
    if (i != (num_ids_ - 1)) {
     _ftprintf(f, _T(","));
    }
  }
 _ftprintf(f, _T("]>\n"));
}

////
// index_of
////

int LmIdSet::index_of(lyra_id_t id) const
{
  DECLARE_TheLineNum;
  for (int i = 0; i < num_ids_; ++i) {
    if (ids_[i] == id) {
      return i;
    }
  }
  return -1;
}
