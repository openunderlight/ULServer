// GsUtil.cpp  -*- C++ -*-
// $Id: GsUtil.cpp,v 1.22 1998/02/12 23:33:31 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsUtil.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

#include <list>
#include <parallel/algo.h>

#include "GsUtil.h"
#include "GsMain.h"
#include "LmThread.h"
#include "LmThreadPool.h"
#include "LmMesg.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmLogFile.h"
#include "GsOutputDispatch.h"
#include "GsInputDispatch.h"
#include "GsPlayerSet.h"
#include "GMsg_Error.h"
#include "GMsg_Logout.h"
#include "SMsg_All.h"
#include "RMsg_LoginAck.h"
#include "RMsg_RoomLoginAck.h"
#include "LmLevelDBC.h"
#include "LmServerDBC.h"
#include "PThMutex.h"
#include "LmLocker.h"
#include "../../libsrc/shared/SharedConstants.h"
#include "GsPlayer.h"
#include "LmItemDBC.h"
#include "LmPlayerDBC.h"

#ifndef USE_INLINE
#include "GsUtil.i"
#endif

DECLARE_TheFileName;

////
// SendInternalMessage
///

void GsUtil::SendInternalMessage(GsMain* main, LmMesg& msg, LmThread* thr)
{
  DECLARE_TheLineNum;
  // allocate message buffer
  LmSrvMesgBuf* mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize());
  // read message into it
  mbuf->ReadMessage(msg);
  // send to thread, null connection
  thr->PassMessage(mbuf, 0);
}

int GsUtil::SendInternalMessage(GsMain* main, LmMesg& msg, int threadid)
{
  DECLARE_TheLineNum;
  // look up targett hread
  LmThread* thr = main->ThreadPool()->GetThread(threadid);
  if (!thr) {
    return -1;
  }
  SendInternalMessage(main, msg, thr);
  // return successful
  return 0;
}

////
// BroadcastInternalMessage
////

void GsUtil::BroadcastInternalMessage(GsMain* main, LmMesg& msg, int threadid, bool sendself)
{
  DEFMETHOD(GsUtil, BroadcastInternalMessage);
  DECLARE_TheLineNum;
  // get ids of all threads in server
  std::list<int> tids;
  main->ThreadPool()->GetThreadIDs(tids);
  // if not sending to source thread, remove it from list
  if (!sendself) {
    remove(tids.begin(), tids.end(), threadid);
  }
  tids.sort();
  tids.unique();
  // allocate message buffer
  LmSrvMesgBuf* mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize(), tids.size());
  // read message into it
  mbuf->ReadMessage(msg);
  // send to each thread, from null connection (internal)
  for (std::list<int>::iterator i = tids.begin(); !(bool)(i == tids.end()); ++i) {
    LmThread* thr = main->ThreadPool()->GetThread(*i);
    if (thr) {
      // main->Log()->Debug("%s: sending message[%p] to thread %d", method, mbuf, *i);
      thr->PassMessage(mbuf, 0);
    }
    else {
      main->Log()->Error(_T("%s: threadid %d not found in pool?"), method, *i);
    }
  }
}

////
// BroadcastInternalMessagePlayers - broadcast message to all running player threads
////

void GsUtil::BroadcastInternalMessagePlayers(GsMain* main, LmMesg& msg)
{
  DEFMETHOD(GsUtil, BroadcastInternalMessagePlayers);
  // get ids of all threads in server
  std::list<int> tids;
  main->ThreadPool()->GetThreadIDs(tids);
  tids.sort();
  tids.unique();
  // allocate message buffer
  LmSrvMesgBuf* mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize(), tids.size());
  // read message into it
  mbuf->ReadMessage(msg);
  // send to each thread whose id is > 0 (player threads), from null connection (internal)
  for (std::list<int>::iterator i = tids.begin(); !(bool)(i == tids.end()); ++i) {
    if ((*i) <= 0) {
      continue;
    }
    LmThread* thr = main->ThreadPool()->GetThread(*i);
    if (thr) {
      // main->Log()->Debug("%s: sending message[%p] to thread %d", method, mbuf, *i);
      thr->PassMessage(mbuf, 0);
    }
    else {
      main->Log()->Error(_T("%s: threadid %d not found in pool?"), method, *i);
    }
  }
}

