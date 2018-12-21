// SMsg_ItemDrop.cpp  -*- C++ -*-
// $Id: SMsg_ItemDrop.cpp,v 1.8 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "SMsg_ItemDrop.h"
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

#include "SMsg_ItemDrop.h"
#include "LyraDefs.h"
#include "SMsg.h"
#include "LmItemHdr.h"

#ifndef USE_INLINE
#include "SMsg_ItemDrop.i"
#endif

////
// constructor
////

SMsg_ItemDrop::SMsg_ItemDrop()
  : LmMesg(SMsg::ITEMDROP, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE, DROP_UNKNOWN);
}

////
// destructor
////

SMsg_ItemDrop::~SMsg_ItemDrop()
{
  // empty
}

////
// Init
////

void SMsg_ItemDrop::Init(lyra_id_t playerid, const LmItemHdr& itemhdr, int status)
{
  SetPlayerID(playerid);
  SetItemHeader(itemhdr);
  SetStatus(status);
}

////
// hton
////

void SMsg_ItemDrop::hton()
{
  HTONL(data_.status);
  HTONL(data_.playerid);
  data_.itemhdr.ConvertToNetwork();
}

////
// ntoh
////

void SMsg_ItemDrop::ntoh()
{
  NTOHL(data_.status);
  NTOHL(data_.playerid);
  data_.itemhdr.ConvertToHost();
}

////
// Dump: print to FILE stream
////

void SMsg_ItemDrop::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_ItemDrop[%p,%d]: "), this, sizeof(SMsg_ItemDrop));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u status=%c>\n"), PlayerID(), Status());
    ItemHeader().Dump(f, indent + 1);
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
