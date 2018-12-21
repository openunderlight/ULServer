// LmSockAddrInet.h  -*- C++ -*-
// $Id: LmSockAddrInet.h,v 1.12 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// internet socket address

#ifndef INCLUDED_LmSockAddrInet
#define INCLUDED_LmSockAddrInet

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#ifdef WIN32
#include "../shared/unix.h"
#include <winsock.h>
typedef int socklen_t;
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif /* WIN32 */

#include "../../libsrc/shared/LyraDefs.h"
#include "LmSockAddr.h"

// forward declarations

// the class

class LmSockAddrInet : public LmSockAddr {

public:

  enum {
    // constants
    DEFAULT_PORT = Lyra::PORT_UNKNOWN,
    DEFAULT_ADDRESS = INADDR_ANY
  };

public:

  LmSockAddrInet();
  virtual ~LmSockAddrInet();

  int  Init();
  int  Init(unsigned short port);
  int  Init(const TCHAR* hostaddr, unsigned short port);
  int  Init(unsigned long saddr, unsigned short port);

  void SetPort(unsigned short port);

  // Inet-specific methods
  int Port() const;
  unsigned long IPAddress() const;
  const TCHAR* AddressString() const;

  int Family() const;
  sockaddr* SockAddr() const;
  int SockAddrSize() const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

  LmSockAddr* Copy() const;

  static const TCHAR* AddressString(unsigned long ip_addr);
  // no longer supported due to user space threads
  //  static unsigned long GetHostByName(const TCHAR* hostname);

private:

  sockaddr_in addr_;

};

#ifdef USE_INLINE
#include "LmSockAddrInet.i"
#endif

#endif /* INCLUDED_LmSockAddrInet */
