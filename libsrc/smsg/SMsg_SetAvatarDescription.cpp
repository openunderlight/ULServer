// SMsg_SetAvatarDescription.cpp  -*- C++ -*-
// $Id: SMsg_SetAvatarDescription.cpp,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_SetAvatarDescription.h"
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

#include "SMsg_SetAvatarDescription.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_SetAvatarDescription.i"
#endif

////
// constructor
////

SMsg_SetAvatarDescription::SMsg_SetAvatarDescription()
  : LmMesg(SMsg::SETAVATARDESCRIPTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(Lyra::ID_UNKNOWN, _T("none"));
}

////
// destructor
////

SMsg_SetAvatarDescription::~SMsg_SetAvatarDescription()
{
  // empty
}

////
// Init
////

void SMsg_SetAvatarDescription::Init(lyra_id_t playerid, const TCHAR* description)
{
  SetPlayerID(playerid);
  SetDescription(description);
}

////
// hton
////

void SMsg_SetAvatarDescription::hton()
{
  HTONL(data_.playerid);
  // not converted: Description
}

////
// ntoh
////

void SMsg_SetAvatarDescription::ntoh()
{
  NTOHL(data_.playerid);
  // not converted: Description
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_SetAvatarDescription::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_SetAvatarDescription[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("player=%u desc='%s'>\n"), PlayerID(), Description());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// SetDescription
////

void SMsg_SetAvatarDescription::SetDescription(const TCHAR* description)
{
 _tcsnccpy(data_.description, description, sizeof(data_.description));
  TRUNC(data_.description, sizeof(data_.description));
  calc_size();
}

////
// calc_size
////

void SMsg_SetAvatarDescription::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.description);
  // add string length, plus 1 for null
  size += (_tcslen(data_.description) + 1);
  SetMessageSize(size);
}
