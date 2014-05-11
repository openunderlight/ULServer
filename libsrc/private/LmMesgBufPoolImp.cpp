// LmMesgBufPoolImp.cpp  -*- C++ -*-
// $Id: LmMesgBufPoolImp.cpp,v 1.14 1997-10-30 12:38:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation of LmMesgBufPoolImp

#ifdef __GNUC__
#pragma implementation "LmMesgBufPoolImp.h"
#endif

#include <stdio.h>

#include <list>
#include <algo.h>
#include <time.h>

#include "LmMesgBufPoolImp.h"
#include "LmSrvMesgBuf.h"

#ifndef USE_INLINE
#include "LmMesgBufPoolImp.i"
#endif

DECLARE_TheFileName;

////
// constructor
////

LmMesgBufPoolImp::LmMesgBufPoolImp(int maxsparebufs)
  : max_sparebufs_(maxsparebufs),
    num_allocated_(0),
    num_deleted_(0),
    num_ret_rogue_(0)
{
  DECLARE_TheLineNum;
  // empty
}

////
// destructor
////

LmMesgBufPoolImp::~LmMesgBufPoolImp()
{
  DECLARE_TheLineNum;
  // delete buffers in each list
  buflist_t::iterator i;
  for (i = spare_.begin(); !(bool)(i == spare_.end()); ++i) {
    LmDELETE(*i);
  }
  for (i = used_.begin(); !(bool)(i == used_.end()); ++i) {
    LmDELETE(*i);
  }
}

////
// AllocateBuffer: return a message buffer large enough to hold a
//   message of the specified size; buffer may be returned from the
//   spare pool or allocated if needed.
////

class BufferLargeEnough {
private:
  int size_;
public:
  BufferLargeEnough(int size) {
    size_ = size;
  }
  bool operator()(const LmSrvMesgBuf* b) {
    return (b->MaxMessageSize() >= size_);
  }
};
  
LmSrvMesgBuf* LmMesgBufPoolImp::AllocateBuffer(int msize, int use_count)
{
  DECLARE_TheLineNum;
  // search for buffer large enough in spare list
  buflist_t::iterator i = find_if(spare_.begin(), spare_.end(), BufferLargeEnough(msize));
  // found?
  LmSrvMesgBuf* buf;
  if (!(bool)(i == spare_.end())) {
    buf = *i;
    buf->SetUseCount(use_count);
    // remove from spare list
    spare_.erase(i);
  }
  else {
    // allocate a new buffer
    buf = LmNEW(LmSrvMesgBuf(msize, use_count));
    num_allocated_++;
  }
  // put buffer in used list, and return it
  used_.push_front(buf);
  // set last use time
  buf->SetUseTime(time(NULL));
  return buf;
}

////
// ReturnBuffer: return buffer to spare list, and compact the spare
//   list if needed.  If buffer wasn't in used pool, put in spare list.
//   If buffer's use count is nonzero, simply decrement and leave in used list.
////

void LmMesgBufPoolImp::ReturnBuffer(LmSrvMesgBuf* pbuf)
{
  DECLARE_TheLineNum;
  // find in used list
  buflist_t::iterator i = find(used_.begin(), used_.end(), pbuf);
  // if found
  if (!(bool)(i == used_.end())) {
    pbuf->RemoveReference();
    if (pbuf->UseCount() == 0) {
      // remove from used list
      used_.erase(i);
    }
  }
  else {
    num_ret_rogue_++;
  }
  // add to spare list if use count is zero (even if buffer wasn't allocated by the pool;
  // otherwise a memory leak occurs), and if it isn't already on the spare list (an error, alas)
  if (pbuf->UseCount() == 0) {
    i = find(spare_.begin(), spare_.end(), pbuf);
    if (i == spare_.end()) { // not found
      spare_.push_front(pbuf);
    }
  }
  compact_spare_list();
}

////
// FreeOldBuffers: check last usage time on buffers in used list; if
//   any were last used more than the given deadline seconds ago, move
//   them to the spare list; return number freed
////

