// LmIdSet.h  -*- C++ -*-
// $Id: LmIdSet.h,v 1.7 1997-08-05 14:58:03-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// non-resizable set of lyra_id_t's

#ifndef INCLUDED_LmIdSet
#define INCLUDED_LmIdSet

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"


#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <pth.h>
#include <vector>

// class forward declarations

// class declarations

class LmIdSet {

public:

  LmIdSet(int max_size);
  LmIdSet(const LmIdSet& idset);
  ~LmIdSet();

  void Empty();
  int Size() const;
  int MaxSize() const;
  bool IsFull() const;
  bool IsEmpty() const;
  bool HasMember(lyra_id_t id) const;
  int Add(lyra_id_t id);
  int Remove(lyra_id_t id);
  lyra_id_t Member(int index) const;

  void Dump(FILE* f, int indent = 0) const;

private:

  int index_of(lyra_id_t id) const;

  short max_ids_;
  short num_ids_;

  typedef std::vector<lyra_id_t> idset_t;
  idset_t ids_;

};

#ifdef USE_INLINE
#include "LmIdSet.i"
#endif

#endif /* INCLUDED_LmIdSet */
