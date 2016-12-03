// LsNetworkOutput.cpp  -*- C++ -*-
// $Id: LsNetworkOutput.cpp,v 1.26 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsNetworkOutput.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include <time.h>

#include "LsNetworkOutput.h"
#include "LyraDefs.h"
#include "LsMain.h"
#include "LsUtil.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmSrvMesgBuf.h"
#include "LmConnection.h"
#include "LmConnectionList.h"
#include "LmConnectionSet.h"
#include "LsMacros.h"
#include "SMsg_All.h"
#include "LsOutputDispatch.h"
#include "LmMesgBufPool.h"

#ifndef USE_INLINE
#include "LsNetworkOutput.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LsNetworkOutput::LsNetworkOutput(LsMain* lsmain)
  : LmNetworkOutput(lsmain->ConnectionSet(), lsmain->BufferPool(), lsmain->Log() /* &logf_ */),
    main_(lsmain)
{
  DECLARE_TheLineNum;
  open_log();
  register_message_handlers();
}

////
// Destructor
////

LsNetworkOutput::~LsNetworkOutput()
{
  DECLARE_TheLineNum;
  close_log();
}

////
// Dump
////

void LsNetworkOutput::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LsNetworkOutput[%p,%d]: main=%p>\n"), this, sizeof(LsNetworkOutput), main_);
  LmNetworkOutput::Dump(f, indent + 1);
}

////
// open_log
////

void LsNetworkOutput::open_log()
{
  // logf_.Init("ls", "out", main_->LevelDBC()->LevelID());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void LsNetworkOutput::close_log()
{
  // logf_.Close();
}

////
// register_message_handlers
////


void LsNetworkOutput::register_message_handlers()
{
  DECLARE_TheLineNum;
  // default message handler
  SetDefaultHandler((MsgHandler)&LsNetworkOutput::handle_Default);
  RegisterHandler(SMsg::LS_ACTION, (MsgHandler)&LsNetworkOutput::handle_SMsg_LS_Action);
}


////
// handle_Default
////

void LsNetworkOutput::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsNetworkOutput, handle_Default);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_LS_Action
////

void LsNetworkOutput::handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsNetworkOutput, handle_SMsg_LS_Action);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_LS_Action, false); // don't send error
  // switch on action to perform
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  switch (msg.Action()) {
  case SMsg_LS_Action::ACTION_EXIT:
    TLOG_Log(_T("%s: shutting down"), method);
    handle_SMsg_LS_Action_Exit();
    SetDone();
    break;
  case SMsg_LS_Action::ACTION_HEARTBEAT:
//    TLOG_Log(_T("%s: network output thread running"), method);
    break;
  case SMsg_LS_Action::ACTION_PING:
    // TLOG_Log(_T("%s: pinging servers"), method);
    handle_SMsg_LS_Action_Ping();
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}

////
// handle_SMsg_LS_Action_Exit
////

void LsNetworkOutput::handle_SMsg_LS_Action_Exit()
{
  DEFMETHOD(LsNetworkOutput, handle_SMsg_LS_Action_Exit);
  DECLARE_TheLineNum;
  // get connection list
  LmConnectionList conn_list;
  main_->ConnectionSet()->GetConnectionList(conn_list);
  // send logout message to all connections
  SMsg_Logout msg;
  msg.Init();
  // (since thread is exiting, can't put SendMesg messages in its queue, they'll be ignored
  LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg.MessageSize());
  mbuf->ReadMessage(msg);
  for (LmConnectionList::iterator i = conn_list.begin(); !(bool)(i == conn_list.end()); ++i) {
    LmConnection* conn = *i;
    if (!conn) {
      TLOG_Error(_T("%s: null connection in conn_list"), method);
      continue;
    }
    SendMessage(mbuf, conn);
  }
  main_->BufferPool()->ReturnBuffer(mbuf);
}

////
// handle_SMsg_LS_Action_Ping
////

void LsNetworkOutput::handle_SMsg_LS_Action_Ping()
{
  DEFMETHOD(LsNetworkOutput, handle_SMsg_LS_Action_Ping);
  DECLARE_TheLineNum;
  // get connection list
  LmConnectionList conn_list;
  main_->ConnectionSet()->GetConnectionList(conn_list);
  // send message to any game server connections
  SMsg_Ping msg;
  msg.InitPing(time(NULL));
  for (LmConnectionList::iterator i = conn_list.begin(); !(bool)(i == conn_list.end()); ++i) {
    LmConnection* conn = *i;
    if (!conn) {
      TLOG_Error(_T("%s: null connection in conn_list"), method);
      continue;
    }
    if (conn->Type() == LmConnection::CT_GSRV) {
      main_->OutputDispatch()->SendMessage(&msg, conn);
    }
  }
}
