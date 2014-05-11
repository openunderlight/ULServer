// GsForwardThread.cpp  -*- C++ -*-
// $Id: GsForwardThread.cpp,v 1.27 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsForwardThread.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif
#include <time.h>

#include "GsForwardThread.h"
#include "GsMain.h"
#include "LmLogFile.h"
#include "LmGlobalDB.h"
#include "SMsg_All.h"
#include "RMsg_All.h"
#include "LmSrvMesgBuf.h"
#include "GsUtil.h"
#include "LmConnection.h"
#include "LmThreadPool.h"
#include "LyraMessage.h"
#include "GsMacros.h"
#include "GsOutputDispatch.h"
#include "LmThread.h"
#include "LmMesgBufPool.h"

#ifndef USE_INLINE
#include "GsForwardThread.i"
#endif

////
// Constructor
////

GsForwardThread::GsForwardThread(GsMain* gsmain)
  : LmThread(gsmain->BufferPool(), gsmain->Log() /* &logf_ */ ),
    main_(gsmain)
{
  register_handlers();
}

////
// Destructor
////

GsForwardThread::~GsForwardThread()
{
  close_log();
}

////
// Run
////

void GsForwardThread::Run()
{
  LmUtil::BlockAllSignals();
  LmThread::Run();
}

////
// Dump
////

void GsForwardThread::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GsForwardThread[%p,%d]: main=[%p]>\n"), this, sizeof(GsForwardThread), main_);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void GsForwardThread::open_log()
{
  // logf_.Init(_T("gs"), _T("fwd"), main_->ServerPort());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void GsForwardThread::close_log()
{
  // logf_.Close();
}

////
// message_playerid: determine target playerid from given message
////

lyra_id_t GsForwardThread::message_playerid(LmSrvMesgBuf* msgbuf)
{
  int msg_type = msgbuf->Header().MessageType();
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  switch (msg_type) {
  // each message type is different, even if the method call is the same :(
  case SMsg::PROXY: {
    SMsg_Proxy msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.PlayerID();
    }
  }
  break;
  case SMsg::ITEMDROP: {
    SMsg_ItemDrop msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.PlayerID();
    }
  }
  break;
  case SMsg::ITEMPICKUP: {
    SMsg_ItemPickup msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.PlayerID();
    }
  }
  break;
  case SMsg::GIVEITEM: {
    SMsg_GiveItem msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.TargetID();
    }
  }
  break;
  case SMsg::GIVEITEMACK: {
    SMsg_GiveItemAck msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.TargetID();
    }
  }
  break;
  case SMsg::TAKEITEMACK: {
    SMsg_TakeItemAck msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.TargetID();
    }
  }
  break;
  case SMsg::LOCATEAVATAR: {
    SMsg_LocateAvatar msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.LocatorID();
    }
  }
  break;
  case SMsg::PARTYLEADER: {
    SMsg_PartyLeader msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.PlayerID();
    }
  }
  break;
  case SMsg::SHOWITEM: {
    SMsg_ShowItem msg;
    if (msg.Read(*msgbuf) == 0) {
      playerid = msg.TargetID();
    }
  }
  break;
  default:
    // let the caller deal with the error
    break;
  }
  return playerid;
}

////
// forward_to_player_thread
////

void GsForwardThread::forward_to_player_thread(lyra_id_t playerid, LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsForwardThread, forward_to_player_thread);
  // get target thread
  LmThread* thread = main_->ThreadPool()->GetThread(playerid);
  if (!thread) {
    TLOG_Error(_T("%s: could not find thread for player %u, message type %u, size %u"), method, playerid, 	msgbuf->Header().MessageType(), msgbuf->Header().MessageSize());
	if (msgbuf->Header().MessageType() == SMsg::PROXY) { // log more info for proxy
		SMsg_Proxy msg;
		if (msg.Read(*msgbuf) >= 0) {
//			TLOG_Error(_T("%s: proxy message enclosed type %u, enclosed message size %u"), method, msg.EnclosedMessageType(), msg.EnclosedMessageSize());
			if (msg.EnclosedMessageType() == RMsg::PLAYERMSG) {
				// get new message buffer
				LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg.EnclosedMessageSize());
				// copy message data into buffer (message data is in network order already)
				LmMesgHdr mhdr;
				mhdr.Init(msg.EnclosedMessageType(), msg.EnclosedMessageSize());
				mbuf->ReadHeader(mhdr);
				memcpy(mbuf->MessageAddress(), msg.MessageBytes(), msg.EnclosedMessageSize());
				RMsg_PlayerMsg playermsg;
				if (playermsg.Read(*mbuf) >= 0) {
//					TLOG_Error(_T("%s: proxy message has player message type %u, param1 %u, param2 %u, sender %u"), method, playermsg.MsgType(), playermsg.State1(), playermsg.State2(), playermsg.SenderID());
				} else {
//					TLOG_Error(_T("%s: could not read enclosed player message in proxy"), method);
				}

			}
		}
		else
			TLOG_Error(_T("%s: could not read enclosed message in proxy"), method);
	}

    return;
  }
  // add reference to message buffer, so that when forward thread is done with buffer
  // the player thread still will have access to it
  msgbuf->AddReference();
  // send to target thread
  thread->PassMessage(msgbuf, conn);
}

