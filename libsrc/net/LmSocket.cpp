// LmSocket.cpp  -*- C++ -*-
// $Id: LmSocket.cpp,v 1.21 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmSocket.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/ioctl.h>
//#include <stropts.h>
#include <poll.h>
#include <unistd.h>
#endif /* WIN32 */

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "LmSocket.h"
#include "LmSockAddrInet.h"

#ifndef USE_INLINE
#include "LmSocket.i"
#endif

extern int errno;

////
// constructor
////

LmSocket::LmSocket()
  : sockfd_(SOCK_INVALID),
    socktype_(LmSockType::DEFAULT_INSTANCE),
    sock_computed_(false),
    peer_computed_(false),
    has_shutdown_(false)
{
  // empty
}

////
// destructor
////

LmSocket::~LmSocket()
{
  // empty
}

////
// Init: initialize from an existing descriptor
////
 
#ifdef WIN32
void LmSocket::Init(SOCKET fd, const LmSockType& socktype)
#else
void LmSocket::Init(int fd, const LmSockType& socktype)
#endif
{
  clear_addresses();
  has_shutdown_ = false;
  sockfd_ = fd;
  socktype_ = socktype;
}

////
// Accept: accept() on socket
////

int LmSocket::Accept(LmSocket& newsock)
{
  int newfd = -1;
  LmSockAddrInet addr;
  socklen_t alen = addr.SockAddrSize();
  newfd = accept(sockfd_, addr.SockAddr(), &alen);
  newsock.Init(newfd, socktype_);
  return newfd;
}

////
// Bind: assigns name to local end of socket
////

int LmSocket::Bind(const LmSockAddrInet& serv_addr)
{
  clear_addresses();
  return bind(sockfd_, serv_addr.SockAddr(), serv_addr.SockAddrSize());
}

////
// Close: calls close() on socket
////

int LmSocket::Close()
{
  clear_addresses();
#ifdef UL_SERVER_WIN32
  return closesocket(sockfd_);
#else
  int rc;
  rc = close(sockfd_);
  sockfd_ = SOCK_INVALID;
  return (rc);
#endif
}

////
// Connect: connects to remote socket designated by given address object
////

int LmSocket::Connect(const LmSockAddrInet& remote_addr)
{
  peer_computed_ = false;
  return connect(sockfd_, remote_addr.SockAddr(), remote_addr.SockAddrSize());
}

////
// Listen: sets up listening queue
////

int LmSocket::Listen(int qlen)
{
  return listen(sockfd_, qlen);
}

////
// Shutdown
////

int LmSocket::Shutdown(int how)
{
  clear_addresses();
  has_shutdown_ = true;
  return shutdown(sockfd_, how);
}

////
// Socket
////

int LmSocket::Socket(const LmSockType& socktype)
{
  clear_addresses();
  socktype_ = socktype;
  sockfd_ = socket(socktype_.Family(), socktype_.Type(), socktype_.Protocol());
  return sockfd_;
}

////
// Recv, RecvFrom, RecvN
////

int LmSocket::Recv(void* buf, int buflen, int flags)
{
  return recv(sockfd_, (char*) buf, buflen, flags);
}

int LmSocket::RecvFrom(void* buf, int buflen, int flags)
{
  return recvfrom(sockfd_, (char*) buf, buflen, flags, 0, 0);
}

int LmSocket::RecvFrom(void* buf, int buflen, LmSockAddrInet& from, int flags)
{
  socklen_t fromlen = (socklen_t)from.SockAddrSize();
  return recvfrom(sockfd_, (char*) buf, buflen, flags, from.SockAddr(), &fromlen);
}

int LmSocket::RecvN(void* bufp, int buflen, int flags)
{
  int nleft, nread;
  char* buf = (char*) bufp;

  nleft = buflen;
  while (nleft > 0) {
    nread = recv(sockfd_, buf, nleft, flags);
    if (nread < 0) {  // error
      int errval = errno;
      return -1;
    }
    else if (nread == 0) {  // EOF
      break;
    }
    nleft -= nread;
    buf += nread;
  }
  return (buflen - nleft);
}

////
// Send, SendTo, SendN
////

int LmSocket::Send(const void* buf, int buflen, int flags)
{
  return send(sockfd_, (const char*) buf, buflen, flags);
}

int LmSocket::SendTo(const void* buf, int buflen, const LmSockAddrInet& to, int flags)
{
  return sendto(sockfd_, (const char*) buf, buflen, flags, to.SockAddr(), to.SockAddrSize());
}

int LmSocket::SendN(const void* bufp, int buflen, int flags)
{
  int nleft, nwritten;
  const char* buf = (const char*) bufp;

  nleft = buflen;
  while (nleft > 0) {
    nwritten = send(sockfd_, buf, nleft, flags);
    if (nwritten <= 0) {  // error
      return -1;
    }
    nleft -= nwritten;
    buf += nwritten;
  }
  return (buflen - nleft);
}

////
// BytesAvailable - return number of bytes available from socket, or -1 if error; note that
//   a return value of 0 does not necesssarily mean the socket has been closed (use EOF())
////

int LmSocket::BytesAvailable() const
{
  unsigned long num_bytes = 0;
  // use an ioctl
#ifdef UL_SERVER_WIN32
  int num_msgs = ioctlsocket(sockfd_, FIONREAD, &num_bytes);
#else
  int num_msgs = ioctl(sockfd_, FIONREAD, &num_bytes);
#endif
  // num_msgs doesn't seem to be all that useful, except for detecting error conditions,
  // despite what the manpage says it should return
  if (num_msgs < 0) {
    return -1;
  }
  return num_bytes;
}