////
// Send_Error - depending on if connection is a client, or a level server, send
//   appropriate kind of error message
////

void GsUtil::Send_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...)
{
  DECLARE_TheLineNum;
  if (conn->Type() == LmConnection::CT_CLIENT) {
    // connected to client, send GMsg_Error
    GMsg_Error msg;
    TCHAR errstring[GMsg_Error::ERRMSG_LEN * 2];  // slack space, don't have snprintf
    va_list args;

    va_start(args, fmt);
   _vstprintf(errstring, fmt, args);
    va_end(args);

    msg.Init(msgtype, errstring);
    main->OutputDispatch()->SendMessage(&msg, conn);
  }
  else {
    // otherwise, send SMsg_Error
    SMsg_Error msg;
    TCHAR errstring[SMsg_Error::ERRMSG_LEN * 2];  // slack space, don't have snprintf
    va_list args;
    
    va_start(args, fmt);
   _vstprintf(errstring, fmt, args);
    va_end(args);
    
    msg.Init(msgtype, errstring);
    main->OutputDispatch()->SendMessage(&msg, conn);
  }
}

////
// Send_GMsg_Error
////

void GsUtil::Send_GMsg_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...)
{
  DECLARE_TheLineNum;
  GMsg_Error msg;
  TCHAR errstring[GMsg_Error::ERRMSG_LEN * 2];  // slack space, don't have snprintf
  va_list args;

  va_start(args, fmt);
 _vstprintf(errstring, fmt, args);
  va_end(args);

  msg.Init(msgtype, errstring);
  main->OutputDispatch()->SendMessage(&msg, conn);
}

////
// Send_SMsg_Error
////

void GsUtil::Send_SMsg_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...)
{
  DECLARE_TheLineNum;
  SMsg_Error msg;
  TCHAR errstring[SMsg_Error::ERRMSG_LEN * 2];  // slack space, don't have snprintf
  va_list args;

  va_start(args, fmt);
 _vstprintf(errstring, fmt, args);
  va_end(args);

  msg.Init(msgtype, errstring);
  main->OutputDispatch()->SendMessage(&msg, conn);
}

////
// Send_RMsg_RoomLoginAck - send message to player if there's a problem
//  going to the new room
////

void GsUtil::Send_RMsg_RoomLoginAck(GsMain* main, LmConnection* conn, int status)
{
  DECLARE_TheLineNum;
  RMsg_RoomLoginAck msg;
  msg.Init(status, 0);
  main->OutputDispatch()->SendMessage(&msg, conn);
}

////
// Send_RMsg_LevelLoginAck - send message to player if there's a problem
//  going to a new level
////

void GsUtil::Send_RMsg_LevelLoginAck(GsMain* main, LmConnection* conn, int status, lyra_id_t roomid, lyra_id_t levelid)
{
  DECLARE_TheLineNum;
  RMsg_LoginAck msg;
  msg.Init(status, roomid, levelid);
  main->OutputDispatch()->SendMessage(&msg, conn);
}



////
// FakeLogout - fake a logout message from the given connection/player, dispatch it
////

void GsUtil::FakeLogout(GsMain* main, LmConnection* conn)
{
  DEFMETHOD(GsUtil, FakeLogout_1);
  DECLARE_TheLineNum;
//  main->Log()->Debug("%s: logging out (fake logout 1 - ghost?) conn [%p] (%c,%u)", method, conn, conn->Type(), conn->ID());

  // fake a logout message from the connection
  LmSrvMesgBuf* mbuf;
  bool is_player = (conn->Type() == LmConnection::CT_CLIENT);
  if (is_player) {
    // client
    GMsg_Logout msg;
    msg.Init(GMsg_Logout::LOGOUT_FAKE);
    mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize());
    mbuf->ReadMessage(msg);
  }
  else {
    // unknown, or other server
    SMsg_Logout msg;
    mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize());
    mbuf->ReadMessage(msg);
  }

  // dispatch it
  if (main->InputDispatch()->DispatchMessage(mbuf, conn) < 0) {
    //main->Log()->Error(_T("%s: could not dispatch fake logout message for conn [%p]"), method, conn);
    // return message buffer
    main->BufferPool()->ReturnBuffer(mbuf);
    // if player, remove from player set
    if (is_player) {
      GsPlayer* player = main->PlayerSet()->GetPlayer(conn->ID());
      if (player) {
	main->PlayerSet()->RemovePlayer(player);
      }
    }
    // couldn't dispatch message, so close ourselves
	main->ConnectionSet()->RemoveConnection(conn); //*****
//	conn->Disable();
  }
}