////
// register_handlers
////


void GsForwardThread::register_handlers()
{
  // default message handler
  SetDefaultHandler((MsgHandler)&GsForwardThread::handle_Default);

  // register SMsg message handlers (forwarding)
  RegisterHandler(SMsg::PROXY, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::ITEMDROP, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::ITEMPICKUP, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::GIVEITEM, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::GIVEITEMACK, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::TAKEITEMACK, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::LOCATEAVATAR, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::PARTYLEADER, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);
  RegisterHandler(SMsg::SHOWITEM, (MsgHandler)&GsForwardThread::handle_SMsg_FWD);

  // register SMsg message handers (non-forwarding)
  RegisterHandler(SMsg::PING, (MsgHandler)&GsForwardThread::handle_SMsg_Ping);

  // register SMsg_GS message handlers
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsForwardThread::handle_SMsg_GS_Action);
}


////
// handle_Default
////

void GsForwardThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsForwardThread, handle_Default);
  HANDLER_ENTRY(false);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_FWD
////

void GsForwardThread::handle_SMsg_FWD(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsForwardThread, handle_SMsg_FWD);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  // get target player
  lyra_id_t playerid = message_playerid(msgbuf);
  if (playerid == Lyra::ID_UNKNOWN) {
    TLOG_Error(_T("%s: could not determine target player for message %s (%d)"), method,
	       LyraMessage::MessageName(msg_type), msg_type);
    return;
  }
  // send to player thread
  forward_to_player_thread(playerid, msgbuf, conn);
}

////
// handle_SMsg_Ping
////

void GsForwardThread::handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsForwardThread, handle_SMsg_Ping);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Ping, false); // don't send error
  // check that connection is not unknown
  if (conn->Type() == LmConnection::CT_UNKNOWN) {
    TLOG_Warning(_T("%s: ping received from unknown connection"), method);
    return;
  }
  // process
  if (msg.PingType() == SMsg_Ping::PING) {
    // TLOG_Debug(_T("%s: ping (%d) from conn [%p] (%c,%d)", method, msg.Nonce(), conn, conn->Type(), conn->ID());
    // return a pong
    msg.InitPong(msg.Nonce());
    main_->OutputDispatch()->SendMessage(&msg, conn);
  }
  else if (msg.PingType() == SMsg_Ping::PONG) {
    int dt = time(NULL) - msg.Nonce();
    // TLOG_Debug(_T("%s: pong (%d) from conn [%p] (%c,%d); dt=%d", method, msg.Nonce(), conn, conn->Type(), conn->ID(), dt);
  }
  else {
    TLOG_Warning(_T("%s: unknown ping %d from conn [%p] (%c,%d)"), method, msg.PingType(), conn, conn->Type(), conn->ID());
  }
}

////
// handle_SMsg_GS_Action
////

void GsForwardThread::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsForwardThread, handle_SMsg_GS_Action);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Action, false); // don't send error
  // TLOG_Debug(_T("%s: action=%c", method, msg.Action());
  // switch on action to perform
  switch (msg.Action()) {
  case SMsg_GS_Action::ACTION_EXIT:
    TLOG_Log(_T("%s: shutting down"), method);
    // signal end of thread's main loop
    SetDone();
    break;
  case SMsg_GS_Action::ACTION_HEARTBEAT:
    TLOG_Log(_T("%s: forward thread running"), method);
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %c"), method, msg.Action());
    break;
  }
}
