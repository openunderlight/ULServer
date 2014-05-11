// LmSrvMesgBuf.h  -*- C++ -*-
// $Id: LmSrvMesgBuf.h,v 1.3 1997-08-12 23:14:16-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message buffer class - server use (extra server-only functionality)

#ifndef INCLUDED_LmSrvMesgBuf
#define INCLUDED_LmSrvMesgBuf

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LmMesgBuf.h"
#include "LmMesgHdr.h"

// forward declarations

class LmMesg;
class LmThread;

// message buffer

class LmSrvMesgBuf : public LmMesgBuf {

public:

  // constants
  enum {
    DEFAULT_MSG_SIZE = 64,
    DEFAULT_USECOUNT = 1
  };

public:

  LmSrvMesgBuf(int msize = DEFAULT_MSG_SIZE, int use = DEFAULT_USECOUNT);
  virtual ~LmSrvMesgBuf();

  // reference counter methods
  int UseCount() const;
  void SetUseCount(int use);
  void AddReference();
  void RemoveReference();

  // statistics
  int TotalUses() const;
  time_t CreationTime() const;
  time_t LastUseTime() const;
  void SetUseTime(time_t usetime);

  void Dump(FILE* f, int indent = 0) const;

private:

  // operations/methods not implemented
  LmSrvMesgBuf(const LmSrvMesgBuf&);
  //operator=(const LmSrvMesgBuf&);

  // reference counter
  int use_count_;

  // statistics
  int total_uses_;
  time_t creation_time_;
  time_t last_use_;

};

#ifdef USE_INLINE
#include "LmSrvMesgBuf.i"
#endif

#endif /* INCLUDED_LmSrvMesgBuf */
