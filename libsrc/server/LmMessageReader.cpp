// LmMessageReader.cpp  -*- C++ -*-
// $Id: LmMessageReader.cpp,v 1.29 1998-02-12 15:33:05-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmMessageReader.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "LmMessageReader.h"
#include "LmDispatch.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmConnection.h"
#include "LmMesgHdr.h"
#include "LmLog.h"
#include "SMsg_RecvMesg.h"
#include "LyraMessage.h"
#include "LmLocker.h"

#ifndef USE_INLINE
#include "LmMessageReader.i"
#endif

////
// Constructor
////

LmMessageReader::LmMessageReader(LmDispatch* dispatch, LmMesgBufPool* mpool, LmLog* log)
  : LmThread(mpool, log),
    dispatch_(dispatch),
    uses_(0),
    msgs_read_(0),
    bytes_read_(0),
    conn_(0)
{
  conn_lock_.Init();
  // register message handlers
  //RegisterHandler(SMsg::RECVMESG, (MsgHandler) &LmMessageReader::handle_SMsg_RecvMesg);
  RegisterHandler(SMsg::RECVMESG, (MSGHANDLER)&LmMessageReader::handle_SMsg_RecvMesg);
}

////
// Destructor
////

LmMessageReader::~LmMessageReader()
{
  // empty
}

////
// Run
////

void LmMessageReader::Run()
{
  // modified LmThread Run()
  SetDone(false);
  while (!Done()) {
    HandleNextMessage();
    ThreadYield();
  }
  DoneRunning();
}

////
// HandleError: default implementation; return true if error was fatal for the connection, ie.
//   no more messages are going to be read from it; return false if error condition was
//   non-fatal, ie. more messages may be read
///

bool LmMessageReader::HandleError(LmConnection* conn, int errcode, int mtype, int msize)
{
  DEFMETHOD(LmMessageReader, HandleError);
  //TLOG_Error(_T("%s: conn=[%p] error=%c mtype=%d msize=%d strerror(%d): %s"), method,  conn, errcode, mtype, msize, errno, strerror(errno));
  return false; // safe return value
}

////
// Dump
////

void LmMessageReader::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmMessageReader[%p,%d]: dispatch=[%p] conn=[%p] uses=%d mread=%d/%d>\n"),
	  this, sizeof(LmMessageReader),
	  dispatch_, conn_, uses_, msgs_read_, bytes_read_);
  // base class
  LmThread::Dump(f, indent + 1);
}

////
// SetCurrentConnection
////

void LmMessageReader::SetCurrentConnection(LmConnection* conn)
{
  LmLocker mon(conn_lock_); // lock during method duration
  conn_ = conn;
}

////
// CurrentConnection
////

LmConnection* LmMessageReader::CurrentConnection() const
{
  LmLocker mon(conn_lock_); // lock during method duration
  return conn_;
}

////
// handle_SMsg_RecvMesg
////

