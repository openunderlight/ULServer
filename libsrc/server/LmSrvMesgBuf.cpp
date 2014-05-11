// LmSrvMesgBuf.cpp  -*- C++ -*-
// $Id: LmSrvMesgBuf.cpp,v 1.4 1997-09-02 19:02:29-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Implementation of LmSrvMesgBuf class

#ifdef __GNUC__
#pragma implementation "LmSrvMesgBuf.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "LmSrvMesgBuf.h"
#include "LmMesg.h"
#include "LmMesgHdr.h"
#include "LmUtil.h"

#ifndef USE_INLINE
#include "LmSrvMesgBuf.i"
#endif

////
// constructor
////

LmSrvMesgBuf::LmSrvMesgBuf(int msize, int use)
  : LmMesgBuf(msize),
    use_count_(use),
    total_uses_(use)
//    creation_time_(time(NULL)),
//    last_use_(time(NULL))
{
    creation_time_ = time(NULL);
    last_use_ = time(NULL);
  // empty
}

////
// destructor
////

LmSrvMesgBuf::~LmSrvMesgBuf()
{
  // empty
}

////
// Dump: print to given FILE stream
////

void LmSrvMesgBuf::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmSrvMesgBuf[%p,%d]: created=%u (%d) ref=%d total_uses=%d last_use=%u (%d)>\n"),
	  this, sizeof(LmSrvMesgBuf),
	  CreationTime(), LmUtil::TimeSince(CreationTime()), UseCount(), TotalUses(),
	  LastUseTime(), LmUtil::TimeSince(LastUseTime()));
  // base class
  LmMesgBuf::Dump(f, indent + 1);
}
