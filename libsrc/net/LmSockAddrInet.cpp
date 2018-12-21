// LmSockAddrInet.cpp  -*- C++ -*-
// $Id: LmSockAddrInet.cpp,v 1.14 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmSockAddrInet.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* WIN32 */

#include <stdio.h>
#include <string.h>

#include "LmSockAddrInet.h"
#include "../../libsrc/shared/LyraDefs.h"

#ifndef USE_INLINE
#include "LmSockAddrInet.i"
#endif

////
// Constructor
////

LmSockAddrInet::LmSockAddrInet()
{
  Init();
}

////
// Destructor
////

LmSockAddrInet::~LmSockAddrInet()
{
  // empty
}

////
// Init: initialize
////

int LmSockAddrInet::Init()
{
  // have to cast to get past overloading
  return Init((unsigned long) DEFAULT_ADDRESS, DEFAULT_PORT);
}

int LmSockAddrInet::Init(unsigned short port)
{
  return Init((unsigned long) INADDR_ANY, port);
}

int LmSockAddrInet::Init(const TCHAR* hostaddr, unsigned short port)
{
  //  Init(inet_addr(hostname), port); // deprecated
  in_addr saddr;

#ifdef UL_SERVER_WIN32
#ifdef _UNICODE // convert to MBCS for API functions
  int len = _tcslen(hostaddr);
  char mbcs[64];
  if (wcstombs(mbcs, hostaddr, len) < len)
	  return -1;
  if ((saddr.s_addr = inet_addr(mbcs)) == INADDR_NONE)
	return -1;
#else
  if ((saddr.s_addr = inet_addr(hostaddr)) == INADDR_NONE)
	return -1;
#endif // _UNICODE
#else
#ifdef _UNICODE
  int len = _tcslen(hostaddr);
  char mbcs[64];
  if (wcstombs(mbcs, hostaddr, len) < len)
	  return -1;
  if (inet_aton(mbcs, &saddr) == 0) 
    return -1;
#else
  if (inet_aton(hostaddr, &saddr) == 0) 
    return -1;
#endif // _UNICODE
#endif // UL_SERVER_WIN32

  return this->Init(saddr.s_addr, port);

}

int LmSockAddrInet::Init(unsigned long saddr, unsigned short port)
{
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = saddr;
  addr_.sin_port = htons(port);
  return 0;
}

////
// AddressString
////

const TCHAR* LmSockAddrInet::AddressString() const
{
  return AddressString(IPAddress());
}

const TCHAR* LmSockAddrInet::AddressString(unsigned long ip_addr)
{
  struct in_addr addr;
  memset(&addr, 0, sizeof(addr));
  addr.s_addr = ip_addr;
  // inet_ntoa is MT-safe, so it's OK to do this
#ifdef _UNICODE
  char sbcs[64];
  static TCHAR wbcs[64];
  strcpy(sbcs, inet_ntoa(addr));
  int len = strlen(sbcs);
  if (mbstowcs(wbcs, sbcs, len) < len)
	  return NULL;
  return (TCHAR*)wbcs;
#else
  return inet_ntoa(addr);
#endif // _UNICODE
}

////
// Copy
////

LmSockAddr* LmSockAddrInet::Copy() const
{
  return LmNEW(LmSockAddrInet(*this));
}

////
// Dump
////

void LmSockAddrInet::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmSockAddrInet[%p,%d]: family=%d ip=%s (%lu) port=%d>\n"), this, sizeof(LmSockAddrInet),
	  Family(), AddressString(), IPAddress(), Port());
}

////
// Dump1
////

void LmSockAddrInet::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%s:%d]"), inet_ntoa(addr_.sin_addr), Port());
}

/*
////
// GetHostByName
////

unsigned long LmSockAddrInet::GetHostByName(const TCHAR* hostname)
{
  TCHAR buf[1024];
  struct hostent* he;
  int h_err = 0;
  unsigned long saddr = 0;
  //  if (gethostbyname_r(hostname, &he, buf, sizeof(buf), &h_err)) {
    // found host, copy first address into address struct
  //memcpy((TCHAR*) &saddr, he->h_addr, MIN((unsigned) he->h_length, sizeof(saddr)));
  //}

  if (he = gethostbyname(hostname)) {
    // &he, buf, sizeof(buf), &h_err)) {
      // found host, copy first address into address struct
  memcpy((TCHAR*) &saddr, he->h_addr_list, MIN((unsigned) he->h_length, sizeof(saddr)));
  }


  else {
    // try simpler dotted quad conversion
    saddr = inet_addr(hostname);
  }
  return saddr;
}

*/
