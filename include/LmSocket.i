// LmSocket.i  -*- C++ -*-
// $Id: LmSocket.i,v 1.8 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE int LmSocket::Descriptor() const
{
  return sockfd_;
}

INLINE const LmSockType& LmSocket::SockType() const
{
  return socktype_;
}

INLINE bool LmSocket::HasShutdown() const
{
  return has_shutdown_;
}

INLINE bool LmSocket::IsValid() const
{
  return (sockfd_ != SOCK_INVALID);
}

INLINE const LmSockAddrInet& LmSocket::SockName() const
{
  if (!sock_computed_) {
    compute_sock_addr();
  }
  return sock_addr_;
}

INLINE const LmSockAddrInet& LmSocket::PeerName() const
{
  if (!peer_computed_) {
    compute_peer_addr();
  }
  return peer_addr_;
}

INLINE void LmSocket::clear_addresses()
{
  sock_addr_.Init();
  peer_addr_.Init();
  sock_computed_ = peer_computed_ = false;
}

INLINE void LmSocket::ComputeAddresses(bool force)
{
  if (!sock_computed_ || force) {
    compute_sock_addr();
  }
  if (!peer_computed_ || force) {
    compute_peer_addr();
  }
}
