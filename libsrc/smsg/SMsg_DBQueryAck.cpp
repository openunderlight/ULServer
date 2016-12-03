// SMsg_DBQueryAck.cpp  -*- C++ -*-
// $Id: SMsg_DBQueryAck.cpp,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_DBQueryAck.h"
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

#include "SMsg_DBQuery.h"
#include "SMsg_DBQueryAck.h"
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_DBQueryAck.i"
#endif

////
// constructor
////

SMsg_DBQueryAck::SMsg_DBQueryAck()
  : LmMesg(SMsg::DS_DB_QUERY_ACK_GT, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
	Init(0, SMsg_DBQuery::SERVER_DB, 0, 0, NULL);
}

////
// destructor
////

SMsg_DBQueryAck::~SMsg_DBQueryAck()
{
  // empty
}

////
// Init
////

void SMsg_DBQueryAck::Init(lyra_id_t player_id,  unsigned char db, 
			unsigned char thread_type, unsigned int size, unsigned char* result)
{
  SetResult(result, size);
  SetID(player_id);
  SetThreadType(thread_type);
  SetDB(db);
}

////
// hton
////

void SMsg_DBQueryAck::hton()
{
  // not converted
}

////
// ntoh
////

void SMsg_DBQueryAck::ntoh()
{
  // not converted
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_DBQueryAck::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_DBQuery[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("db=%u thread=%u id=%u size'%d'>\n"), DB(), ThreadType(), ID(), Size());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

void SMsg_DBQueryAck::SetResult(unsigned char* result, unsigned int size)
{
	memcpy(&(data_.result), result, size);
	data_.size = size;
}


////
// calc_size
////

void SMsg_DBQueryAck::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.result);
  // add string length, plus 1 for null
  size += data_.size;
  SetMessageSize(size);
}
