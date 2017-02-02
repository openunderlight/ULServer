// SMsg_DumpState.cpp  -*- C++ -*-
// $Id: SMsg_DumpState.cpp,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_DumpState.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "SMsg_DumpState.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_DumpState.i"
#endif

////
// constructor
////

SMsg_DumpState::SMsg_DumpState()
  : LmMesg(SMsg::DUMP_STATE, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

SMsg_DumpState::~SMsg_DumpState()
{
  // empty
}

////
// Init
////

void SMsg_DumpState::Init(int type)
{
  SetDumpStateType(type);
}

////
// hton
////

void SMsg_DumpState::hton()
{
  HTONL(data_.type);
}

////
// ntoh
////

void SMsg_DumpState::ntoh()
{
  NTOHL(data_.type);
}

////
// Dump: print to FILE stream
////

void SMsg_DumpState::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_DumpState[%p,%d]: "), this, sizeof(SMsg_DumpState));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("type=%d>\n"), DumpStateType());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
