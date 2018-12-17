// GsUtil.h  -*- C++ -*-
// $Id: GsUtil.h,v 1.13 1997-10-22 19:22:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// utility function class

#ifndef INCLUDED_GsUtil
#define INCLUDED_GsUtil

#ifdef __GNUC__
#pragma interface
#endif

#include "LyraDefs.h"

// class forward declarations

class LmMesg;
class LmConnection;
class LmThread;
class LmLevelDBC;
class GsMain;
class GsPlayer;

// class declarations

struct GsUtil {
  static void SendInternalMessage(GsMain* main, LmMesg& msg, LmThread* thread);
  static int SendInternalMessage(GsMain* main, LmMesg& msg, int threadid);
  static void BroadcastInternalMessage(GsMain* main, LmMesg& msg, int threadid, bool s2s = false);
  static void BroadcastInternalMessagePlayers(GsMain* main, LmMesg& msg);
  static void Send_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...);
  static void Send_GMsg_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...);
  static void Send_SMsg_Error(GsMain* main, LmConnection* conn, int msgtype, const TCHAR* fmt, ...);
  static void Send_RMsg_LevelLoginAck(GsMain* main, LmConnection* conn, int status, lyra_id_t roomid, lyra_id_t levelid);
  static void Send_RMsg_RoomLoginAck(GsMain* main, LmConnection* conn, int status);
  static void FakeLogout(GsMain* main, LmConnection* conn);
  static void FakeLogout(GsMain* main, GsPlayer* player);
  static LmConnection* ConnectToLevelServer(GsMain* main, const LmLevelDBC* ldb);
  static int NightmareXP(int mare_index);
  static void HandleItemError(GsMain* main, const TCHAR* method, int rc, int sc);
  static void HandlePlayerError(GsMain* main, const TCHAR* method, int rc, int sc, bool exit);
  static LmConnection* ConnectToBcastLevelD(GsMain* main);
};

#ifdef USE_INLINE
#include "GsUtil.i"
#endif

#endif /* INCLUDED_GsUtil */