void GsUtil::FakeLogout(GsMain* main, GsPlayer* player)
{
  DEFMETHOD(GsUtil, FakeLogout_2);
  DECLARE_TheLineNum;
  LmConnection* conn = player->Connection();
  lyra_id_t playerid = player->PlayerID();

//  main->Log()->Debug("%s: logging out (fake logout 2 - ghost?) player %u", method, playerid);
  // create a logout message
  LmSrvMesgBuf* mbuf;
  GMsg_Logout msg;
  msg.Init(GMsg_Logout::LOGOUT_FAKE);
  mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize());
  mbuf->ReadMessage(msg);
  // get the player's thread
  LmThread* pthr = main->ThreadPool()->GetThread(playerid);

  // close connection only if id's match
  if (conn && (conn->Type() == LmConnection::CT_CLIENT) && (conn->ID() == playerid)) {
	main->ConnectionSet()->RemoveConnection(conn); //*****
//	conn->Disable();
  }

  if (!pthr) {
    main->Log()->Warning(_T("%s: could not get thread for player %u"), method, playerid);
    // do the best we can to remove player
    main->BufferPool()->ReturnBuffer(mbuf);
    main->PlayerSet()->RemovePlayer(player);
  }
  else {
    // fake a logout message from the "null" connection, since player's connection may not be 
    // active or actually correspond to the player (due to some race condition)
    main->Log()->Debug(_T("%s: faking logout message from the null connection for player %u"), method, playerid);
    pthr->PassMessage(mbuf, 0);

  }
}

////
// ConnectToLevelServer
////

LmConnection* GsUtil::ConnectToLevelServer(GsMain* main, const LmLevelDBC* ldb)
{
  DEFMETHOD(GsUtil, ConnectToLevelServer);
  static PThMutex lock; // lock for this method
  static bool lock_init = false;
  // only do this once
  if (!lock_init) {
    lock_init = true;
    lock.Init();
  }
  LmLocker mon(lock); // lock during method duration
  // check level
  if (!ldb) {
    main->Log()->Error(_T("%s: null level!"), method);
    return 0;
  }
  lyra_id_t levelid = ldb->LevelID();
  // main->Log()->Debug("%s: connecting to level %u", method, levelid);
  // look up connection
  LmConnection* conn = main->ConnectionSet()->GetConnection(LmConnection::CT_LSRV, levelid);
  // if there is no existing connection
  if (!conn) {
    // main->Log()->Debug("%s: making new connection", method);
    // determine ip and port
    unsigned long level_ip = main->ServerDBC()->LevelServerIP(levelid);
    int port = main->ServerDBC()->LevelServerPort(levelid);
    if ((level_ip == 0) || (port == 0)) {
      main->Log()->Error(_T("%s: could not find IP/port for level %u (ip=%lu, port=%d)"), method, levelid, level_ip, port);
      return 0;
    }
    // create new socket, connect to level server
    LmSockAddrInet saddr;
    saddr.Init(level_ip, port);
    LmSocket sock;
    sock.Socket(LmSockType::Inet_Stream());
    if (sock.Connect(saddr) < 0) {
      main->Log()->Error(_T("%s: could not connect to level server: %s"), method, strerror(errno));
      return 0;
    }
    // add socket to set of connections
    conn = main->ConnectionSet()->AllocateConnection(sock);
    if (!conn) {
      main->Log()->Error(_T("%s: could not allocate connection"), method);
      sock.Close();
      return 0;
    }
    // change connection type
    conn->SetMessageRange(SMsg::MIN, SMsg::MAX);
    main->ConnectionSet()->UpdateConnection(conn, LmConnection::CT_LSRV, levelid);
    //conn->Dump(main->Log()->Stream(), 2);
    // server login
    // main->Log()->Debug("%s: logging into level %d", method, levelid);
    SMsg_Login msg;
    msg.Init(LmConnection::CT_GSRV, main->ServerPort());
    if (main->OutputDispatch()->SendMessage(&msg, conn) < 0) {
      main->Log()->Error(_T("%s: could not send server login message"), method);
      // TODO: remove from connection set?
    }
  }
  // main->Log()->Debug("%s: returning connection [%p] (%c,%d)", method, conn, conn->Type(), conn->ID());
  return conn;
}



