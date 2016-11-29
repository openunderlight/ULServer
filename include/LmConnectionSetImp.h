// LmConnectionSetImp.h  -*- C++ -*-
// $Id: LmConnectionSetImp.h,v 1.4 1997-08-20 01:34:53-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// private implementation of LmConnectionSet

#ifndef INCLUDED_LmConnectionSetImp
#define INCLUDED_LmConnectionSetImp

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <map>

#include "LyraDefs.h"

// class forward declarations

class LmConnection;
class LmConnectionList;

// class declarations

class LmConnectionSetImp {

public:

  LmConnectionSetImp();
  ~LmConnectionSetImp();

  void AddConnection(LmConnection* conn);
  void RemoveConnection(LmConnection* conn);
  LmConnection* GetConnection(int conn_type, int conn_id) const;
  LmConnection* GetConnection(int sockfd) const;
  bool HasConnection(LmConnection* conn) const;
  void Empty();
  int FillSet(fd_set* fdset) const;
  void GetConnectionList(LmConnectionList& conn_list) const;
  int NumConnections(int ctype = 0) const;

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmConnectionSetImp(const LmConnectionSetImp&);
  //operator=(const LmConnectionSetImp&);

  // map key
  struct cs_key_t {
    int conn_type;
    int conn_id;
    // constructor
    cs_key_t(int type = 0, int id = 0) {
      conn_type = type;
      conn_id = id;
    }
    // ordering on keys: by ID's if types are same, otherwise by type
    int operator<(const cs_key_t& right) const {
      return (conn_type == right.conn_type) ?
	(conn_id < right.conn_id) : (conn_type < right.conn_type);
    }
  };

  // (multi)map of connection type/id to a connection pointer
  typedef std::multimap< cs_key_t, LmConnection*, std::less<cs_key_t> > cs_map_t;
  cs_map_t map_;

};

#ifdef USE_INLINE
#include "LmConnectionSetImp.i"
#endif

#endif /* INCLUDED_LmConnectionSetImp */
