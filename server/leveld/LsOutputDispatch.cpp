// LsOutputDispatch.cpp  -*- C++ -*-
// $Id: LsOutputDispatch.cpp,v 1.12 1997-08-17 16:49:23-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsOutputDispatch.h"
#endif

#include <stdarg.h>
#include <stdio.h>

#include "LsOutputDispatch.h"
#include "LyraDefs.h"
#include "LsMain.h"
#include "LmThreadPool.h"
#include "RMsg_All.h"
#include "LmLevelDBC.h"
#include "LmRoomDB.h"
#include "SMsg_SendMesg.h"
#include "LmMesgBufPool.h"
#include "LmSrvMesgBuf.h"

#ifndef USE_INLINE
#include "LsOutputDispatch.i"
#endif

////
// Constructor
////

LsOutputDispatch::LsOutputDispatch(LsMain* lsmain)
  : LmDispatch(lsmain->BufferPool()),
    main_(lsmain)
{
  // empty
}

////
// Destructor
////

LsOutputDispatch::~LsOutputDispatch()
{
  // empty
}

////
// SendMessage
////

void LsOutputDispatch::SendMessage(LmMesg* msg, LmConnection* conn)
{
  // put message in a buffer
  LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg->MessageSize());
  mbuf->ReadMessage(*msg);
  // send it
  SendMessage(mbuf, conn);
}

void LsOutputDispatch::SendMessage(LmSrvMesgBuf* mbuf, LmConnection* conn)
{
  // create "send message" message
  SMsg_SendMesg out_msg;
  out_msg.Init(conn, mbuf);
  // dispatch it
  if (DispatchMessage(&out_msg, 0) < 0) {
    main_->BufferPool()->ReturnBuffer(mbuf);
  }
}

////
// ComputeTarget
////

LmThread* LsOutputDispatch::ComputeTarget(LmSrvMesgBuf* /* mbuf */, LmConnection* /* conn*/)
{
  // output goes to network output thread, always
  return main_->ThreadPool()->GetThread(LsMain::THREAD_NETOUTPUT);
}

////
// Dump
////

void LsOutputDispatch::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsOutputDispatch[%p,%d]: main=%p>\n"), this, sizeof(LsOutputDispatch), main_);
  LmDispatch::Dump(f, indent + 1);
}
