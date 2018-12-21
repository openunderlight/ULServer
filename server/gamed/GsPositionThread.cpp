// GsPositionThread.cpp  -*- C++ -*-
// $Id: GsPositionThread.cpp,v 1.42 1998/04/17 02:08:10 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsPositionThread.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>


#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "GsPositionThread.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "GsMain.h"
#include "LmGlobalDB.h"
#include "LmLevelDBC.h"
#include "LmSockAddrInet.h"
#include "LmSockType.h"
#include "LmConnection.h"
#include "LmMesgHdr.h"
#include "LmSrvMesgBuf.h"
#include "RMsg_PlayerUpdate.h"
#include "RMsg_Update.h"
//#include "RMsg_PeerUpdate.h"
#include "GsPlayerSet.h"
#include "GsPlayer.h"
#include "LmConnectionSet.h"
#include "GsOutputDispatch.h"
#include "SMsg_GS_Action.h"
#include "SMsg_ResetPort.h"
#include "GsUtil.h"
#include "GsMacros.h"
#include "LmLogFile.h"
#include "LmTimer.h"

#ifndef USE_INLINE
#include "GsPositionThread.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

GsPositionThread::GsPositionThread(GsMain* gsmain)
  : LmThread(gsmain->BufferPool(), gsmain->Log() /* &logf_ */ ),
    main_(gsmain),
    usock_(gsmain->SocketUDP()),
    msgbuf_(0),
    ls_udp_in_bytes_(0),
    ls_udp_in_msgs_(0),
    ls_udp_out_bytes_(0),
    ls_udp_out_msgs_(0),
    cl_udp_in_bytes_(0),
    cl_udp_in_msgs_(0),
    cl_udp_out_bytes_(0),
    cl_udp_out_msgs_(0)
{
  DECLARE_TheLineNum;
  // message buffer needs to be large enough to hold a RMsg_Update or RMsg_PlayerUpdate
  RMsg_Update m1;
  RMsg_PlayerUpdate m2;
  int bufsize = MAX(m1.MaxMessageSize(), m2.MaxMessageSize());
  msgbuf_ = LmNEW(LmSrvMesgBuf(bufsize));
  open_log();
  register_message_handlers();
}

////
// Destructor
////

GsPositionThread::~GsPositionThread()
{
  DECLARE_TheLineNum;
  usock_->Shutdown();
  LmDELETE(msgbuf_);
  close_log();
}

////
// Run
////

void GsPositionThread::Run()
{
  //  TLOG_Debug(_T("Position thread pid = %d"), getpid());
  LmUtil::BlockAllSignals()
  DECLARE_TheLineNum;
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

void GsPositionThread::HandleUDP()
{
  DEFMETHOD(GsPositionThread, HandleUDP);
  DECLARE_TheLineNum;

  // wait up to 1/2 second for message to be available on usock_
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500000;
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(usock_->Descriptor(), &fds);
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
  // initialize buffer
  msgbuf_->ReadHeader(hdr);
  // read entire message, including header
  rc = usock_->RecvFrom(msgbuf_->BufferAddress(), msgbuf_->BufferSize());
  if (rc < 0) { // error?
    TLOG_Error(_T("%s: recvfrom: %s"), method, strerror(errno));
    return;
  }
  // get full message?
  if (rc != msgbuf_->BufferSize()) {
    TLOG_Error(_T("%s: only read %d bytes, expected %d"), method, rc, msgbuf_->BufferSize());
    return;
  }
  //  TLOG_Debug(_T("%s: Got UDP update; mtype=%d msize=%d ip=%s:%d"), method, hdr.MessageType(), hdr.MessageSize(), caddr.AddressString(), caddr.Port());
  // dispatch
  switch (hdr.MessageType()) {
  case RMsg::UPDATE: // client->ls update
    cl_udp_in_msgs_++;
    cl_udp_in_bytes_ += rc;
    handle_RMsg_Update_UDP(msgbuf_, caddr);
    break;
  case RMsg::PLAYERUPDATE: // ls->client update
    ls_udp_in_msgs_++;
    ls_udp_in_bytes_ += rc;
    handle_RMsg_PlayerUpdate(msgbuf_, caddr);
    break;
  default:
    TLOG_Error(_T("%s: unknown message type %d received"), method, hdr.MessageType());
    break;
  }
}

////
// Dump
////

void GsPositionThread::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<GsPositionThread[%p,%d]: main=[%p] usock=[%p]>\n"), this, sizeof(GsPositionThread),
	  main_, usock_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("ls udp: in=%d/%d out=%d/%d\n"),
	  ls_udp_in_msgs_, ls_udp_in_bytes_, ls_udp_out_msgs_, ls_udp_out_bytes_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("cl udp: in=%d/%d out=%d/%d\n"),
	  cl_udp_in_msgs_, cl_udp_in_bytes_, cl_udp_out_msgs_, cl_udp_out_bytes_);
  msgbuf_->Dump(f, indent + 1);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void GsPositionThread::open_log()
{
  // logf_.Init("gs", "pos", main_->ServerPort());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void GsPositionThread::close_log()
{
  // logf_.Close();
}

////
// register_message_handlers
////


void GsPositionThread::register_message_handlers()
{
  DECLARE_TheLineNum;
  // default message handler
  SetDefaultHandler((MsgHandler) &GsPositionThread::handle_Default);
  // SMsg_LS_* handlers
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsPositionThread::handle_SMsg_GS_Action);
  // RMsg handles
  RegisterHandler(RMsg::UPDATE, (MsgHandler)&GsPositionThread::handle_RMsg_Update_TCP);
}


