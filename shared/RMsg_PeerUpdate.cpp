// RMsg_PeerUpdate.cpp  -*- C++ -*-
// $Id: RMsg_PeerUpdate.cpp,v 1.10 1997-11-06 18:21:32-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "RMsg_PeerUpdate.h"
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

#include "RMsg_PeerUpdate.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "RMsg_PeerUpdate.i"
#endif

////
// Constructor
////

RMsg_PeerUpdate::RMsg_PeerUpdate()
  : LmMesg(RMsg::PING, sizeof(data_t), sizeof(data_t), &data_)
{
	Init(LmPeerUpdate::DEFAULT_INSTANCE);
}

////
// Init
////

void RMsg_PeerUpdate::Init(const LmPeerUpdate& update)
{
	SetUpdate(update);
}


////
// hton
////

void RMsg_PeerUpdate::hton()
{
  data_.update.ConvertToNetwork();
}

////
// ntoh
////

void RMsg_PeerUpdate::ntoh()
{
  data_.update.ConvertToHost();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void RMsg_PeerUpdate::Dump(FILE* f, int indent) const
{
  data_.update.Dump(f, indent);
  // print out base class
  LmMesg::Dump(f, indent + 1);

}
#endif /* USE_DEBUG */