////
// EndOfFile - return true if socket has reached end-of-file, false otherwise
////

bool LmSocket::EndOfFile() const
{
  // if bytes are available, haven't reached EOF
  if (BytesAvailable() > 0) {
    return false;
  }
  // check if the socket can be read from
#ifdef WIN32
  // under Windows, first we use ioctlsocket to see if there
  // is data available. If it returns 0, it means either there is no
  // data to be read, or the socket is closed (EOF). Select is then
  // used; if it says data is available, then it must be an EOF.
  DWORD dummy = 0;
  fd_set sockset;
  FD_ZERO(&sockset);
  FD_SET(sockfd_, &sockset);
  timeval timeout;
  timeout.tv_sec = timeout.tv_usec = 0;
  DWORD rc = select(dummy, &sockset, NULL, NULL, &timeout);
  if (rc > 0) // if at EOF, select will say the socket is ready to read from,
			  // since we know from ioctlsocket that 0 bytes are available
	return 1;
  else
	return 0;
#else
  struct pollfd p;
  p.fd = sockfd_;
  p.events = POLLIN;
  p.revents = 0;
  int rc = poll(&p, 1, 0);
  // data available, but 0 bytes -> EOF
  if ((rc == 1) && (p.revents & POLLIN)) {
    return true;
  }
  return false;
#endif
}

////
// GetSockOpt
////

int LmSocket::GetSockOpt(int level, int optname, void* optval, int* optlen) const
{
  return getsockopt(sockfd_, level, optname, (char*) optval, (socklen_t*)optlen);
}

int LmSocket::GetSockOpt(int optname) const
{
  int optval;
  socklen_t optlen;
  optval = -1;
  optlen = sizeof(optval);
  getsockopt(sockfd_, SOL_SOCKET, optname, (char*) &optval, &optlen);
  return optval;
}

////
// SetSockOpt
////

int LmSocket::SetSockOpt(int level, int optname, const void* optval, int optlen)
{
  return setsockopt(sockfd_, level, optname, (char*) optval, optlen);
}

int LmSocket::SetSockOpt(int optname, int optval)
{
  return setsockopt(sockfd_, SOL_SOCKET, optname, (char*) &optval, sizeof(optval));
}

////
// BindRange - uses the IP from the given address, and uses a port in the given range; 
//   serv_addr is modified to hold the port used
////

int LmSocket::BindRange(LmSockAddrInet& serv_addr, int min_port, int max_port)
{
  clear_addresses();
  for (int port = min_port; port <= max_port; ++port) {
    serv_addr.SetPort(port);
    if (bind(sockfd_, serv_addr.SockAddr(), serv_addr.SockAddrSize()) == 0) {
      return 0;
    }
  }
  return -1; // couldn't bind
}

////
// Dump
////

void LmSocket::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmSocket[%p,%d]: fd=%d type="), this, sizeof(LmSocket), Descriptor());
  SockType().Dump1(f);
 _ftprintf(f, _T(">\n"));
  indent++;
  if (!sock_computed_) {
    compute_sock_addr();
  }
  INDENT(indent, f);
 _ftprintf(f, _T("sockname(%d): "), sock_computed_);
  sock_addr_.Dump(f);
  if (!peer_computed_) {
    compute_peer_addr();
  }
  INDENT(indent, f);
 _ftprintf(f, _T("peername(%d): "), peer_computed_);
  peer_addr_.Dump(f);
}

////
// Dump1
////

void LmSocket::Dump1(FILE* f) const
{
  if (!sock_computed_) {
    compute_sock_addr();
  }
  if (!peer_computed_) {
    compute_peer_addr();
  }
 _ftprintf(f, _T("[%d"), Descriptor());
 _ftprintf(f, _T(",sock(%d)="), sock_computed_);
  sock_addr_.Dump1(f);
 _ftprintf(f, _T(",peer(%d)="), peer_computed_);
  peer_addr_.Dump1(f);
 _ftprintf(f, _T("]"));
}

////
// compute_sock_addr
////

void LmSocket::compute_sock_addr() const
{
  LmSockAddrInet addr;
  socklen_t alen = addr.SockAddrSize();
  LmSocket* self = (LmSocket*) this;
  for (int i = 0; i < 5; ++i) { // retry 5 times
    int rc = getsockname(sockfd_, addr.SockAddr(), &alen);
    if (rc == 0) {
      self->sock_addr_ = addr;
      self->sock_computed_ = true;
      return;
    }
  }
  // never succeeded, so, what do we do? set it to something obvious
  self->sock_computed_ = false;
  self->sock_addr_.Init(); // IP 0.0.0.0, port 0
}

////
// compute_peer_addr
////

void LmSocket::compute_peer_addr() const
{
  LmSockAddrInet addr;
  socklen_t alen = addr.SockAddrSize();
  LmSocket* self = (LmSocket*) this;
  for (int i = 0; i < 5; ++i) { // retry 5 times
    int rc = getpeername(sockfd_, addr.SockAddr(), &alen);
    if (rc == 0) {
      self->peer_addr_ = addr;
      self->peer_computed_ = true;
      return;
    }
  }
  // never succeeded, so, what do we do? set it to something obvious
  self->peer_computed_ = false;
  self->peer_addr_.Init(); // IP 0.0.0.0, port 0
}
