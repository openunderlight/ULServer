// LmConnection.cpp  -*- C++ -*-
// $Id: LmConnection.cpp,v 1.19 1997-10-22 19:21:21-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmConnection.h"
#endif

#include <stdio.h>
#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */

#include <time.h>
#include "LmConnection.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgHdr.h"
#include "LmSocket.h"
#include "LmUtil.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LmConnection.i"
#endif

////
// Constructor
////

LmConnection::LmConnection()
  : id_(Lyra::ID_UNKNOWN),
    type_(CT_UNKNOWN),
    time_connect_(0),
    time_in_(0),
    time_out_(0),
    msg_min_(0),
    msg_max_(0),
    msgs_in_(0),
    msgs_out_(0),
    bytes_in_(0),
    bytes_out_(0),
    is_connected_(false),
	is_closed_(false),
	challenge_time_(0),
    msg_read_(false)
{
  mr_lock_.Init();
}

////
// Destructor
////

LmConnection::~LmConnection()
{
  Disconnect();
}

////
// Init
////

void LmConnection::Init(LmSocket& sock, lyra_id_t id, int contype)
{
  is_connected_ = true;
  is_closed_ = false;
  sock_.Init(sock.Descriptor(), sock.SockType());
  sock_.ComputeAddresses(true);
  id_ = id;
  type_ = contype;
  init_stats();
  // init message range to largest possible value
  SetMessageRange(Lyra::MSG_MIN, Lyra::MSG_MAX);
}

////
// Disconnect
////

void LmConnection::Disconnect()
{
  if (is_connected_) {
    // close down socket
    is_connected_ = false;
    sock_.Close();
    init_stats(); 
  }
}


#if 0

////
// Disable
////

// to prevent server crashes, we disable sockets by shutting down
// the connection and setting the ID to zero
void LmConnection::Disable()
{
	if (is_connected_)
	{
		//sock_.Shutdown();
	}
	id_ = LmConnection::DEAD_CONNECTION;
}


////
// Close Socket
////

// diagnostic method to find source of crashes
void LmConnection::CloseSocket()
{
	if (is_connected_ && !is_closed_)
	{
		is_closed_ = true;
		msg_read_ = false;
		sock_.Close();
	}
}

#endif
////
// SendMessage: send message over socket, return bytes sent (>0) if successful or -1 if error/EOF.
////

int LmConnection::SendMessage(LmSrvMesgBuf& msgbuf)
{
  if (!is_connected_) {
    return -1;
  }
  // send message over socket until entire message is sent
  int bytes = sock_.SendN(msgbuf.BufferAddress(), msgbuf.BufferSize());
  // send entire message?
  if (bytes != msgbuf.BufferSize()) {
    return -2;
  }
  // update stats
  ++msgs_out_;
  bytes_out_ += bytes;
  time_out_ = time(NULL);
  // return ok
  return bytes;
}

////
// ReceiveMessage: read message from socket into buffer; return bytes read (>0) if successful, -1
//   if error/EOF.
////

int LmConnection::ReceiveMessage(LmSrvMesgBuf& msgbuf)
{
  return ReceiveMessage(msgbuf, msg_min_, msg_max_);
}

int LmConnection::ReceiveMessage(LmSrvMesgBuf& msgbuf, int min_mtype, int max_mtype)
{
  if (!is_connected_) {
    return -1;
  }
  // receive message header
  LmMesgHdr mh;
  int hbytes = sock_.RecvN(mh.HeaderAddress(), mh.HeaderSize());
  // EOF?
  if (hbytes == 0) {
    return -2;
  }
  // get whole header?
  if (hbytes != mh.HeaderSize()) {
    return -3;
  }
  mh.SetByteOrder(ByteOrder::NETWORK);
  msgbuf.ReadHeader(mh);
  // check message type for inclusion in given range
  if ((mh.MessageType() <= min_mtype) || (mh.MessageType() >= max_mtype)) {
    return -4;
  }
  // check message length
  if (mh.MessageSize() > Lyra::MSG_MAX_SIZE) {
    return -5;
  }
  // receive message body
  int mbytes = sock_.RecvN(msgbuf.MessageAddress(), msgbuf.MessageSize());
  // get whole message?
  if (mbytes != msgbuf.MessageSize()) {
    return -6;
  }
  // update stats
  ++msgs_in_;
  bytes_in_ += hbytes + mbytes;
  time_in_ = time(NULL);
  // return ok
  return (hbytes + mbytes);
}

////
// PeekAtHeader: return 0 if successful, < 0 otherwise
////

int LmConnection::PeekAtHeader(LmMesgHdr& msghdr)
{
  if (!is_connected_) {
    return -1;
  }
  int hbytes = sock_.RecvN(msghdr.HeaderAddress(), msghdr.HeaderSize(), MSG_PEEK);
  // EOF?
  if (hbytes == 0) {
    return -2;
  }
  // get whole header?
  if (hbytes != msghdr.HeaderSize()) {
    return -3;
  }
  msghdr.SetByteOrder(ByteOrder::NETWORK);
  return 0;
}

////
// StartMessageReading
////

void LmConnection::StartMessageReading()
{
  LmLocker mon(mr_lock_); // lock variable
  msg_read_ = true;
}

////
// EndMessageReading
////

void LmConnection::EndMessageReading()
{
  LmLocker mon(mr_lock_); // lock variable
  msg_read_ = false;
}

////
// MessageBeingRead?
////

bool LmConnection::MessageBeingRead() const
{
  LmLocker mon(mr_lock_); // lock variable
  return msg_read_;
}

////
// Dump
////

void LmConnection::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmConnection[%p,%d]: id=%u type=%c mrange=[%d,%d] isconn=%d msg=%d>\n"),
	  this, sizeof(LmConnection),
	  ID(), Type(), MinMessageType(), MaxMessageType(), IsConnected(), MessageBeingRead());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("in(%ld)=%dm/%db out(%ld)=%dm/%db connect=%ld online=%ld idle=%ld/%ld\n"),
	  LastIn(), MessagesIn(), BytesIn(), LastOut(), MessagesOut(), BytesOut(),
	  ConnectTime(), Online(), IdleIn(), IdleOut());
  sock_.Dump(f, indent + 1);
}

////
// Dump1
////

void LmConnection::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[%u,%c,sock="), ID(), Type());
  sock_.Dump1(f);
 _ftprintf(f, _T("]"));
}

////
// init_stats
////

void LmConnection::init_stats()
{
  time_connect_ = time_in_ = time_out_ = time(NULL);
  msgs_in_ = msgs_out_ = 0;
  bytes_in_ = bytes_out_ = 0;
  id_ = Lyra::ID_UNKNOWN;
  msg_read_ = false;
}