void LmMessageReader::handle_SMsg_RecvMesg(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LmMessageReader, handle_SMsg_RecvMesg);

  //TLOG_Debug(_T("%s: received new internal message from conn [%p]!"), method, conn);

  // check that conn is null
  if (conn) {
    TLOG_Error(_T("%s: internal message received from non-NULL conn [%p]!"), method, conn);
    SetCurrentConnection(0);
    return;
  }
  // read message
  SMsg_RecvMesg msg;
  if (msg.Read(*msgbuf) < 0) {
    //TLOG_Error(_T("%s: could not read message, type %d"), method, msgbuf->Header().MessageType());
    SetCurrentConnection(0);
    return;
  }
  uses_++;
  //TLOG_Debug(_T("%s: num uses = %u"), method, uses_);
  // read message from enclosed connection
  LmConnection* sconn = msg.Connection();
  if (!sconn) {
    //TLOG_Error(_T("%s: null connection to read from?"), method);
    return;
  }

  if (!sconn->IsConnected()) {
    TLOG_Error(_T("%s: unconnected connection %u to read from?"), method, sconn);
    // reset thread's current connection
    sconn->EndMessageReading();
    SetCurrentConnection(0);
	return;
  }

  // this shouldn't be necessary, but it can't hurt
  sconn->StartMessageReading();
  //TLOG_Debug(_T("%s: receiving from conn [%p]\n"), method, sconn);
  // read all messages available from connection; this used to read only a single message, but
  // servers tend to send bunches of them, so we read until there's nothing left to get
  int num_bytes = 0;
  int num_read = 0;
  bool fatal_error = false; // error handler returns this value
  do {
    // see how many bytes are available
    num_bytes = sconn->Socket().BytesAvailable();
    //TLOG_Debug(_T("%s: bytes available=%d errno(%d)='%s'\n"), method, num_bytes, errno, strerror(errno));
    // if 0 bytes available, attempt to determine if EOF has been reached
    if (num_bytes == 0) {
      if (sconn->Socket().EndOfFile()) {
	//TLOG_Debug(_T("%s: detected EOF on connection [%p]\n"), method, sconn);
	HandleError(sconn, ERR_EOF, 0, 0);
	break;
      }
      else {
	// not EOF, but nothing to read; don't continue reading, since it will lock the socket
	fatal_error = HandleError(sconn, ERR_NODATA, 0, 0);
	//TLOG_Debug(_T("%s: fatal error %u handled"), method, fatal_error);
	break; // bust out of loop
      }
    }
	else if (num_bytes < 0) {
      TLOG_Error(_T("%s: error determining # of bytes reading SMsg; errno=%d, err=%s\n"), method, errno, strerror(errno));
	  fatal_error = true;
	  break;
	}
    //TLOG_Debug(_T("%s: num bytes received = %d"), method, num_bytes);

    // peek at header, so we can figure out the message size
    LmMesgHdr mh;
    int rc = sconn->PeekAtHeader(mh);
    if (rc < 0) {
      TLOG_Error(_T("%s: error peeking at header; rc=%d; num bytes=%d; errno=%d\n"), method, rc, num_bytes, errno);
      fatal_error = HandleError(sconn, ERR_HEADERPEEK, 0, 0);
      break;
    }
    //TLOG_Debug(_T("%s: incoming message: type(%d)=%s size=%d\n"), method, mh.MessageType(), LyraMessage::MessageName(mh.MessageType()), mh.MessageSize());
    if (mh.MessageSize() > Lyra::MSG_MAX_SIZE) {
      //TLOG_Error(_T("%s: message size overflow"), method);
      fatal_error = HandleError(sconn, ERR_MSGSIZE, mh.MessageType(), mh.MessageSize());
      break;
    }
    // allocate message buffer
    LmSrvMesgBuf* mbuf = BufferPool()->AllocateBuffer(mh.MessageSize());
    // read message
    rc = sconn->ReceiveMessage(*mbuf);
    if (rc < 0) {
      TLOG_Error(_T("%s: error receiving message; rc=%d\n"), method, rc);
      BufferPool()->ReturnBuffer(mbuf);
      fatal_error = HandleError(sconn, ERR_RECEIVE, mh.MessageType(), mh.MessageSize());
      break;
    }
    //TLOG_Debug(_T("%s: received %d bytes, Dispatching to conn %ld\n"), method, rc, sconn->Socket().Descriptor());
    // dispatch it
    if (dispatch_->DispatchMessage(mbuf, sconn) < 0) {
      //TLOG_Error(_T("%s: error dispatching message"), method);
      BufferPool()->ReturnBuffer(mbuf);
      fatal_error = HandleError(sconn, ERR_DISPATCH, mh.MessageType(), mh.MessageSize());
      break;
    }
    else {
      num_read++;
      msgs_read_++;
      bytes_read_ += mbuf->BufferSize();
      //TLOG_Debug(_T("%s: mbuf [%p], size %d, type %d, conn [%p], dispatched successfully\n"), method, mbuf, mbuf->BufferSize(), mbuf->Header().MessageType(), sconn);
    }
    // get number of bytes remaining to be read
    num_bytes = sconn->Socket().BytesAvailable();
    if (num_bytes > 0) {
      //TLOG_Debug(_T("%s: bytes remaining=%d errno(%d)='%s'\n"), method, num_bytes, errno, strerror(errno));
    }
  } while (num_bytes > 0); // until nothing ready to read
  // reset connection's message reader flag, if it's possible for more messages to be read
  if (!fatal_error) {
    sconn->EndMessageReading();
  }
  // reset thread's current connection
  SetCurrentConnection(0);
  // log
  if (num_read > 0) {
    //TLOG_Debug(_T("%s: read %d messages\n"), method, num_read);
  }
}
