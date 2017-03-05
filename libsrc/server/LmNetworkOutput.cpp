// LmNetworkOutput.cpp  -*- C++ -*-
// $Id: LmNetworkOutput.cpp,v 1.23 1998-02-12 15:33:05-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmNetworkOutput.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "LmNetworkOutput.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmLog.h"
#include "LmSrvMesgBuf.h"
#include "SMsg_SendMesg.h"
#include "LmMesgBufPool.h"

#ifndef USE_INLINE
#include "LmNetworkOutput.i"
#endif

////
// Constructor
////

LmNetworkOutput::LmNetworkOutput(LmConnectionSet* cset, LmMesgBufPool* bufpool, LmLog* log)
  : LmThread(bufpool, log),
    cset_(cset)
{
	//RegisterHandler(SMsg::SENDMESG, (MsgHandler)&LmNetworkOutput::handle_SMsg_SendMesg);
	RegisterHandler(SMsg::SENDMESG, (MSGHANDLER)(&LmNetworkOutput::handle_SMsg_SendMesg));
}

////
// Run: thread _tmain() function
////


void LmNetworkOutput::Run()
{
  LmUtil::BlockAllSignals();
  LmThread::Run();
}


////
// Destructor
////

LmNetworkOutput::~LmNetworkOutput()
{
  // empty
}

////
// SendMessage
////

void LmNetworkOutput::SendMessage(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LmNetworkOutput, SendMessage);
  if (!conn || !msgbuf) {
    TLOG_Warning(_T("%s: null connection or message buffer; msgbuf=[%p] conn=[%p]"), method, msgbuf, conn);
    return;
  }
  //TLOG_->Debug(_T("%s: msgbuf=[%p] conn=[%p] bufsize=%d msgtype=%d\n"), method, msgbuf, conn, msgbuf->BufferSize(), msgbuf->Header().MessageType());
  // double-check that connection is still around
  if (cset_ && !cset_->HasConnection(conn)) {
    //TLOG_->Warning(_T("%s: connection %p no longer active\n"), method, conn);
    return;
  }
  int rc = conn->SendMessage(*msgbuf);
  if (rc < 0) {
    HandleError(conn, msgbuf, ERR_SEND);
    return;
  }
  //TLOG_->Debug(_T("%s: msgbuf=[%p] conn=[%p] sent %d bytes\n"), method, msgbuf, conn, rc);
}

////
// HandleError - default implementation
////

void LmNetworkOutput::HandleError(LmConnection* conn, LmSrvMesgBuf* mbuf, int errcode)
{
  DEFMETHOD(LmNetworkOutput, HandleError);
  TLOG_Error(_T("%s: conn [%p] buf [%p]: error %d encountered; errstr(%d)=%s"), method,
	     conn, mbuf, errcode, errno, strerror(errno));
}

////
// Dump
////

void LmNetworkOutput::Dump(FILE* f, int indent ) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmNetworkOutput[%p,%d]: cset=[%p]>\n"), this, sizeof(LmNetworkOutput), cset_);
  LmThread::Dump(f, indent + 1);
}

////
// handle_SMsg_SendMesg
////

void LmNetworkOutput::handle_SMsg_SendMesg(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LmNetworkOutput, handle_SMsg_SendMesg);
  // check that conn is null
  if (conn) {
    TLOG_Error(_T("%s: internal message received from conn [%p]!"), method, conn);
    return;
  }
  // read message
  SMsg_SendMesg msg;
  if (msg.Read(*msgbuf) < 0) {
    TLOG_Error(_T("%s: could not read message, type %d"), method, msgbuf->Header().MessageType());
    return;
  }
  // get buffer, connection from message
  LmConnection* out_conn = msg.Connection();
  LmSrvMesgBuf* out_mbuf = msg.MesgBuf();
  if (!out_conn) {
    TLOG_Warning(_T("%s: null connection"), method);
  }
  if (!out_mbuf) {
    TLOG_Warning(_T("%s: null message buffer"), method);
  }
  // send message out
  if (out_conn && out_mbuf) {
    //TLOG_Debug(_T("%s: sending message [%p] out on connection [%p]\n"), method, out_mbuf, out_conn);
    SendMessage(out_mbuf, out_conn);
  }
  // done with the buffer inside the message
  if (out_mbuf) {
    BufferPool()->ReturnBuffer(out_mbuf);
  }
}
