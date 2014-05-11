// LmSocket.h  -*- C++ -*-
// $Id: LmSocket.h,v 1.16 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// socket wrapper class - only works with IP sockets (TCP/UDP)

#ifndef INCLUDED_LmSocket
#define INCLUDED_LmSocket

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmSockType.h"
#include "LmSockAddrInet.h"

// forward declarations

// the socket class

class LmSocket {

public:

  enum {
    // constants
    SOCK_INVALID = -1,

    SHUTDOWN_READ = 0,
    SHUTDOWN_WRITE = 1,
    SHUTDOWN_READWRITE = 2,

    // default values
    DEFAULT_SOCK = SOCK_INVALID,
    DEFAULT_QLEN = 5
  };

public:

  LmSocket();
  ~LmSocket();

#ifdef WIN32
  void Init(SOCKET fd, const LmSockType& socktype);
#else
  void Init(int fd, const LmSockType& socktype);
#endif

  // from the socket API
  int Accept(LmSocket& newsock);
  int Bind(const LmSockAddrInet& serv_addr);
  int Close();
  int Connect(const LmSockAddrInet& remote_addr);
  int Listen(int qlen = DEFAULT_QLEN);
  int Shutdown(int how = SHUTDOWN_READWRITE);
  int Socket(const LmSockType& socktype);

  int BindRange(LmSockAddrInet& serv_addr, int min_port, int max_port);

  // data transfer methods
  int Recv(void* buf, int buflen, int flags = 0);
  int RecvFrom(void* buf, int buflen, int flags = 0);
  int RecvFrom(void* buf, int buflen, LmSockAddrInet& fromaddr, int flags = 0);
  int Send(const void* buf, int buflen, int flags = 0);
  int SendTo(const void* buf, int buflen, const LmSockAddrInet& toaddr, int flags = 0);

  // "guaranteed" transfers (work only on stream sockets)
  int RecvN(void* buf, int buflen, int flags = 0);
  int SendN(const void* buf, int buflen, int flags = 0);

  int BytesAvailable() const;
  bool EndOfFile() const;

  // get/set options
  int GetSockOpt(int level, int optname, void* optval, int* optlen) const;
  int GetSockOpt(int optname) const;
  int SetSockOpt(int level, int optname, const void* optval, int optlen);
  int SetSockOpt(int optname, int optval);

  // get local/remote addresses
  void ComputeAddresses(bool force = false);
  const LmSockAddrInet& PeerName() const;
  const LmSockAddrInet& SockName() const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

  int Descriptor() const;
  const LmSockType& SockType() const;
  bool IsValid() const;
  bool HasShutdown() const;

private:

  // not implemented
  LmSocket(const LmSocket&);
  //operator=(const LmSocket&);

  void clear_addresses();
  void compute_sock_addr() const;
  void compute_peer_addr() const;
  int move_fd(int fd, int min_fd = 256);

#ifdef WIN32
  SOCKET sockfd_;                // socket descriptor
#else
  int sockfd_;                // socket descriptor
#endif
  LmSockType socktype_;       // socket type
  LmSockAddrInet sock_addr_;  // local address
  LmSockAddrInet peer_addr_;  // peer address
  bool sock_computed_;
  bool peer_computed_;
  bool has_shutdown_;             // shutdown called

};

#ifdef USE_INLINE
#include "LmSocket.i"
#endif

#endif /* INCLUDED_LmSocket */
