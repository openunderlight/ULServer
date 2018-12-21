// SMsg_ResetPort.cpp  -*- C++ -*-
// $Id: SMsg_ResetPort.cpp,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_ResetPort.h"
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

#include "SMsg_ResetPort.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_ResetPort.i"
#endif

////
// constructor
////

SMsg_ResetPort::SMsg_ResetPort()
  : LmMesg(SMsg::RESETPORT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0);
}

////
// destructor
////

SMsg_ResetPort::~SMsg_ResetPort()
{
  // empty
}

////
// Init
////

void SMsg_ResetPort::Init(lyra_id_t playerid, int port)
{
  SetPlayerID(playerid);
  SetPort(port);
}

////
// hton
////

void SMsg_ResetPort::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.port);
}

////
// ntoh
////

void SMsg_ResetPort::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.port);
}

////
// Dump: print to FILE stream
////

void SMsg_ResetPort::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_ResetPort[%p,%d]: "), this, sizeof(SMsg_ResetPort));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("playerid=%u port=%d>\n"), PlayerID(), Port());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
