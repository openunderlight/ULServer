// LmConnectionSetImp.cpp  -*- C++ -*-
// $Id: LmConnectionSetImp.cpp,v 1.7 1997-08-25 15:48:41-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmConnectionSetImp.h"
#endif


#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LmConnectionSetImp.h"
#include "LmConnection.h"
#include "LmConnectionList.h"

#ifndef USE_INLINE
#include "LmConnectionSetImp.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LmConnectionSetImp::LmConnectionSetImp()
{
  DECLARE_TheLineNum;
  // empty
}

////
// Destructor
////

LmConnectionSetImp::~LmConnectionSetImp()
{
  DECLARE_TheLineNum;
  // empty
}

////
// AddConnection: add connection to map
////

void LmConnectionSetImp::AddConnection(LmConnection* conn)
{
  DECLARE_TheLineNum;
  cs_key_t key(conn->Type(), conn->ID());
  std::pair<const cs_key_t, LmConnection*> val(key, conn);
  map_.insert(val);
}

////
// RemoveConnection: remove connection from map
////

void LmConnectionSetImp::RemoveConnection(LmConnection* conn)
{
	DECLARE_TheLineNum;
	cs_map_t::iterator i = map_.begin();
	while (1) {
		if (i == map_.end())
			break;
		if ((*i).second == conn) {
			map_.erase(i);
			break;
		}
		++i;
	}
}

////
// GetConnection: return 0 if not found
////

LmConnection* LmConnectionSetImp::GetConnection(int conn_type, int conn_id) const
{
  DECLARE_TheLineNum;
  LmConnection* conn = 0;
  // TODO: replace this with more efficient search (use find())
  cs_map_t::const_iterator i = map_.begin();
  while (1) {
	if (i == map_.end())
		break;
    cs_key_t key = (*i).first;
    if ((key.conn_type == conn_type) && (key.conn_id == conn_id)) {
      conn = (*i).second;
      break;
    }
	++i;
  }
  return conn;
}

LmConnection* LmConnectionSetImp::GetConnection(int sockfd) const
{
  DECLARE_TheLineNum;
  LmConnection* conn = 0;
  cs_map_t::const_iterator i = map_.begin();
  // have to use a linear search, for descriptor
  while (1) {
	if (i == map_.end())
		break;
    conn = (*i).second;
    if (conn->Socket().Descriptor() == sockfd) {
      break;
    }
	++i;
  }
  return conn;
}

////
// HasConnection
////

bool LmConnectionSetImp::HasConnection(LmConnection* conn) const
{
  DECLARE_TheLineNum;
  cs_map_t::const_iterator i = map_.begin();
  while (1) {
	if ( i == map_.end())
		break;
    if ((*i).second == conn) {
      return true;
    }
	++i;
  }
  return false;
}

////
// Empty: empty map
////

void LmConnectionSetImp::Empty()
{
  DECLARE_TheLineNum;
  map_.erase(map_.begin(), map_.end());
}

////
// FillSet: fill fd_set with connections that are currently not processing
//   a message; return maximum descriptor value
////

int LmConnectionSetImp::FillSet(fd_set* fdset) const
{
  DECLARE_TheLineNum;
  int maxfd = 0;
  cs_map_t::const_iterator i = map_.begin();
  while (1) {
    if (i == map_.end())
		break; 
    LmConnection* conn = (*i).second;
    if (!conn->MessageBeingRead() && conn->Socket().IsValid() &&
	//	conn->IsConnected()) {
		conn->IsConnected() && //!conn->IsClosed() &&
	   (conn->ID() != LmConnection::DEAD_CONNECTION)) {
      int fd = conn->Socket().Descriptor();
      FD_SET(fd, fdset);
      maxfd = MAX(maxfd, fd);
    }
	++i;
  }
  return maxfd;
}

////
// GetConnectionList: fill list with all active connections
////

void LmConnectionSetImp::GetConnectionList(LmConnectionList& conn_list) const
{
  DECLARE_TheLineNum;
  cs_map_t::const_iterator i = map_.begin();
  while (1) {
	if ( i == map_.end()) 
		break; 
    LmConnection* conn = (*i).second;
    if (conn) {
      conn_list.push_back(conn);
    }
	++i;
  }
}

////
// NumConnections
////

int LmConnectionSetImp::NumConnections(int ctype) const
{
  DECLARE_TheLineNum;
  // return all?
  if (ctype == 0) {
    return map_.size();
  }
  // otherwise, count those of a given type
  int count = 0;
  cs_map_t::const_iterator i = map_.begin();
  while (1) {
	if (i == map_.end())
	  break;
    LmConnection* conn = (*i).second;
    if (conn && (conn->Type() == ctype)) {
      count++;
    }
	++i;
  }
  return count;
}

////
// Dump
////

void LmConnectionSetImp::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LmConnectionSetImp[%p,%d]: active=%d>\n"), this, sizeof(LmConnectionSetImp),
	  NumConnections());
  for (cs_map_t::const_iterator i = map_.begin(); !(i == map_.end()); ++i) {
    LmConnection* conn = (*i).second;
    if (!conn) {
      INDENT(indent + 1, f);
     _ftprintf(f, _T("NULL CONNECTION IN ACTIVE MAP\n"));
    }
    else {
      conn->Dump(f, indent + 1);
    }
  }
}