////
// NightmareXP - return xp for killing given nightmare
////

int GsUtil::NightmareXP(int mare_index)
{
  int xp = 0; // default
  switch (mare_index) {
  default:
  case Avatars::EMPHANT:
    xp = 50;
    break;
  case Avatars::BOGROM:
    xp = 250;
    break;
  case Avatars::AGOKNIGHT:
    xp = 1000;
    break;
  case Avatars::SHAMBLIX:
    xp = 10000;
    break;
  case Avatars::HORRON:
    xp = 25000;
    break;
  }
  return xp;
}

////
// HandleItemError - handle error returned from LmItemDBC
////

void GsUtil::HandleItemError(GsMain* main, const TCHAR* calling_method, int rc, int sc)
{
  static bool send_mail = true;
  switch (rc) {
  case 0:
    return;
  case LmItemDBC::MYSQL_ERROR: {
    main->Log()->Error(_T("%s: itemdb fatal error encountered; sqlcode=%d"), calling_method, sc);
    if (send_mail) {
      // send mail to database admin
      TCHAR hname[256];
	// *** STRING LITERAL ***
     _stprintf(hname, _T("(unknown)"));
      gethostname(hname, sizeof(hname));
      LmUtil::SendMail(_T("gamed@underlight"), main->ServerDBC()->DatabaseAdminEmail(), _T("Underlight: database error"),
		       _T("HostName: %s\n")
		       _T("Server Info: port %d, pid %lu\n")
		       _T("Error: item database fatal error in method %s\n")
		       _T("Reason: sqlcode %d\n"),
		       hname, main->ServerPort(), main->ServerPid(),
		       calling_method, sc);
      send_mail = false; // only send mail once, since we are restarting anyway
    }
#ifdef WIN32 
	main->SetSIGTERM(true);
#else
    kill(getpid(), SIGTERM);
#endif
  }
  break;
  default:
    main->Log()->Error(_T("%s: itemdb unknown return code %d; sqlcode=%d"), calling_method, rc, sc);
    break;
  }
}

////
// HandlePlayerError - handle error returned from LmPlayerDBC
////

void GsUtil::HandlePlayerError(GsMain* main, const TCHAR* calling_method, 
								int rc, int sc, bool exit)
{
  static bool send_mail = true;
  switch (rc) {
  case 0:
    return;
  case LmPlayerDBC::MYSQL_ERROR: { 
    main->Log()->Error(_T("%s: playerdb fatal error encountered; sqlcode=%d"), calling_method, sc);
    if (send_mail) {
      // send mail to database admin
      TCHAR hname[256];
	  	// *** STRING LITERAL ***
     _stprintf(hname, _T("(unknown)"));
      gethostname(hname, sizeof(hname));
      LmUtil::SendMail(_T("gamed@underlight"), main->ServerDBC()->DatabaseAdminEmail(), _T("Underlight: database error"),
		       _T("HostName: %s\n")
		       _T("Server Info: port %d, pid %lu\n")
		       _T("Error: player database fatal error in method %s\n")
		       _T("Reason: sqlcode %d\n"),
		       hname, main->ServerPort(), main->ServerPid(),
		       calling_method, sc);
      send_mail = false; // only do this once
    } 
// shut down server
	if (exit) {
#ifdef WIN32 
		main->SetSIGTERM(true);
#else
		kill(getpid(), SIGTERM);
#endif
	}
  }
  break;
  default:
    main->Log()->Error(_T("%s: playerdb unknown return code %d; sqlcode=%d"), calling_method, rc, sc);
    break;
  }
}
