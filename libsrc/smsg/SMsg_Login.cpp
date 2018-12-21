// SMsg_Login.cpp  -*- C++ -*-
// $Id: SMsg_Login.cpp,v 1.7 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_Login.h"
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

#include "SMsg_Login.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_Login.i"
#endif

////
// constructor
////

SMsg_Login::SMsg_Login()
  : LmMesg(SMsg::LOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init('U', 0);
}

////
// destructor
////

SMsg_Login::~SMsg_Login()
{
  // empty
}

////
// Init
////

void SMsg_Login::Init(int servertype, lyra_id_t id)
{
  SetServerType(servertype);
  SetID(id);
}

////
// hton
////

void SMsg_Login::hton()
{
  HTONL(data_.servertype);
  HTONL(data_.id);
}

////
// ntoh
////

void SMsg_Login::ntoh()
{
  NTOHL(data_.servertype);
  NTOHL(data_.id);
}

////
// Dump: print to FILE stream
////

void SMsg_Login::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_Login[%p,%d]: "), this, sizeof(SMsg_Login));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("servertype=%c id=%u>\n"), ServerType(), ID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
