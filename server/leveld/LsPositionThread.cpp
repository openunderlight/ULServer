// LsPositionThread.cpp  -*- C++ -*-
// $Id: LsPositionThread.cpp,v 1.38 1998-04-16 19:08:18-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsPositionThread.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include "LsPositionThread.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "LsMain.h"
#include "LmGlobalDB.h"
#include "LmLevelDBC.h"
#include "LmSockAddrInet.h"
#include "LmSockType.h"
#include "LmConnection.h"
#include "LmMesgHdr.h"
#include "LmSrvMesgBuf.h"
#include "RMsg.h"
#include "RMsg_Update.h"
#include "LsPlayerSet.h"
#include "LsPlayer.h"
#include "LmConnectionSet.h"
#include "SMsg_LS_Action.h"
#include "LsUtil.h"
#include "LsMacros.h"

#ifndef USE_INLINE
#include "LsPositionThread.i"
#endif

////
// Constructor
////

LsPositionThread::LsPositionThread(LsMain* lsmain)
  : LmThread(lsmain->BufferPool(), lsmain->Log() /* &logf_ */),
    main_(lsmain),
    usock_(lsmain->SocketUDP()),
    msgbuf_(0)
{
  // message buffer must be large enough for an RMsg_Update message
  RMsg_Update m1;
  msgbuf_ = LmNEW(LmSrvMesgBuf(m1.MaxMessageSize()));
  open_log();
  register_message_handlers();
}

////
// Destructor
////

LsPositionThread::~LsPositionThread()
{
  usock_->Shutdown();
  LmDELETE(msgbuf_);
  close_log();
}

////
// Run
////

void LsPositionThread::Run()
{
  LmUtil::BlockAllSignals();

  // modified LmThread mainloop
  while (!Done()) {
    HandleUDP();
    HandleAvailableMessages();
    Yield();
  }
  DoneRunning();
}

////
// HandleUDP
////

void LsPositionThread::HandleUDP()
{
  DEFMETHOD(LsPositionThread, HandleUDP);
  // wait up to 1/2 second for message to be available on usock_
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(usock_->Descriptor(), &fds);

 // TLOG_Error(_T("%s: In HandleUDP() in Position Thread; pid = %d"), method, getpid());
#ifdef WIN32
  int rc = select(usock_->Descriptor() + 1, &fds, NULL, NULL, &tv);
#else
  int rc = pth_select(usock_->Descriptor() + 1, &fds, NULL, NULL, &tv);
#endif
  if (rc < 0) {
    TLOG_Error(_T("%s: select: %s"), method, strerror(errno));
    return;
  }
  // if nothing there, return
  if (rc == 0) {
    return;
  }
  // the only descriptor set should be usock_'s, but might as well check
  if (!FD_ISSET(usock_->Descriptor(), &fds)) {
    TLOG_Error(_T("%s: UDP socket descriptor not set?"), method);
    return;
  }
  // else, read message
  LmSockAddrInet caddr;
  LmMesgHdr hdr;
#ifdef WIN32
  unsigned char buffer[64];
  rc = usock_->RecvFrom(buffer, 64, caddr, MSG_PEEK);
  memcpy((void*)hdr.HeaderAddress(), buffer, hdr.HeaderSize());
#else
  // don't actually read bytes, since it may discard the rest of the UDP datagram
  rc = usock_->RecvFrom(hdr.HeaderAddress(), hdr.HeaderSize(), caddr, MSG_PEEK);
#endif
  if (rc < 0) {  // error?
#ifdef WIN32
	int error = WSAGetLastError();
#endif
    TLOG_Error(_T("%s: recvfrom: %s"), method, strerror(errno));
    return;
  }
  if (rc < hdr.HeaderSize()) {  // read a full header?
    TLOG_Error(_T("%s: incomplete header, %d bytes read"), method, rc);
    return;
  }
  hdr.SetByteOrder(ByteOrder::NETWORK);
  hdr.ConvertToHost();
  // init buffer
  msgbuf_->ReadHeader(hdr);
  // read entire message, including header
  rc = usock_->RecvFrom(msgbuf_->BufferAddress(), msgbuf_->BufferSize());
  if (rc < 0) {
    TLOG_Error(_T("%s: recvfrom: %s"), method, strerror(errno));
    return;
  }
  // read full message?
  if (rc != msgbuf_->BufferSize()) {
    TLOG_Error(_T("%s: only read %d bytes, expected %d"), method, rc, msgbuf_->BufferSize());
    return;
  }


  //TLOG_Debug(_T("%s: mtype=%d msize=%d ip=%s:%d"), method, hdr.MessageType(), hdr.MessageSize(), caddr.AddressString(), caddr.Port());
  // dispatch
  switch (hdr.MessageType()) {
  case RMsg::UPDATE:
    handle_RMsg_Update(msgbuf_, caddr);
    break;
  default:
    TLOG_Error(_T("%s: unknown message type %d received"), method, hdr.MessageType());
    break;
  }
}