int LmMesgBufPoolImp::FreeOldBuffers(int deadline)
{
  DECLARE_TheLineNum;
  int num_freed = 0;
  time_t nuke_time = time(NULL) - deadline;
  buflist_t freed_list;
  // put buffers to free in freed_list (avoid modifying used_ within the loop)
  buflist_t::iterator i;
  for (i = used_.begin(); !(bool)(i == used_.end()); ++i) {
    LmSrvMesgBuf* buf = *i;
    if (buf->LastUseTime() < nuke_time) {
      freed_list.push_front(buf);
    }
  }
  // free buffers in freed_list
  for (i = freed_list.begin(); !(bool)(i == freed_list.end()); ++i) {
    LmSrvMesgBuf* buf = *i;
    buf->SetUseCount(1);
    ReturnBuffer(buf);
    num_freed++;
  }
  return num_freed;
}

////
// SetMaxSpareBuffers: set the maximum size of the spare buffer pool,
//   and compact the pool if needed (ie. if it's larger than the
//   given size).
////

void LmMesgBufPoolImp::SetMaxSpareBuffers(int maxsparebufs)
{
  DECLARE_TheLineNum;
  max_sparebufs_ = maxsparebufs;
  // sanity check
  if (max_sparebufs_ < 0) {
    max_sparebufs_ = 0;
  }
  compact_spare_list();
}

////
// Dump: print to FILE stream
////

void LmMesgBufPoolImp::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmMesgBufPoolImp[%p,%d]: used=%d spare=%d/%d alloced=%d freed=%d rogue=%d ok=%d>\n"),
	  this, sizeof(LmMesgBufPoolImp),
	  NumUsedBuffers(), NumSpareBuffers(), MaxSpareBuffers(),
	  num_allocated_, num_deleted_, num_ret_rogue_, CheckIntegrity());
  ++indent;
  if (used_.size() > 0) {
    INDENT(indent, f);
   _ftprintf(f, _T("used:\n"));
    for (buflist_t::const_iterator i = used_.begin(); !(bool)(i == used_.end()); ++i) {
      (*i)->Dump(f, indent + 1);
    }
  }
  if (spare_.size() > 0) {
    INDENT(indent, f);
   _ftprintf(f, _T("spare:\n"));
    for (buflist_t::const_iterator i = spare_.begin(); !(bool)(i == spare_.end()); ++i) {
      (*i)->Dump(f, indent + 1);
    }
  }
}

////
// CheckIntegrity: check structure for any problems, return 1 if OK, < 0 if not
////

int LmMesgBufPoolImp::CheckIntegrity() const
{
  DECLARE_TheLineNum;
  // check that number of spare buffers is less than maximum allowed
  if (spare_.size() > max_sparebufs_) {
    return -1;
  }
  // check that spare list has no duplicates
  buflist_t tmp_spare = spare_;
  tmp_spare.sort();
  if (!(bool)(adjacent_find(tmp_spare.begin(), tmp_spare.end()) == tmp_spare.end())) {
    return -2;
  }
  // check that used list has no duplicates
  buflist_t tmp_used = used_;
  tmp_used.sort();
  if (!(bool)(adjacent_find(tmp_used.begin(), tmp_used.end()) == tmp_used.end())) {
    return -3;
  }
  // check that used and spare lists have no common buffers
  for (buflist_t::iterator i = tmp_spare.begin(); !(bool)(i == tmp_spare.end()); ++i) {
    if (!(bool)(find(tmp_used.begin(), tmp_used.end(), *i) == tmp_used.end())) {
      return -4;
    }
  }
  // all checks OK
  return 1;
}

////
// compact_spare_list: if there are more spare buffers than max_sparebufs_, remove some
////

void LmMesgBufPoolImp::compact_spare_list()
{
  DECLARE_TheLineNum;

  // until the number of spare buffers is less than max_sparebufs_,
  // remove one from the pool, and free it
  while ((spare_.size() > max_sparebufs_) && (spare_.size() > 0)) {
    LmSrvMesgBuf* buf = spare_.front();
    spare_.pop_front();
    LmDELETE(buf);
    num_deleted_++;
  }
}
