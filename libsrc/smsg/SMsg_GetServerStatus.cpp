// SMsg_GetServerStatus.cpp  -*- C++ -*-
// $Id: SMsg_GetServerStatus.cpp,v 1.1 1997-09-09 01:55:51-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_GetServerStatus.h"
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

#include "SMsg_GetServerStatus.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_GetServerStatus.i"
#endif

////
// constructor
////

SMsg_GetServerStatus::SMsg_GetServerStatus()
  : LmMesg(SMsg::GETSERVERSTATUS, sizeof(data_t), sizeof(data_t), &data_)
{
  Init();
}

////
// destructor
////

SMsg_GetServerStatus::~SMsg_GetServerStatus()
{
  // empty
}

////
// Init
////

void SMsg_GetServerStatus::Init(int status, lyra_id_t id)
{
  SetStatus(status);
  SetID(id);
}

////
// hton
////

void SMsg_GetServerStatus::hton()
{
  HTONL(data_.status);
  HTONL(data_.id);
}

////
// ntoh
////

void SMsg_GetServerStatus::ntoh()
{
  NTOHL(data_.status);
  NTOHL(data_.id);
}

////
// Dump: print to FILE stream
////

void SMsg_GetServerStatus::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_GetServerStatus[%p,%d]: "), this, sizeof(SMsg_GetServerStatus));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("status=%c id=%u>\n"), Status(), ID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
