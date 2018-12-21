// SMsg_RotateLogs.cpp  -*- C++ -*-
// $Id: SMsg_RotateLogs.cpp,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_RotateLogs.h"
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

#include "SMsg_RotateLogs.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_RotateLogs.i"
#endif

////
// constructor
////

SMsg_RotateLogs::SMsg_RotateLogs()
  : LmMesg(SMsg::ROTATE_LOGS, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

SMsg_RotateLogs::~SMsg_RotateLogs()
{
  // empty
}

////
// Init
////

void SMsg_RotateLogs::Init(int type)
{
  SetRotateLogsType(type);
}

////
// hton
////

void SMsg_RotateLogs::hton()
{
  HTONL(data_.type);
}

////
// ntoh
////

void SMsg_RotateLogs::ntoh()
{
  NTOHL(data_.type);
}

////
// Dump: print to FILE stream
////

void SMsg_RotateLogs::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_RotateLogs[%p,%d]: "), this, sizeof(SMsg_RotateLogs));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("type=%d>\n"), RotateLogsType());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
