// SMsg_DBQuery.cpp  -*- C++ -*-
// $Id: SMsg_DBQuery.cpp,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_DBQuery.h"
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
#include "LyraDefs.h"
#include "SMsg.h"

#ifndef USE_INLINE
#include "SMsg_DBQuery.i"
#endif

////
// constructor
////

SMsg_DBQuery::SMsg_DBQuery()
  : LmMesg(SMsg::DS_DB_QUERY, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  Init(0, DBQUERY_NO_RESULT, SERVER_DB, 0, _T(""));
}

////
// destructor
////

SMsg_DBQuery::~SMsg_DBQuery()
{
  // empty
}

////
// Init
////

void SMsg_DBQuery::Init(lyra_id_t player_id, unsigned char result, unsigned char db, 
			unsigned char thread_type, const TCHAR* query)
{
  SetResult(result);
  SetID(player_id);
  SetThreadType(thread_type);
  SetDB(db);
  SetQuery(query);
}

////
// hton
////

void SMsg_DBQuery::hton()
{
  // internal message - not used
  calc_size();
}

////
// ntoh
////

void SMsg_DBQuery::ntoh()
{
  // internal message - not used
  calc_size();
}

////
// Dump: print to FILE stream
////

#ifdef USE_DEBUG
void SMsg_DBQuery::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_DBQuery[%p]: "), this);
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("result=%u db=%u thread=%u id=%u query'%s'>\n"), Result(), DB(), ThreadType(), ID(), Query());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}
#endif /* USE_DEBUG */

////
// Setquery
////

void SMsg_DBQuery::SetQuery(const TCHAR* query)
{
  _tcsnccpy(data_.query, query, sizeof(data_.query));
  TRUNC(data_.query, sizeof(data_.query));
  calc_size();
}

////
// calc_size
////

void SMsg_DBQuery::calc_size()
{
  // initial size: whole structure minus variable-length text
  int size = sizeof(data_t) - sizeof(data_.query);
  // add string length, plus 1 for null
  size += (_tcslen(data_.query) + 1);
  SetMessageSize(size);
}
