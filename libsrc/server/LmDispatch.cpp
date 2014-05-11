// LmDispatch.cpp  -*- C++ -*-
// $Id: LmDispatch.cpp,v 1.16 1997-08-20 01:34:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmDispatch.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <map>

#include "LmDispatch.h"
#include "LmMesg.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmThread.h"
#include "LmConnectionList.h"
#include "LyraMessage.h"

#ifndef USE_INLINE
#include "LmDispatch.i"
#endif

DECLARE_TheFileName;

////
// LmDispatchTable implementation
////

class LmDispatchTable : public std::map< int, int, std::less<int> > {
  // nothing extra
};

////
// Constructor
////

LmDispatch::LmDispatch(LmMesgBufPool* bufpool)
  : bufpool_(bufpool),
    dt_(LmNEW(LmDispatchTable()))
{
  DECLARE_TheLineNum;
  // empty
}

////
// Destructor
////

LmDispatch::~LmDispatch()
{
  DECLARE_TheLineNum;
  LmDELETE(dt_);
}

////
// DispatchMessage: send message to appropriate thread; return 0 if successful, -1 otherwise
////

int LmDispatch::DispatchMessage(LmSrvMesgBuf* mbuf, LmConnection* conn)
{
  DECLARE_TheLineNum;
  LmThread* thread = ComputeTarget(mbuf, conn);
  if (thread) {
    thread->PassMessage(mbuf, conn);
    return 0;
  }
  // thread not found, oops
  return -1;
}

int LmDispatch::DispatchMessage(LmMesg* msg, LmConnection* conn)
{
  DECLARE_TheLineNum;
  // create message buffer for input message
  LmSrvMesgBuf* mbuf;
  if (bufpool_) {
    // allocate message buffer from pool if it exists
    mbuf = bufpool_->AllocateBuffer(msg->MessageSize());
  }
  else {
    // create a new one, which (it is assumed) will be deleted by recipient thread!
    // (this is the default behavior of LmThreads in the absence of a buffer pool)
    mbuf = LmNEW(LmSrvMesgBuf(msg->MessageSize()));
  }
  mbuf->ReadMessage(*msg);
  return DispatchMessage(mbuf, conn);
}

int LmDispatch::DispatchMessage(LmSrvMesgBuf* mbuf, LmConnectionList& conn_list)
{
  DECLARE_TheLineNum;
  // quick check
  if (conn_list.size() == 0) {
    return 0;
  }
  // remove any duplicates from connection list
  conn_list.sort();
  conn_list.unique();
  // initialize message buffer's use count
  mbuf->SetUseCount(conn_list.size());
  // send message to each target
  int retval = 0;
  for (LmConnectionList::iterator i = conn_list.begin(); !(bool)(i == conn_list.end()); ++i) {
    if (DispatchMessage(mbuf, (*i)) < 0) {
      // TODO: reduce use count?
      retval = -1;
    }
  }
  // return 0 if all were successful, -1 otherwise
  return retval;
}

int LmDispatch::DispatchMessage(LmMesg* msg, LmConnectionList& conn_list)
{
  DECLARE_TheLineNum;
  // quick check
  if (conn_list.size() == 0) {
    return 0;
  }
  // create message buffer for input message
  LmSrvMesgBuf* mbuf;
  if (bufpool_) {
    // allocate message buffer from pool if it exists
    mbuf = bufpool_->AllocateBuffer(msg->MessageSize());
  }
  else {
    // create a new one, which (it is assumed) will be deleted by recipient thread!
    // (this is the default behavior of LmThreads in the absence of a buffer pool)
    mbuf = LmNEW(LmSrvMesgBuf(msg->MessageSize()));
  }
  mbuf->ReadMessage(*msg);
  return DispatchMessage(mbuf, conn_list);
}

////
// ComputeTarget: return pointer to thread willing to accept message, or null
//   if none exists.  This is the useless default implementation.
////

LmThread* LmDispatch::ComputeTarget(LmSrvMesgBuf* /* mbuf */, LmConnection* /* conn */)
{
  DECLARE_TheLineNum;
  return 0;
}

////
// AddMapping: add mapping from into dispatch table (convention: target is actually
//   a printable character, and cannot be 0)
////

void LmDispatch::AddMapping(int mesgtype, int target)
{
  DECLARE_TheLineNum;
  (*dt_)[mesgtype] = target;
}

////
// AddMappings: add an array of mappings into table
////

void LmDispatch::AddMappings(const int mesgtypes[], int num_types, int target)
{
  DECLARE_TheLineNum;
  for (int i = 0; i < num_types; ++i) {
    AddMapping(mesgtypes[i], target);
  }
}

////
// GetTarget: return target from dispatch table, or 0 if not in table
////

int LmDispatch::GetTarget(int mesgtype) const
{
  DECLARE_TheLineNum;
  LmDispatchTable::iterator i = dt_->find(mesgtype);
  int retval = 0;
  if (!(bool)(i == dt_->end())) {
    retval = (*i).second;
  }
  return retval;
}

////
// Dump
////

void LmDispatch::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmDispatch[%p,%d]: dt=[%p] bufpool=[%p]>\n"), this, sizeof(LmDispatch), dt_, bufpool_);
  for (LmDispatchTable::iterator i = dt_->begin(); !(bool)(i == dt_->end()); ++i) {
    INDENT(indent + 1, f);
    int mtype = (*i).first;
   _ftprintf(f, _T("mesg %s (%d) -> %c\n"), LyraMessage::MessageName(mtype), mtype, (*i).second);
  }
}
