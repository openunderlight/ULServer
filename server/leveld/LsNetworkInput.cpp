// LsNetworkInput.cpp  -*- C++ -*-
// $Id: LsNetworkInput.cpp,v 1.45 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsNetworkInput.h"
#endif

#include <errno.h>
#include <string.h>
#include <cstdlib>

#include "LsNetworkInput.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LsMessageReader.h"
#include "LsMain.h"
#include "LsInputDispatch.h"
#include "LmLog.h"
#include "LsUtil.h"
#include "LmSrvMesgBuf.h"
#include "SMsg_All.h"
#include "LmLocker.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmConnectionList.h"
#include "LmMesgBufPool.h"
#include "RMsg_All.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "LmLogFile.h"
#include "LsMacros.h"
#include "LsPlayerList.h"
#include "LmServerDBC.h"

#ifndef USE_INLINE
#include "LsNetworkInput.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LsNetworkInput::LsNetworkInput(LsMain* lsmain)
  : LmNetworkInput(lsmain->InputDispatch(), lsmain->ConnectionSet(), lsmain->BufferPool(), lsmain->Log() /* &logf_ */),
    main_(lsmain)
{
  DECLARE_TheLineNum;
  open_log();
  register_message_handlers();
  AddListener(main_->SocketTCP());
  //  StartIdleReaders(4);
}

////
// Destructor
////

LsNetworkInput::~LsNetworkInput()
{
  DECLARE_TheLineNum;
  close_log();
}

////
// StartIdleReaders
////

void LsNetworkInput::StartIdleReaders(int num_readers)
{
  DECLARE_TheLineNum;
  for (int i = 0; i < num_readers; ++i) {
    AddReader(StartReader());
  }
}

////
// Run
////

void LsNetworkInput::Run()
{
  DECLARE_TheLineNum;
  LmUtil::BlockAllSignals();
  LmNetworkInput::Run();
}

////
// AllowConnection - return true if connection is allowed; level servers can only be
//   connected to from other servers, so check that socket's peer IP is a server IP
////

bool LsNetworkInput::AllowConnection(LmSocket* sock)
{
  return main_->ServerDBC()->IsServerIP(sock->PeerName().IPAddress());
}

////
// StartReader
////

LmMessageReader* LsNetworkInput::StartReader()
{
  DEFMETHOD(LsNetworkInput, StartReader);
  DECLARE_TheLineNum;
  LsMessageReader* thr = LmNEW(LsMessageReader(main_, Log()));
  // stack size on these can be smaller (64K should be more than plenty)
  PThAttr attr;
  attr.Init();
  attr.SetJoinable(FALSE);
//  attr.SetStackSize(262144);
  attr.SetStackSize(131072);
  // create thread
  if (thr->Create(&attr) < 0) {
    TLOG_Error(_T("%s: could not create reader thread: %s"), method, strerror(errno));
    LmDELETE(thr);
    thr = 0;
  }
  return thr;
}

////
// Dump
////

void LsNetworkInput::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<LsNetworkInput[%p,%d]: main=[%p]>\n"), this, sizeof(LsNetworkInput), main_);
  LmNetworkInput::Dump(f, indent + 1);
}

////
// open_log
////

void LsNetworkInput::open_log()
{
  // logf_.Init("ls", "in", main_->LevelDBC()->LevelID());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void LsNetworkInput::close_log()
{
  // logf_.Close();
}

////
// register_message_handlers
////


void LsNetworkInput::register_message_handlers()
{
  DECLARE_TheLineNum;

  SetDefaultHandler((MsgHandler) &LsNetworkInput::handle_Default);
  RegisterHandler(SMsg::LS_ACTION, (MsgHandler)&LsNetworkInput::handle_SMsg_LS_Action);
}


////
// handle_Default
////

void LsNetworkInput::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsNetworkInput, handle_Default);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_LS_Action
////

void LsNetworkInput::handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsNetworkInput, handle_SMsg_LS_Action);
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
    SetDone();
    break;
  case SMsg_LS_Action::ACTION_CHECK_IDLE_PLAYERS:
    handle_SMsg_LS_Action_CheckIdlePlayers();
    break;
  case SMsg_LS_Action::ACTION_CHECK_IDLE_CLIENTS:
    handle_SMsg_LS_Action_CheckIdleClients();
    break;
  case SMsg_LS_Action::ACTION_HEARTBEAT:
//    TLOG_Log(_T("%s: network input thread running"), method);
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}

////
// handle_SMsg_LS_Action_CheckIdlePlayers
////

