// SMsg_ShowItem.cpp  -*- C++ -*-
// $Id: SMsg_ShowItem.cpp,v 1.1 1997-12-11 15:11:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_ShowItem.h"
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

#include "SMsg_ShowItem.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_ShowItem.i"
#endif

////
// constructor
////

SMsg_ShowItem::SMsg_ShowItem()
  : LmMesg(SMsg::SHOWITEM, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, Lyra::ID_UNKNOWN, LmItemHdr::DEFAULT_INSTANCE, _T("no name"));
}

////
// destructor
////

SMsg_ShowItem::~SMsg_ShowItem()
{
  // empty
}

////
// Init
////

void SMsg_ShowItem::Init(lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& itemheader, const TCHAR* itemname)
{
  SetSourceID(sourceid);
  SetTargetID(targetid);
  SetItemHeader(itemheader);
  SetItemName(itemname);
}

////
// hton
////

void SMsg_ShowItem::hton()
{
  HTONL(data_.sourceid);
  HTONL(data_.targetid);
  data_.itemheader.ConvertToNetwork();
  // not converted: ItemName
}

////
// ntoh
////

void SMsg_ShowItem::ntoh()
{
  NTOHL(data_.sourceid);
  NTOHL(data_.targetid);
  data_.itemheader.ConvertToHost();
  // not converted: ItemName
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_ShowItem::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_ShowItem[%p]: "), this);
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

////
// SetItemName
////

void SMsg_ShowItem::SetItemName(const TCHAR* itemname)
{
 _tcsnccpy(data_.itemname, itemname, sizeof(data_.itemname));
  TRUNC(data_.itemname, sizeof(data_.itemname));
}
