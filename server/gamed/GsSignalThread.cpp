// GsSignalThread.cpp  -*- C++ -*-
// $Id: GsSignalThread.cpp,v 1.35 1998/03/11 23:30:53 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsSignalThread.h"
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <sys/wait.h>
#endif
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "GsSignalThread.h"
#include "LyraDefs.h"
#include "LmLog.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmSrvMesgBuf.h"
#include "LmMesg.h"
#include "LmThreadPool.h"
#include "LmMesgBufPool.h"
#include "GsMain.h"
#include "SMsg_GS_Action.h"
#include "GsUtil.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "GsMacros.h"
#include "GsPlayerSet.h"
#include "LmRand.h"

#ifndef USE_INLINE
#include "GsSignalThread.i"
#endif

DECLARE_TheFileName;

unsigned int last_signal_ = 0;

////
// Constructor
////

GsSignalThread::GsSignalThread(GsMain* gsmain)
  : LmSignalThread(gsmain->BufferPool(), gsmain->Log() /* &logf_ */ ),
    main_(gsmain)
{
 
  DECLARE_TheLineNum;
  register_message_handlers();
  register_signal_handlers();
}

////
// Destructor
////

GsSignalThread::~GsSignalThread()
{
  DECLARE_TheLineNum;
}

////
// Dump
////

void GsSignalThread::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<GsSignalThread[%p,%d]: main=[%p]>\n"), this, sizeof(GsSignalThread),
	  main_);
  LmSignalThread::Dump(f, indent + 1);
}

////
// Run
////

void GsSignalThread::Run()
{
  DECLARE_TheLineNum;
  LmSignalThread::Run();
}

////
// register_message_handlers
////


void GsSignalThread::register_message_handlers()
{
  DECLARE_TheLineNum;
  SetDefaultHandler((MsgHandler) &GsSignalThread::handle_Default);
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsSignalThread::handle_SMsg_GS_Action);
}


////
// handle_Default: default message handler
////

void GsSignalThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsSignalThread, (GsSignalThread::handle_Default));
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // print error message to log
  //TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_GS_Action
////

void GsSignalThread::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsSignalThread, handle_SMsg_GS_Action);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Action, false); // don't send error
  // process
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  switch (msg.Action()) {
  case SMsg_GS_Action::ACTION_EXIT:
    //    TLOG_Log(_T("%s: exiting"), method);
    SetDone();
    break;
  default:
    //TLOG_Error(_T("%s: illegal action code %c"), method, msg.Action());
    break;
  }
}


////
// register_signal_handlers
////

void GsSignalThread::register_signal_handlers()
{
  DECLARE_TheLineNum;
  SetDefaultSignalHandler((SignalHandler)&GsSignalThread::handle_SIG_Default);
  RegisterSignalHandler(SIGTERM, (SignalHandler)&GsSignalThread::handle_SIGTERM);
}



////
// handle_SIG_Default
////

void GsSignalThread::handle_SIG_Default(int sig)
{
  return;
  DEFMETHOD(GsSignalThread, &(GsSignalThread::handle_SIG_Default));
  DECLARE_TheLineNum;
}


////
// handle_SIGTERM: shut down, and exit
////

void GsSignalThread::handle_SIGTERM(int sig)
{
  DEFMETHOD(GsSignalThread, handle_SIGTERM);
  DECLARE_TheLineNum;

  last_signal_ = sig;

  if (sig != SIGTERM) {
    return;
  }
  main_->SetSIGTERM(true);
}
