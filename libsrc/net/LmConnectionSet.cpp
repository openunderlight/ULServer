// LmConnectionSet.cpp  -*- C++ -*-
// $Id: LmConnectionSet.cpp,v 1.19 1997-08-25 15:47:13-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmConnectionSet.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LmConnectionSet.h"
#include "LyraDefs.h"
#include "LmConnection.h"
#include "PThMutex.h"
#include "LmConnectionSetImp.h"
#include "LmConnectionList.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LmConnectionSet.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LmConnectionSet::LmConnectionSet(int setsize)
  : max_conns_(setsize),
    conns_(LmNEW(LmConnection[setsize])),
    imp_(LmNEW(LmConnectionSetImp()))
{
  DECLARE_TheLineNum;
  lock_.Init();
}

////
// Destructor
////

LmConnectionSet::~LmConnectionSet()
{
  DECLARE_TheLineNum;
  // TODO: close all first?
  LmDELETE(imp_);
  LmDELETEARRAY(conns_);
}

////
// NumConnections - return number of connections of a given type (0 = all)
////

int LmConnectionSet::NumConnections(int ctype) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  return imp_->NumConnections(ctype);
}

////
// AllocateConnection: return pointer to connection, or 0 if none available
////

LmConnection* LmConnectionSet::AllocateConnection(LmSocket& sock)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  // find a connection that is currently unused
  LmConnection* conn = 0;
  for (int i = 0; i < max_conns_; ++i) {
    if (!conns_[i].IsConnected()) {
      conn = &conns_[i];
      // initialize, add to map
      conn->Init(sock);
      imp_->AddConnection(conn);
      break;
    }
  }
  return conn;
}

////
// RemoveConnection
////

void LmConnectionSet::RemoveConnection(LmConnection* conn)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  // remove from map, and disconnect
  imp_->RemoveConnection(conn);
  conn->Disconnect();
  // reset connection type/id
  conn->SetType(LmConnection::CT_UNKNOWN);
  conn->SetID(Lyra::ID_UNKNOWN);
}

////
// GetConnection: return 0 if not found
////

LmConnection* LmConnectionSet::GetConnection(int conn_type, int conn_id) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  return imp_->GetConnection(conn_type, conn_id);
}

LmConnection* LmConnectionSet::GetConnection(int sockfd) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  return imp_->GetConnection(sockfd);
}

////
// UpdateConnection: change type/id for given connection
////

void LmConnectionSet::UpdateConnection(LmConnection* conn, int new_type, int new_id)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  // remove from map
  imp_->RemoveConnection(conn);
  // update
  conn->SetType(new_type);
  conn->SetID(new_id);
  // add to map
  imp_->AddConnection(conn);
}

////
// HasConnection
////

bool LmConnectionSet::HasConnection(LmConnection* conn) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  return imp_->HasConnection(conn);
}

bool LmConnectionSet::HasConnection(int conn_type, int conn_id) const
{
  DECLARE_TheLineNum;
  // Note: don't lock
  return (GetConnection(conn_type, conn_id) != 0);
}

////
// CloseAll: close all connections, erase from set
////

void LmConnectionSet::CloseAll()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  // get connection list
  LmConnectionList conn_list;
  imp_->GetConnectionList(conn_list);
  // empty map
  imp_->Empty();
  // close them all
  LmConnectionList::iterator i = conn_list.begin();
  while (1) {
	if (i == conn_list.end()) 
		  break;
    LmConnection* conn = (*i);
    conn->Disconnect();
	++i;
  }
}

////
// FillSet: fill fd_set with connections that are currently not processing
//   a message; return maximum descriptor value
////

int LmConnectionSet::FillSet(fd_set* fdset) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  return imp_->FillSet(fdset);
}

////
// GetConnectionList: fill list with all active connections
////

void LmConnectionSet::GetConnectionList(LmConnectionList& conn_list) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  imp_->GetConnectionList(conn_list);
}

////
// Dump
////

void LmConnectionSet::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for this method's duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmConnectionSet[%p,%d]: max=%d conns=[%p]>\n"), this, sizeof(LmConnectionSet),
	  max_conns_, conns_);
  imp_->Dump(f, indent + 1);
}
