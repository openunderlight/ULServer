// LmSockType.h  -*- C++ -*-
// $Id: LmSockType.h,v 1.6 1997-10-14 21:39:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// socket type wrapper

#ifndef INCLUDED_LmSockType
#define INCLUDED_LmSockType

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"

// forward declarations

// class declarations

class LmSockType {

public:

  // default ("invalid") instance
  static const LmSockType DEFAULT_INSTANCE;

public:

  LmSockType();
  LmSockType(int family, int type, int protocol);
  LmSockType(int family, int type, const TCHAR* protocol);

  void Init(int family, int type, int protocol);
  void Init(int family, int type, const TCHAR* protocol);

  int Family() const;
  int Type() const;
  int Protocol() const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

  // commonly-used predefined LmSockType objects
  static const LmSockType& Inet_Stream();
  static const LmSockType& Inet_Datagram();
  static const LmSockType& Unix_Stream();
  static const LmSockType& Unix_Datagram();

private:

  int family_;
  int type_;
  int protocol_;

};

int operator==(const LmSockType& st1, const LmSockType& st2);

#ifdef USE_INLINE
#include "LmSockType.i"
#endif

#endif /* INCLUDED_LmSockType */