////
// Dump
////

void LsPositionThread::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsPositionThread[%p,%d]: main=[%p] usock=[%p]>\n"), this, sizeof(LsPositionThread),
	  main_, usock_);
  msgbuf_->Dump(f, indent + 1);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void LsPositionThread::open_log()
{
  // logf_.Init("ls", "pos", main_->LevelDBC()->LevelID());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void LsPositionThread::close_log()
{
  // logf_.Close();
}

////
// register_message_handlers
////

void LsPositionThread::register_message_handlers()
{
  // default message handler
  SetDefaultHandler((MsgHandler)&LsPositionThread::handle_Default);
  // SMsg_LS_* handlers
  RegisterHandler(SMsg::LS_ACTION, (MsgHandler)&LsPositionThread::handle_SMsg_LS_Action);
}


////
// handle_Default
////

void LsPositionThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsPositionThread, handle_Default);
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_LS_Action
////

void LsPositionThread::handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsPositionThread, handle_SMsg_LS_Action);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_LS_Action, false); // don't send error
  // process
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  switch (msg.Action()) {
  case SMsg_LS_Action::ACTION_EXIT:
    TLOG_Log(_T("%s: exiting"), method);
    SetDone();
    break;
  case SMsg_LS_Action::ACTION_HEARTBEAT:
//    TLOG_Log(_T("%s: position thread running"), method);
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}

////
// handle_RMsg_Update
////

void LsPositionThread::handle_RMsg_Update(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr)
{
  DEFMETHOD(LsPositionThread, handle_RMsg_Update);

  // read the message
  RMsg_Update msg;
  if (msg.Read(*msgbuf) < 0) {
    TLOG_Error(_T("%s: could not read message, type %d"), method, msgbuf->Header().MessageType());
    return;
  }
  lyra_id_t playerid = msg.PlayerID();
  // check that message came from player in the game
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    TLOG_Warning(_T("%s: update from player %u, not in game"), method, playerid);
    return;
  }

  //    struct in_addr b1;
  //b1.s_addr = player->IPAddress();
  // TCHAR aconnstr[20];
  //_tcsnccpy(aconnstr, inet_ntoa(b1), sizeof(aconnstr));
  //TLOG_Warning(_T("%s: top update  player %u, connection IP = %s, msg PlayerID = %u"), method, playerid, aconnstr, msg.PlayerID());


  // if the peerupdate player ID = 0, and x = 0, it's a hacked "reset port" 
  // packet from the level server with the port # in the "Y" field
  if ((msg.PlayerID() == 0) &&
	  (msg.PeerUpdate().X() == 0)) {
	player->SetUpdateAddress(player->IPAddress(), msg.PeerUpdate().Y());
    TLOG_Debug(_T("%s: leveld port reset to %d for player %u"), method, msg.PeerUpdate().Y(), playerid);
	return;
  }

 
  // struct in_addr a1;
  // a1.s_addr = player->IPAddress();
  // TCHAR connstr[20];
  //_tcsnccpy(connstr, inet_ntoa(a1), sizeof(connstr));
  // TLOG_Warning(_T("%s: player %u, connection IP = %s"), method, playerid, connstr);


  // check IP address for possible spoofing
  if (player->IPAddress() != caddr.IPAddress()) {
    struct in_addr a1;
    a1.s_addr = player->IPAddress();
    TCHAR connstr[20];
   _tcsnccpy(connstr, inet_ntoa(a1), sizeof(connstr));
    TLOG_Warning(_T("%s: possible spoof, player %u, connection IP = %s, udp IP = %s:%d"), method,
                 playerid, connstr, caddr.AddressString(), caddr.Port());
    return;
  }

  // update player's info
  
  //TLOG_Debug(_T("%s: received peer update from player %u"), playerid);
  msg.PeerUpdate().SetRealtimeID(player->RealtimeID());

  //TLOG_Debug(_T("got peer update packet from player %u on port %d; x= %d, y=%d, z=%d, rtid=%d"), playerid, caddr.Port(), msg.PeerUpdate().X(), msg.PeerUpdate().Y(), msg.PeerUpdate().Z(), player->RealtimeID());

  player->SetPlayerUpdate(msg.PeerUpdate());
}
