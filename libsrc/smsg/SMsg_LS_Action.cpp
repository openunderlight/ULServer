// SMsg_LS_Action.cpp  -*- C++ -*-
// $Id: SMsg_LS_Action.cpp,v 1.6 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_LS_Action.h"
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

#include "SMsg_LS_Action.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_LS_Action.i"
#endif

////
// constructor
////

SMsg_LS_Action::SMsg_LS_Action()
  : LmMesg(SMsg::LS_ACTION, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(ACTION_NONE, 0);
}

////
// destructor
////

SMsg_LS_Action::~SMsg_LS_Action()
{
  // empty
}

////
// Init
////

void SMsg_LS_Action::Init(int action, short parameter)
{
  SetAction(action);
  SetParameter(parameter);
}

void SMsg_LS_Action::Init(int action)
{
  SetAction(action);
  SetParameter(0);
}

////
// hton
////

void SMsg_LS_Action::hton()
{
  // internal -- not used
}

////
// ntoh
////

void SMsg_LS_Action::ntoh()
{
  // internal -- not used
}

////
// Dump: print to FILE stream
////

void SMsg_LS_Action::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_LS_Action[%p,%d]: "), this, sizeof(SMsg_LS_Action));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("action=%c parameter=%d>\n"), Action(),Parameter());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
