// GsNetworkOutput.cpp  -*- C++ -*-
// $Id: GsNetworkOutput.cpp,v 1.27 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsNetworkOutput.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <time.h>


#include "GsNetworkOutput.h"
#include "GsMain.h"
#include "SMsg_GS_Action.h"
#include "GsUtil.h"
//#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmSrvMesgBuf.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "GsMacros.h"
#include "GMsg_ServerDown.h"
#include "LmConnectionList.h"
#include "SMsg_Ping.h"
#include "GsOutputDispatch.h"

#ifndef USE_INLINE
#include "GsNetworkOutput.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

GsNetworkOutput::GsNetworkOutput(GsMain* gsmain)
  : LmNetworkOutput(gsmain->ConnectionSet(), gsmain->BufferPool(), gsmain->Log() /* &logf_ */ ),
    main_(gsmain)
{
  DECLARE_TheLineNum;
  open_log();
  register_message_handlers();
}

////
// Destructor
////

GsNetworkOutput::~GsNetworkOutput()
{
  DECLARE_TheLineNum;
  close_log();
}

////
// Dump
////

void GsNetworkOutput::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<GsNetworkOutput[%p,%d]: main=[%p]>\n"), this, sizeof(GsNetworkOutput), main_);
  LmNetworkOutput::Dump(f, indent + 1);
}

////
// open_log
////

void GsNetworkOutput::open_log()
{
  // logf_.Init("gs", "out", main_->ServerPort());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void GsNetworkOutput::close_log()
{
  // logf_.Close();
}

////
// register_message_handlers
////


void GsNetworkOutput::register_message_handlers()
{
  DECLARE_TheLineNum;
  // default message handler
  SetDefaultHandler((MsgHandler)  &GsNetworkOutput::handle_Default);
  // register SMsg_GS_* handlers
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsNetworkOutput::handle_SMsg_GS_Action);
}


////
// handle_Default
////

void GsNetworkOutput::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsNetworkOutput, &(GsNetworkOutput::handle_Default));
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_GS_Action
////

void GsNetworkOutput::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsNetworkOutput, handle_SMsg_GS_Action);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Action, false); // don't send error
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  // switch on action to perform
  switch (msg.Action()) {
  case SMsg_GS_Action::ACTION_EXIT:
    TLOG_Log(_T("%s: shutting down"), method);
    SetDone();
    break;
  case SMsg_GS_Action::ACTION_HEARTBEAT:
    TLOG_Log(_T("%s: network output thread running"), method);
    break;
  case SMsg_GS_Action::ACTION_PING:
    // TLOG_Log(_T("%s: pinging servers"), method);
    handle_SMsg_GS_Action_Ping();
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}

////
// handle_SMsg_GS_Action_Ping
////

void GsNetworkOutput::handle_SMsg_GS_Action_Ping()
{
  DEFMETHOD(GsNetworkOutput, handle_SMsg_GS_Action_Ping);
  DECLARE_TheLineNum;
  // get connection list
  LmConnectionList conn_list;
  main_->ConnectionSet()->GetConnectionList(conn_list);
  // send message to any level server connections
  SMsg_Ping msg;
  msg.InitPing(time(NULL));
  for (LmConnectionList::iterator i = conn_list.begin(); !(bool)(i == conn_list.end()); ++i) {
    LmConnection* conn = *i;
    if (!conn) {
      TLOG_Error(_T("%s: null connection in conn_list"), method);
      continue;
    }
    if (conn->Type() == LmConnection::CT_LSRV) {
      main_->OutputDispatch()->SendMessage(&msg, conn);
    }
  }
}
