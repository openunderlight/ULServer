// LmConnectionSet.h  -*- C++ -*-
// $Id: LmConnectionSet.h,v 1.12 1997-09-09 02:52:56-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// manages a set of connections

#ifndef INCLUDED_LmConnectionSet
#define INCLUDED_LmConnectionSet

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#ifdef WIN32
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */

#include "LyraDefs.h"
#include "PThMutex.h"

// class forward declarations

class LmConnection;
class LmSocket;
class LmConnectionList;
class LmConnectionSetImp;

// class declarations

class LmConnectionSet {

public:

  LmConnectionSet(int setsize);
  ~LmConnectionSet();

  LmConnection* AllocateConnection(LmSocket& sock);
  void RemoveConnection(LmConnection* conn);
  LmConnection* GetConnection(int conn_type, int conn_id) const;
  LmConnection* GetConnection(int sockfd) const;
  void UpdateConnection(LmConnection* conn, int new_type, int new_id);
  bool HasConnection(LmConnection* conn) const;
  bool HasConnection(int conn_type, int conn_id) const;
  void CloseAll();
  int FillSet(fd_set* fdset) const;
  void GetConnectionList(LmConnectionList& conn_list) const;
  int NumConnections(int ctype = 0) const;
  int MaxConnections() const;

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmConnectionSet(const LmConnectionSet&);
  //operator=(const LmConnectionSet&);

  PThMutex lock_;        // object lock
  LmConnectionSetImp* imp_;  // map of active connections (private implementation)
  LmConnection* conns_;      // array of all available connections
  int max_conns_;            // size of array

};

#ifdef USE_INLINE
#include "LmConnectionSet.i"
#endif

#endif /* INCLUDED_LmConnectionSet */
