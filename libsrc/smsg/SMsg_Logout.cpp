// SMsg_Logout.cpp  -*- C++ -*-
// $Id: SMsg_Logout.cpp,v 1.6 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_Logout.h"
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

#include "SMsg_Logout.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_Logout.i"
#endif

////
// constructor
////

SMsg_Logout::SMsg_Logout()
  : LmMesg(SMsg::LOGOUT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  //Init();
}

////
// destructor
////

SMsg_Logout::~SMsg_Logout()
{
  // empty
}

////
// Init
////

void SMsg_Logout::Init()
{
  // nothing to init
}

////
// hton
////

void SMsg_Logout::hton()
{
  // nothing to convert
}

////
// ntoh
////

void SMsg_Logout::ntoh()
{
  // nothing to convert
}

////
// Dump: print to FILE stream
////

void SMsg_Logout::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_Logout[%p,%d]: "), this, sizeof(SMsg_Logout));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("(no data)>\n"));
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
