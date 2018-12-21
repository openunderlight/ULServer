// SMsg_Proxy.cpp  -*- C++ -*-
// $Id: SMsg_Proxy.cpp,v 1.9 1997-08-04 18:39:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// message implementation

#ifdef __GNUC__
#pragma implementation "SMsg_Proxy.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else /* !WIN32 */
#include <sys/types.h>
#include <netinet/in.h>
#endif /* WIN32 */
#include <stdio.h>
#include <string.h>

#include "SMsg_Proxy.h"
#include "LyraDefs.h"
#include "SMsg.h"
#include "LmMesgBuf.h"

#ifndef USE_INLINE
#include "SMsg_Proxy.i"
#endif

////
// constructor
////

SMsg_Proxy::SMsg_Proxy()
  : LmMesg(SMsg::PROXY, sizeof(data_t), sizeof(data_t), &data_)
{
  // initialize default message data values
  SetPlayerID(Lyra::ID_UNKNOWN);
  SetProxyType(PROXY_UNKNOWN);
  data_.mtype = 0;
  data_.msize = 0;
  calc_size();
}

////
// destructor
////

SMsg_Proxy::~SMsg_Proxy()
{
  // empty
}

////
// Init
////

void SMsg_Proxy::Init(lyra_id_t playerid, int proxy_type, LmMesg& msg)
{
  SetPlayerID(playerid);
  SetProxyType(proxy_type);
  SetEnclosedMessage(msg);
}

void SMsg_Proxy::Init(lyra_id_t playerid, int proxy_type, LmMesgBuf* msgbuf)
{
  SetPlayerID(playerid);
  SetProxyType(proxy_type);
  SetEnclosedMessage(msgbuf);
}

////
// SetEnclosedMessage
////

void SMsg_Proxy::SetEnclosedMessage(LmMesg& msg)
{
  // init header info
  data_.mtype = msg.MessageType();
  data_.msize = msg.MessageSize();
  // copy message data, in NETWORK order
  msg.ConvertToNetwork();
  memcpy(data_.msgbytes, msg.MessageAddress(), data_.msize);
  msg.ConvertToHost(); // leave in host order upon return
  // variable-length message
  calc_size();
}

void SMsg_Proxy::SetEnclosedMessage(LmMesgBuf* msgbuf)
{
  // init header info
  data_.mtype = msgbuf->Header().MessageType();
  data_.msize = msgbuf->Header().MessageSize();
  // copy message data, in NETWORK order
  memcpy(data_.msgbytes, msgbuf->MessageAddress(), data_.msize);
  // variable-length message
  calc_size();
}

////
// CopyToBuffer - copy enclosed message into given message buffer
////

void SMsg_Proxy::CopyToBuffer(LmMesgBuf& msgbuf)
{
  // create header
  LmMesgHdr mhdr;
  mhdr.Init(EnclosedMessageType(), EnclosedMessageSize());
  // init buffer
  msgbuf.ReadHeader(mhdr);
  // copy message bytes into buffer
  memcpy(msgbuf.MessageAddress(), MessageBytes(), EnclosedMessageSize());
}

////
// hton
////

void SMsg_Proxy::hton()
{
  HTONL(data_.playerid);
  HTONL(data_.mtype);
  HTONL(data_.msize);
  // no conversion: msgbytes
}

////
// ntoh
////

void SMsg_Proxy::ntoh()
{
  NTOHL(data_.playerid);
  NTOHL(data_.mtype);
  NTOHL(data_.msize);
  // no conversion: msgbytes
  calc_size(); // variable-size message
}

////
// Dump: print to FILE stream
////

void SMsg_Proxy::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<SMsg_Proxy[%p,%d]: "), this, sizeof(SMsg_Proxy));
  if (ByteOrder() == ByteOrder::HOST) {
   _ftprintf(f, _T("playerid=%u ptype='%c' mtype=%d msize=%d>\n"),
	    PlayerID(), ProxyType(), EnclosedMessageType(), EnclosedMessageSize());
  }
  else {
   _ftprintf(f, _T("(network order)>\n"));
  }
  // print out base class
  LmMesg::Dump(f, indent + 1);
}

////
// calc_size
////

void SMsg_Proxy::calc_size()
{
  // initial size: whole structure, minus variable-sized field (message bytes)
  int size = sizeof(data_t) - sizeof(data_.msgbytes);
  // add message bytes length
  size += data_.msize;
  SetMessageSize(size);
}
