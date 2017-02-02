// SMsg_LS_Login.cpp  -*- C++ -*-
// $Id: SMsg_LS_Login.cpp,v 1.6 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_LS_Login.h"
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

#include "SMsg_LS_Login.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_LS_Login.i"
#endif

////
// constructor
////

SMsg_LS_Login::SMsg_LS_Login()
  : LmMesg(SMsg::LS_LOGIN, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN);
}

////
// destructor
////

SMsg_LS_Login::~SMsg_LS_Login()
{
  // empty
}

////
// Init
////

void SMsg_LS_Login::Init(lyra_id_t playerid)
{
  SetPlayerID(playerid);
}

////
// hton
////

void SMsg_LS_Login::hton()
{
  // internal -- not needed
}

////
// ntoh
////

void SMsg_LS_Login::ntoh()
{
  // internal -- not needed
}

////
// Dump: print to FILE stream
////

void SMsg_LS_Login::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_LS_Login[%p,%d]: "), this, sizeof(SMsg_LS_Login));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u>\n"), PlayerID());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
