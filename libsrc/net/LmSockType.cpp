// LmSockType.cpp  -*- C++ -*-
// $Id: LmSockType.cpp,v 1.6 1997-10-14 21:39:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmSockType.h"
#endif

#include <stdio.h>
#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */

#include "LmSockType.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmSockType.i"
#endif

// file-only functions
static void init_predefined();

// default instance
const LmSockType LmSockType::DEFAULT_INSTANCE;

////
// Constructors
////

LmSockType::LmSockType()
{
  Init(AF_UNSPEC, -1, -1);
}

LmSockType::LmSockType(int family, int type, int protocol)
{
  Init(family, type, protocol);
}

LmSockType::LmSockType(int family, int type, const TCHAR* protocol)
{
  Init(family, type, protocol);
}

////
// Init
////

void LmSockType::Init(int family, int type, int protocol)
{
  family_ = family;
  type_ = type;
  protocol_ = protocol;
}

void LmSockType::Init(int family, int type, const TCHAR* protocol)
{  
  family_ = family;
  type_ = type;
  protocol_ = -1;
  // look up protocol
  struct protoent* pe;
#ifdef _UNICODE
  int len = _tcslen(protocol);
  char mbcs[16];
  if (wcstombs(mbcs, protocol, len) < len)
	  return;
  if (pe = getprotobyname(mbcs)) {
    protocol_ = pe->p_proto;
  }
#else
  // we use hard coded numbers to avoid a memory leak in getprotobyname
  if (!_tcscmp(protocol, "udp"))
	  protocol_ = 17; // UDP
  else
	  protocol_ = 6;  // TCP
  //if (pe = getprotobyname(protocol)) {
//    protocol_ = pe->p_proto;
	//free(pe);
  //}
#endif
}

////
// Dump
////

void LmSockType::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmSockType[%p,%d]: family=%d type=%d proto=%d>\n"), this, sizeof(LmSockType),
	  Family(), Type(), Protocol());
}

////
// Dump1
////

void LmSockType::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%d,%d,%d]"), Family(), Type(), Protocol());
}

////
// Equality operator
////

int operator==(const LmSockType& st1, const LmSockType& st2)
{
  return ((st1.Family() == st2.Family()) &&
	  (st1.Type() == st2.Type()) &&
	  (st1.Protocol() == st2.Protocol()));
}

// predfined socket type objects
static LmSockType Inet_Stream_;
static LmSockType Inet_Datagram_;
static LmSockType Unix_Stream_;
static LmSockType Unix_Datagram_;

// flag indicating they have been initialized
static bool predef_init = false;

////
// init_predefined - initialize predefined socket types (used to be during static
//   initialization, but this fails for some reason under solaris 2.6)
////

void init_predefined()
{
  if (!predef_init) {
    Inet_Stream_.Init(AF_INET, SOCK_STREAM, _T("tcp"));
    Inet_Datagram_.Init(AF_INET, SOCK_DGRAM, _T("udp"));
    Unix_Stream_.Init(AF_UNIX, SOCK_STREAM, 0);
    Unix_Datagram_.Init(AF_UNIX, SOCK_DGRAM, 0);
    predef_init = true;
  }
}

////
// Return references to predefined socket types
////

const LmSockType& LmSockType::Inet_Stream()
{
  init_predefined();
  return Inet_Stream_;
}

const LmSockType& LmSockType::Inet_Datagram()
{
  init_predefined();
  return Inet_Datagram_;
}

const LmSockType& LmSockType::Unix_Stream()
{
  init_predefined();
  return Unix_Stream_;
}

const LmSockType& LmSockType::Unix_Datagram()
{
  init_predefined();
  return Unix_Datagram_;
}
