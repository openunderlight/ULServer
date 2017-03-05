// LmSockAddr.h  -*- C++ -*-
// $Id: LmSockAddr.h,v 1.5 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// socket address abstract base class

#ifndef INCLUDED_LmSockAddr
#define INCLUDED_LmSockAddr

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

struct sockaddr;

// the class

class LmSockAddr {

public:

  virtual ~LmSockAddr();

  virtual int Family() const = 0;
  virtual sockaddr* SockAddr() const = 0;
  virtual int SockAddrSize() const = 0;

  virtual LmSockAddr* Copy() const = 0;

  virtual void Dump(FILE* f, int indent = 0) const = 0;
  virtual void Dump1(FILE* f) const = 0;

};

#ifdef USE_INLINE
#include "LmSockAddr.i"
#endif

#endif /* INCLUDED_LmSockAddr */
