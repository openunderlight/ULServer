// LmMesgBufPool.cpp  -*- C++ -*-
// $Id: LmMesgBufPool.cpp,v 1.14 1997-10-30 12:38:51-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation of LmMesgBufPool

#ifdef __GNUC__
#pragma implementation "LmMesgBufPool.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include <time.h>
#include "LmMesgBufPool.h"
#include "LmMesgBufPoolImp.h"
#include "PThMutex.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LmMesgBufPool.i"
#endif

////
// constructor
////

LmMesgBufPool::LmMesgBufPool(int maxsparebufs)
  : imp_(LmNEW(LmMesgBufPoolImp(maxsparebufs))),
    num_allocated_(0),
    num_returned_(0)
{
  lock_.Init();
}

////
// destructor
////

LmMesgBufPool::~LmMesgBufPool()
{
  LmDELETE(imp_);
}

////
// AllocateBuffer
////

LmSrvMesgBuf* LmMesgBufPool::AllocateBuffer(int msize, int use_count)
{
  LmLocker mon(lock_); // lock object for method duration
  num_allocated_ += use_count;
  LmSrvMesgBuf* buf = imp_->AllocateBuffer(msize, use_count);
  return buf;
}

////
// ReturnBuffer
////

void LmMesgBufPool::ReturnBuffer(LmSrvMesgBuf* pbuf)
{
  LmLocker mon(lock_); // lock object for method duration
  num_returned_++;
  imp_->ReturnBuffer(pbuf);
}

////
// FreeOldBuffers
////

int LmMesgBufPool::FreeOldBuffers(int age)
{
  LmLocker mon(lock_); // lock object for method duration
  return imp_->FreeOldBuffers(age);
}

////
// Dump: print to FILE stream
////

void LmMesgBufPool::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmMesgBufPool[%p,%d]: imp=[%p] alloced=%d returned=%d>\n"), this, sizeof(LmMesgBufPool),
	  imp_, num_allocated_, num_returned_);
  imp_->Dump(f, indent + 1);
}
