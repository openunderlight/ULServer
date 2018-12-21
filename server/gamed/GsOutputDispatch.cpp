// GsOutputDispatch.cpp  -*- C++ -*-
// $Id: GsOutputDispatch.cpp,v 1.12 1997/08/26 00:11:02 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsOutputDispatch.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "GsOutputDispatch.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "GsMain.h"
#include "LmThreadPool.h"
#include "GMsg_All.h"
#include "LmLevelDBC.h"
#include "LmRoomDB.h"
#include "SMsg_SendMesg.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"

#ifndef USE_INLINE
#include "GsOutputDispatch.i"
#endif

////
// Constructor
////

GsOutputDispatch::GsOutputDispatch(GsMain* gsmain)
  : LmDispatch(gsmain->BufferPool()),
    main_(gsmain)
{
  // empty
}

////
// Destructor
////

GsOutputDispatch::~GsOutputDispatch()
{
  // empty
}

////
// SendMessage
////

int GsOutputDispatch::SendMessage(LmMesg* msg, LmConnection* conn)
{
  // put message in a buffer
  LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg->MessageSize());
  mbuf->ReadMessage(*msg);
  // send it
  return SendMessage(mbuf, conn);
}

int GsOutputDispatch::SendMessage(LmSrvMesgBuf* mbuf, LmConnection* conn)
{
  // create "send message" message
  SMsg_SendMesg out_msg;
  out_msg.Init(conn, mbuf);
  // dispatch it
  if (DispatchMessage(&out_msg, 0) < 0) {
    main_->BufferPool()->ReturnBuffer(mbuf);
    return -1;
  }
  return 0;
}

////
// ComputeTarget
////

LmThread* GsOutputDispatch::ComputeTarget(LmSrvMesgBuf* /* mbuf */, LmConnection* /* conn*/)
{
  // output goes to network output thread, always
  return main_->ThreadPool()->GetThread(GsMain::THREAD_NETOUTPUT);
}

////
// Dump
////

void GsOutputDispatch::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GsOutputDispatch[%p,%d]: main=[%p]>\n"), this, sizeof(GsOutputDispatch), main_);
  LmDispatch::Dump(f, indent + 1);
}
