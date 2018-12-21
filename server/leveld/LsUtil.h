// LsUtil.h  -*- C++ -*-
// $Id: LsUtil.h,v 1.11 1997-10-22 19:22:15-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// utility function class

#ifndef INCLUDED_LsUtil
#define INCLUDED_LsUtil

#ifdef __GNUC__
#pragma interface
#endif

#include <list>

#include "../../libsrc/shared/LyraDefs.h"

// class forward declarations

class LmMesg;
class LmConnection;
class LsMain;
class LsPlayer;
class LsPlayerList;
class LmThread;

// class declarations

struct LsUtil {
  static void SendInternalMessage(LsMain* main, LmMesg& msg, LmThread* thr);
  static int SendInternalMessage(LsMain* main, LmMesg& msg, int threadid);
  static void BroadcastInternalMessage(LsMain* main, LmMesg& msg, int threadid, bool s2s = false);
  static void Send_SMsg_Error(LsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...);
  static void Send_SMsg_Proxy(LsMain* main, LmConnection* conn, lyra_id_t playerid, LmMesg& msg);
  static void Send_SMsg_Proxy(LsMain* main, LsPlayer* player, LmMesg& msg);
  static void Send_SMsg_Proxy(LsMain* main, LsPlayerList& player_list, LmMesg& msg);
  static void Send_RMsg_Error(LsMain* main, LsPlayer* player, int msgtype, const TCHAR* fmt, ...);
  static void Send_RMsg_LevelLoginAck(LsMain* main, LsPlayer* player, int status, lyra_id_t roomid);
  static void Send_RMsg_RoomLoginAck(LsMain* main, LsPlayer* player, short status, short num_neighbors);
  static void HandleItemError(LsMain* main, const TCHAR* method, int rc, int sc);
};

#ifdef USE_INLINE
#include "LsUtil.i"
#endif

#endif /* INCLUDED_LsUtil */