void LsNetworkInput::handle_SMsg_LS_Action_CheckIdlePlayers()
{
  DEFMETHOD(LsNetworkInput, handle_SMsg_LS_Action_CheckIdlePlayers);
  DECLARE_TheLineNum;
  // TLOG_Debug(_T("%s: checking idle players"), method);
  // get list of players
  LsPlayerList plist;
  main_->PlayerSet()->GetPlayerList(plist);
  for (LsPlayerList::iterator i = plist.begin(); !(bool)(i == plist.end()); ++i) {
    LsPlayer* player = *i;
#if 0
    // check if last incoming message was past the idle timeout
    if (LmUtil::TimeSince(player->LastUpdateUpdate()) > PLAYER_TIMEOUT) {
      //      TLOG_Debug(_T("%s: idle timeout for player %u [%p]"), method, player->PlayerID(), player);
      fake_player_logout(player);
    }
#endif
    if (player->IdleTime() > PLAYER_TIMEOUT) {
      //      TLOG_Debug(_T("%s: idle timeout for player %u [%p]"), method, player->PlayerID(), player);
      fake_player_logout(player);
    }
  }
}

////
// handle_SMsg_LS_Action_CheckIdleClients
////

void LsNetworkInput::handle_SMsg_LS_Action_CheckIdleClients()
{
  DEFMETHOD(LsNetworkInput, handle_SMsg_LS_Action_CheckIdleClients);
  DECLARE_TheLineNum;
  // TLOG_Debug(_T("%s: checking idle clients"), method);
  // get list of clients
  LmConnectionList conn_list;
  main_->ConnectionSet()->GetConnectionList(conn_list);
  // TLOG_Debug(_T("%s: number of connections: %d"), method, conn_list.size());
  // check each network connection
  for (LmConnectionList::iterator i = conn_list.begin(); !(bool)(i == conn_list.end()); ++i) {
    LmConnection* conn = *i;
    // if connection is a game server, never timeout
    if (conn->Type() == LmConnection::CT_GSRV) {
      continue;
    }
    // check if last incoming message was past the idle timeout
    if (conn->IdleIn() > CLIENT_TIMEOUT) {
      //      TLOG_Debug(_T("%s: idle timeout for connection (%c,%d) [%p]"), method, conn->Type(), conn->ID(), conn);
      close_connection(conn);
    }
  }
}

////
// close_connection
////

void LsNetworkInput::close_connection(LmConnection* conn)
{
  DEFMETHOD(LsNetworkInput, close_connection);
  DECLARE_TheLineNum;
  if (!conn) {
    TLOG_Error(_T("%s: trying to close null connection!"), method);
    return;
  }
  //  TLOG_Debug(_T("%s: closing connection [%p]"), method, conn);
  // check if there is a thread reading a message from that connection, and cancel it if so
#if 0
  lock_.Lock();
  for (std::list<reader_t>::iterator i = readers_.begin(); i != readers_.end(); ++i) {
    if ((*i).conn == conn) {
      LsMessageReader* thr = (*i).thread;
      if (thr->IsRunning()) {
	thr->Cancel();
      }
    }
  }
  lock_.UnLock();
#endif
  // fake a logout message from the client
  SMsg_Logout msg;
  LmSrvMesgBuf* mbuf;
  mbuf = main_->BufferPool()->AllocateBuffer(msg.MessageSize());
  mbuf->ReadMessage(msg);
  if (main_->InputDispatch()->DispatchMessage(mbuf, conn) < 0) {
    TLOG_Error(_T("%s: could not dispatch fake logout message for connection [%p]"), method, conn);
    // return message buffer
    main_->BufferPool()->ReturnBuffer(mbuf);
    // couldn't dispatch message, so close ourselves
    main_->ConnectionSet()->RemoveConnection(conn);
  }
}

////
// fake_player_logout
////

void LsNetworkInput::fake_player_logout(LsPlayer* player)
{
  DEFMETHOD(LsNetworkInput, fake_player_logout);
  DECLARE_TheLineNum;
  LmConnection* conn = player->Connection();
  // create logout message
  RMsg_Logout msg_logout;
  msg_logout.Init(RMsg_Logout::FINAL);
  // put it in a proxy message
  SMsg_Proxy msg_proxy;
  msg_proxy.InitProcess(player->PlayerID(), msg_logout);
  // allocate message buffer, initialize it
  LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg_proxy.MessageSize());
  mbuf->ReadMessage(msg_proxy);
  // send it
  if (main_->InputDispatch()->DispatchMessage(mbuf, conn) < 0) {
    TLOG_Error(_T("%s: could not dispatch fake player logout message for connection [%p]"), method, conn);
    // return message buffer
    main_->BufferPool()->ReturnBuffer(mbuf);
    // remove player from game
    main_->PlayerSet()->RemovePlayer(player);
  }
}
