// LsUtil.cpp  -*- C++ -*-
// $Id: LsUtil.cpp,v 1.24 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsUtil.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

#include <list>
#include <algorithm>

#include "LsUtil.h"
#include "LsMain.h"
#include "LmThread.h"
#include "LmThreadPool.h"
#include "LmMesg.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "RMsg_All.h"
#include "SMsg_All.h"
#include "LsOutputDispatch.h"
#include "LmLogFile.h"
#include "LmServerDBC.h"
#include "LmLevelDBC.h"
#include "LsPlayer.h"
#include "LsPlayerList.h"
#include "LmItemDBC.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef USE_INLINE
#include "LsUtil.i"
#endif

DECLARE_TheFileName;

////
// SendInternalMessage
///

void LsUtil::SendInternalMessage(LsMain* main, LmMesg& msg, LmThread* thr)
{
  DEFMETHOD(LsUtil, SendInternalMessage_1);
  DECLARE_TheLineNum;
  // allocate message buffer
  LmSrvMesgBuf* mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize());
  // read message into it
  mbuf->ReadMessage(msg);
  // send to thread, null connection
  thr->PassMessage(mbuf, 0);
}

int LsUtil::SendInternalMessage(LsMain* main, LmMesg& msg, int threadid)
{
  DEFMETHOD(LsUtil, SendInternalMessage_2);
  DECLARE_TheLineNum;
  // look up target thread
  LmThread* thr = main->ThreadPool()->GetThread(threadid);
  if (!thr) {
    main->Log()->Error(_T("%s: could not get thread for id %d"), method, threadid);
    return -1;
  }
  SendInternalMessage(main, msg, thr);
  // return successful
  return 0;
}

////
// BroadcastInternalMessage
////

void LsUtil::BroadcastInternalMessage(LsMain* main, LmMesg& msg, int threadid, bool sendself)
{
  DEFMETHOD(LsUtil, BroadcastInternalMessage);
  DECLARE_TheLineNum;
  // get ids of all threads in server
  std::list<int> tids;
  main->ThreadPool()->GetThreadIDs(tids);
  // if not sending to source thread, remove it from list
  if (!sendself) {
    remove(tids.begin(), tids.end(), threadid);
  }
  tids.unique();
  // allocate message buffer
  LmSrvMesgBuf* mbuf = main->BufferPool()->AllocateBuffer(msg.MessageSize(), tids.size());
  // read message into it
  mbuf->ReadMessage(msg);
  // send to each thread, from null connection (internal)
  for (std::list<int>::iterator i = tids.begin(); !(bool)(i == tids.end()); ++i) {
    LmThread* thr = main->ThreadPool()->GetThread(*i);
    if (thr) {
      //main->Log()->Debug("%s: sending message[%p] to thread %d", method, mbuf, *i);
      thr->PassMessage(mbuf, 0);
    }
    else {
      main->Log()->Error(_T("%s: threadid %d not found in pool?"), method, *i);
    }
  }
}

////
// Send_RMsg_Error - send through game server proxy
////

void LsUtil::Send_RMsg_Error(LsMain* main, LsPlayer* player, int msgtype, const TCHAR* fmt, ...)
{
  DECLARE_TheLineNum;
  RMsg_Error msg;
  TCHAR errstring[RMsg_Error::ERRMSG_LEN * 2];  // slack space, don't have snprintf
  va_list args;

  va_start(args, fmt);
 _vstprintf(errstring, fmt, args);
  va_end(args);

  msg.Init(msgtype, errstring);
  LsUtil::Send_SMsg_Proxy(main, player, msg);
}

////
// Send_RMsg_LoginAck
////

void LsUtil::Send_RMsg_LevelLoginAck(LsMain* main, LsPlayer* player, int status, lyra_id_t roomid)
{
  DECLARE_TheLineNum;
  RMsg_LoginAck msg;
  msg.Init(status, roomid, main->LevelDBC()->LevelID());
  LsUtil::Send_SMsg_Proxy(main, player, msg);
}

////
// Send_RMsg_RoomLoginAck
////

void LsUtil::Send_RMsg_RoomLoginAck(LsMain* main, LsPlayer* player, short status, short num_neighbors)
{
  DECLARE_TheLineNum;
  RMsg_RoomLoginAck msg;
  msg.Init(status, num_neighbors);
  LsUtil::Send_SMsg_Proxy(main, player, msg);
}
////
// Send_SMsg_Proxy - send message to player through game server
////

void LsUtil::Send_SMsg_Proxy(LsMain* main, LmConnection* conn, lyra_id_t playerid, LmMesg& msg)
{
  DECLARE_TheLineNum;
  SMsg_Proxy msg_proxy;
  msg_proxy.Init(playerid, SMsg_Proxy::PROXY_FORWARD, msg);
  main->OutputDispatch()->SendMessage(&msg_proxy, conn);
}

void LsUtil::Send_SMsg_Proxy(LsMain* main, LsPlayer* player, LmMesg& msg)
{
  DECLARE_TheLineNum;
  LsUtil::Send_SMsg_Proxy(main, player->Connection(), player->PlayerID(), msg);
}

void LsUtil::Send_SMsg_Proxy(LsMain* main, LsPlayerList& player_list, LmMesg& msg)
{
  DECLARE_TheLineNum;
  // unique the list, in case there are duplicates
  player_list.sort();
  player_list.unique();
  // unfortunately there's currently no way to multicast these through the game
  // server, they must be sent one-by-one.
  LsPlayerList::iterator i;
  for (i = player_list.begin(); !(bool)(i == player_list.end()); ++i) {
    LsPlayer* player = *i;
    LsUtil::Send_SMsg_Proxy(main, player->Connection(), player->PlayerID(), msg);
  }
}

////
// Send_SMsg_Error
////

void LsUtil::Send_SMsg_Error(LsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...)
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
// HandleItemError - handle error returned from LmItemDBC
////

void LsUtil::HandleItemError(LsMain* main, const TCHAR* calling_method, int rc, int sc)
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
      LmUtil::SendMail(_T("leveld@underlight"), main->ServerDBC()->DatabaseAdminEmail(), _T("Underlight: database error"),
		       _T("HostName: %s\n") _T("Server Info: level %d, pid %lu\n")  _T("Error: item database fatal error in method %s\n") _T("Reason: sqlcode %d\n"),
		       hname, main->LevelDBC()->LevelID(), main->ServerPid(),
		       calling_method, sc);
      send_mail = false; // only do this once
    }
    // shut down server
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