////
// handle_Default
////

void GsPositionThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPositionThread, (GsPositionThread::handle_Default));
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_GS_Action
////

void GsPositionThread::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPositionThread, handle_SMsg_GS_Action);
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
    TLOG_Log(_T("%s: exiting"), method);
    SetDone();
    break;
  case SMsg_GS_Action::ACTION_HEARTBEAT:
    TLOG_Log(_T("%s: position thread running"), method);
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %c"), method, msg.Action());
    break;
  }
}

////
// handle_RMsg_Update_UDP: handles update for UDP clients
////

void GsPositionThread::handle_RMsg_Update_UDP(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr)
{
  DEFMETHOD(GsPositionThread, handle_RMsg_Update_UDP);
  DECLARE_TheLineNum;

  // read the message
  RMsg_Update msg;
  if (msg.Read(*msgbuf) < 0) {
    TLOG_Error(_T("%s: could not read message, type %d"), method, msgbuf->Header().MessageType());
    return;
  }
  //msg.Dump(TLOG_Stream(), 2);
  lyra_id_t playerid = msg.PlayerID();
  // check that message came from player in the game
  GsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    //TLOG_Warning(_T("%s: update from player %u, not in game"), method, playerid);
    return;
  }
  // update player info
  // LmTimer timer;
  // timer.Start();
  int rc = player->CheckAndReceiveUpdate(msg.PeerUpdate());

  // timer.Stop();
#ifdef UL3D
  TLOG_Debug(_T("%s: player %u - idle time %d, time %u"), method, playerid, player->IdleTime(), time(NULL));
#endif
  if (rc < 0) {
    // if rc is -1, then log, up to 5 times
    if ((rc == -1) && (player->NumWeaponChecksFailed() < 5)) {		
		player->CheckAndReceiveUpdate(msg.PeerUpdate());
      SECLOG(6, _T("%s: player %u: peer update contained illegal weapon (bm=%d vel=%d fx=%d dmg=%d)"), method,
	     playerid, msg.PeerUpdate().WeaponBitmap(), msg.PeerUpdate().WeaponVelocity(),
	     msg.PeerUpdate().WeaponEffect(), msg.PeerUpdate().WeaponDamage());
    }
    // Note: attack bits were cleared, so we must copy the message back into the buffer
    msgbuf->ReadMessage(msg);
  }
  // check that player is logged into a level
  if (!player->InLevel() || !player->LevelDBC()) {
    //TLOG_Warning(_T("%s: player %u sent update while not in level"), method, playerid);
    return;
  }
  // removed - 5/02 BMP to allow use via a firewall
  // check IP address for possible spoofing
  if (player->ClientAddress().IPAddress() != caddr.IPAddress()) {
	  player->SetUpdateAddress(caddr.IPAddress(), caddr.Port());
    //TCHAR connstr[20];
   //_tcsnccpy(connstr, player->ClientAddress().AddressString(), sizeof(connstr));
    //SECLOG(1, _T("%s: player %u: possible C->S update spoof, connection IP = %s, udp IP = %s:%d"), method,
	   //playerid, connstr, caddr.AddressString(), caddr.Port());
    //return;
  }

  if (caddr.Port() != player->UpdateAddress().Port())
  { // If the client UDP bound port is not what we expect, it means they
	// must be using NAT or something similar. In this case, we need to send
	// the "correct" port to the level server
	  player->SetUpdateAddress(caddr.IPAddress(), caddr.Port());
	  //TLOG_Debug(_T("gamed reset peer update port for player %u to %d"), playerid, caddr.Port());
	  if (!player->Firewall()) // only send on to leveld if we're not forward the UDP updates
		send_SMsg_ResetPort(player->PlayerID(), caddr.Port(), player->LevelConnection());
  }

  // forward to player's level server via UDP
  //TLOG_Debug(_T("%s: redirecting update from player %u to %s:%d"), method, playerid, saddr.AddressString(), saddr.Port());
  //TLOG_Debug(_T("got peer update packet from player %u on port %d; x= %d, y=%d"), playerid, caddr.Port(), msg.PeerUpdate().X(), msg.PeerUpdate().Y());
  usock_->SendTo(msgbuf->BufferAddress(), msgbuf->BufferSize(), player->LevelAddress());
  // update stats
  ls_udp_out_msgs_++;
  ls_udp_out_bytes_ += msgbuf->BufferSize();
}


////
// handle_RMsg_Update_TCP: updates for TCP-only clients - deprecated
////

//#if 0
void GsPositionThread::handle_RMsg_Update_TCP(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPositionThread, handle_RMsg_Update_TCP);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(true);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();

  // accept message
  ACCEPT_MSG(RMsg_Update, true); // send error

  if (conn->ID() != msg.PlayerID()) { 
      TLOG_Error(_T("%s: message from client id %u, not player %u"), method, conn->ID(), msg.PlayerID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("id mismatch")); 
      return; 
    } 

  // process
  // TLOG_Debug(_T("%s: got update"), method);

  //msg.Dump(TLOG_Stream(), 2);
  lyra_id_t playerid = msg.PlayerID();
  // check that message came from player in the game
  GsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    //TLOG_Warning(_T("%s: update from player %u, not in game"), method, playerid);
    return;
  }
  // update player info
  // LmTimer timer;
  // timer.Start();
  int rc = player->CheckAndReceiveUpdate(msg.PeerUpdate());
  // timer.Stop();
  // TLOG_Debug(_T("%s: player %u: rc=%d, time=%lu usec"), method, playerid, rc, timer.MicroSeconds());
  if (rc < 0) {
    // if rc is -1, then log, up to 5 times
    if ((rc == -1) && (player->NumWeaponChecksFailed() < 5)) {
      SECLOG(6, _T("%s: player %u: peer update contained illegal weapon (bm=%d vel=%d fx=%d dmg=%d)"), method,
	     playerid, msg.PeerUpdate().WeaponBitmap(), msg.PeerUpdate().WeaponVelocity(),
	     msg.PeerUpdate().WeaponEffect(), msg.PeerUpdate().WeaponDamage());
    }
    // Note: attack bits were cleared, so we must copy the message back into the buffer
    ACCEPT_MSG(RMsg_Update, true);
  }
  // check that player is logged into a level
  if (!player->InLevel() || !player->LevelDBC()) {
    //TLOG_Warning(_T("%s: player %u sent update while not in level"), method, playerid);
    return;
  }

  //TLOG_Debug("%s: player %u sending TCP position update", method, msg.PlayerID());

  // forward to player's level server via UDP
  //TLOG_Debug(_T("%s: redirecting update from player %u to %s:%d"), method, playerid, saddr.AddressString(), saddr.Port());
  usock_->SendTo(msgbuf->BufferAddress(), msgbuf->BufferSize(), player->LevelAddress());
  
  return;
}
//#endif

////
// handle_RMsg_PlayerUpdate
////

void GsPositionThread::handle_RMsg_PlayerUpdate(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr)
{
  DEFMETHOD(GsPositionThread, handle_RMsg_PlayerUpdate);
  DECLARE_TheLineNum;

  // read the message
  RMsg_PlayerUpdate msg;
  if (msg.Read(*msgbuf) < 0) {
    TLOG_Error(_T("%s: could not read message, type %d"), method, msgbuf->Header().MessageType());
    return;
  }
  //msg.Dump(TLOG_Stream(), 2);
  lyra_id_t playerid = msg.PlayerID();
  // check that message target is in the game
  GsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    //TLOG_Warning(_T("%s: peer update for player %u, not in game"), method, playerid);
    return;
  }
  // check that player is logged into a level
  if (!player->InLevel() || !player->LevelDBC()) {
    //TLOG_Warning(_T("%s: player %u received update while not in level"), method, playerid);
    return;
  }
  // check IP address for possible spoofing
  if (player->LevelAddress().IPAddress() != caddr.IPAddress()) {
    TCHAR levelstr[20];
   _tcsnccpy(levelstr, player->LevelAddress().AddressString(), sizeof(levelstr));
    SECLOG(1, _T("%s: player %u: possible S->C update spoof, level connection IP = %s, udp IP = %s:%d"), method,
	   playerid, levelstr, caddr.AddressString(), caddr.Port());
    return;
  }
  // copy update info into player structure
  // player->ReceivedServerUpdate(msg);
  // forward to player via UDP
  //TLOG_Debug(_T("%s: redirecting player update for player %u to %s:%d"), method, playerid, player->UpdateAddress().AddressString(), player->UpdateAddress().Port());
   if( player->TCPOnly() )
     main_->OutputDispatch()->SendMessage(&msg, player->Connection());
   else
      usock_->SendTo(msgbuf->BufferAddress(), msgbuf->BufferSize(), player->UpdateAddress());

  // update stats
  cl_udp_out_msgs_++;
  cl_udp_out_bytes_ += msgbuf->BufferSize();
}


////
// send_SMsg_ResetPort
////

void GsPositionThread::send_SMsg_ResetPort(lyra_id_t playerid, int port, LmConnection* level_conn)
{
  DEFMETHOD(GsPositionThread, send_SMsg_ResetPort);
  SMsg_ResetPort msg;
  msg.Init(playerid, port);
  main_->OutputDispatch()->SendMessage(&msg, level_conn);
}
