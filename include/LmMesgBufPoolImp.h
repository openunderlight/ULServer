// LmMesgBufPoolImp.h  -*- C++ -*-
// $Id: LmMesgBufPoolImp.h,v 1.7 1997-07-18 16:05:17-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message buffer pool implementation (non-locking)

#ifndef INCLUDED_LmMesgBufPoolImp
#define INCLUDED_LmMesgBufPoolImp

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"

// forward declarations

class LmSrvMesgBuf;

// the message buffer class

class LmMesgBufPoolImp {

public:

  LmMesgBufPoolImp(int maxsparebufs);
  ~LmMesgBufPoolImp();

  LmSrvMesgBuf* AllocateBuffer(int msize, int use_count);
  void ReturnBuffer(LmSrvMesgBuf* pbuf);

  int NumSpareBuffers() const;
  int NumUsedBuffers() const;
  int MaxSpareBuffers() const;

  int FreeOldBuffers(int deadline);
  void SetMaxSpareBuffers(int maxsparebufs);

  void Dump(FILE* f, int indent = 0) const;
  int CheckIntegrity() const;

private:

  // not implemented
  LmMesgBufPoolImp(const LmMesgBufPoolImp&);
  //operator=(const LmMesgBufPoolImp&);

  void compact_spare_list();

  // buffer list type
  typedef std::list<LmSrvMesgBuf*> buflist_t;

  int max_sparebufs_;
  buflist_t used_;
  buflist_t spare_;

  int num_allocated_; // total number of message buffers allocated (ie. "new"ed)
  int num_deleted_;   // total number of message buffers deleted
  int num_ret_rogue_; // number of message buffers returned that were not found in used list

};

#ifdef USE_INLINE
#include "LmMesgBufPoolImp.i"
#endif

#endif /* INCLUDED_LmMesgBufPoolImp */
