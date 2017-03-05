// SMsg_DS_Action.cpp  -*- C++ -*-
// $Id: SMsg_DS_Action.cpp,v 1.6 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_DS_Action.h"
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

#include "SMsg_DS_Action.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_DS_Action.i"
#endif

////
// constructor
////

SMsg_DS_Action::SMsg_DS_Action()
  : LmMesg(SMsg::DS_ACTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(ACTION_NONE);
}

////
// destructor
////

SMsg_DS_Action::~SMsg_DS_Action()
{
  // empty
}

////
// Init
////

void SMsg_DS_Action::Init(int action)
{
  SetAction(action);
}

////
// hton
////

void SMsg_DS_Action::hton()
{
  // internal -- not used
}

////
// ntoh
////

void SMsg_DS_Action::ntoh()
{
  // internal -- not used
}

////
// Dump: print to FILE stream
////

void SMsg_DS_Action::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_DS_Action[%p,%d]: "), this, sizeof(SMsg_DS_Action));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("action=%c>\n"), Action());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
