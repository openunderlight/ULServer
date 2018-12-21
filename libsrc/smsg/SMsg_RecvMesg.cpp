// SMsg_RecvMesg.cpp  -*- C++ -*-
// $Id: SMsg_RecvMesg.cpp,v 1.1 1997-08-12 14:38:54-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_RecvMesg.h"
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

#include "SMsg_RecvMesg.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_RecvMesg.i"
#endif

////
// constructor
////

SMsg_RecvMesg::SMsg_RecvMesg()
  : LmMesg(SMsg::RECVMESG, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0);
}

////
// destructor
////

SMsg_RecvMesg::~SMsg_RecvMesg()
{
  // empty
}

////
// Init
////

void SMsg_RecvMesg::Init(LmConnection* conn)
{
  SetConnection(conn);
}

////
// hton
////

void SMsg_RecvMesg::hton()
{
  // nothing -- internal message
}

////
// ntoh
////

void SMsg_RecvMesg::ntoh()
{
  // nothing -- internal message
}

////
// Dump: print to FILE stream
////

void SMsg_RecvMesg::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_RecvMesg[%p,%d]: "), this, sizeof(SMsg_RecvMesg));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("conn=[%p]>\n"), Connection());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
