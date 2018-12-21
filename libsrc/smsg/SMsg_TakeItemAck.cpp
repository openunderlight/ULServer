// SMsg_TakeItemAck.cpp  -*- C++ -*-
// $Id: SMsg_TakeItemAck.cpp,v 1.1 1997-09-18 16:49:05-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_TakeItemAck.h"
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

#include "SMsg_TakeItemAck.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_TakeItemAck.i"
#endif

////
// constructor
////

SMsg_TakeItemAck::SMsg_TakeItemAck()
  : LmMesg(SMsg::TAKEITEMACK, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, 0, 0, LmItemHdr::DEFAULT_INSTANCE);
}

////
// destructor
////

SMsg_TakeItemAck::~SMsg_TakeItemAck()
{
  // empty
}

////
// Init
////

void SMsg_TakeItemAck::Init(int status, lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& itemheader)
{
  SetStatus(status);
  SetSourceID(sourceid);
  SetTargetID(targetid);
  SetItemHeader(itemheader);
}

////
// hton
////

void SMsg_TakeItemAck::hton()
{
  HTONL(data_.status);
  HTONL(data_.sourceid);
  HTONL(data_.targetid);
  data_.itemheader.ConvertToNetwork();
}

////
// ntoh
////

void SMsg_TakeItemAck::ntoh()
{
  NTOHL(data_.status);
  NTOHL(data_.sourceid);
  NTOHL(data_.targetid);
  data_.itemheader.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_TakeItemAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_TakeItemAck[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("data>\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */
