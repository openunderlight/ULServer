// LmSockAddrInet.i  -*- C++ -*-
// $Id: LmSockAddrInet.i,v 1.3 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE int LmSockAddrInet::Port() const
{
  return ntohs(addr_.sin_port);
}

INLINE unsigned long LmSockAddrInet::IPAddress() const
{
  return addr_.sin_addr.s_addr;
}

INLINE int LmSockAddrInet::Family() const
{
  return addr_.sin_family;
}

INLINE sockaddr* LmSockAddrInet::SockAddr() const
{
  return (sockaddr*) &addr_;
}

INLINE int LmSockAddrInet::SockAddrSize() const
{
  return sizeof(addr_);
}

INLINE void LmSockAddrInet::SetPort(unsigned short port)
{
  addr_.sin_port = htons(port);
}
