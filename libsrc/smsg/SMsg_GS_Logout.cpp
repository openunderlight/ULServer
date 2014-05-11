// SMsg_GS_Logout.cpp  -*- C++ -*-
// $Id: SMsg_GS_Logout.cpp,v 1.8 1997-10-03 18:30:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_GS_Logout.h"
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

#include "SMsg_GS_Logout.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_GS_Logout.i"
#endif

////
// constructor
////

SMsg_GS_Logout::SMsg_GS_Logout()
  : LmMesg(SMsg::GS_LOGOUT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, 0, 0);
}

////
// destructor
////

SMsg_GS_Logout::~SMsg_GS_Logout()
{
  // empty
}

////
// Init
////

void SMsg_GS_Logout::Init(lyra_id_t playerid, int online, int status)
{
  SetPlayerID(playerid);
  SetOnline(online);
  SetStatus(status);
}

////
// hton
////

void SMsg_GS_Logout::hton()
{
  // internal message - not used
}

////
// ntoh
////

void SMsg_GS_Logout::ntoh()
{
  // internal message - not used
}

////
// Dump: print to FILE stream
////

void SMsg_GS_Logout::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
  _ftprintf(f, _T("<SMsg_GS_Logout[%p,%d]: "), this, sizeof(SMsg_GS_Logout));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("playerid=%u online=%d status=%c>\n"), PlayerID(), Online(), Status());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
