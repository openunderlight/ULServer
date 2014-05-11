// GsGameThread.h  -*- C++ -*-
// $Id: GsGameThread.h,v 1.27 1998-02-03 23:40:06-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message handling thread for master game server

#ifndef INCLUDED_GsGameThread
#define INCLUDED_GsGameThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmThread.h"
#include "LmLogFile.h"

// forward declarations

class GsMain;
class LmLevelDBC;
class LmMesg;
class LmPlayerDB;
class GsPlayer;
class GsPlayerThread;

// the class

class GsGameThread : public LmThread {

public:

  GsGameThread(GsMain* gsmain);
  ~GsGameThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();

private:

  // not implemented
  GsGameThread(const GsGameThread&);
  //operator=(const GsGameThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // GMsg_* message handlers
  void handle_GMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_PreLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_AgentLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // RMsg message handlers
  void handle_RMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg message handlers
  void handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GetServerStatus(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_DumpState(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_RotateLogs(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_DBQueryAckGT(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_GS_* message handlers
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GS_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // message sending routines
  void send_GMsg_LoginAck(LmConnection* conn, time_t conn_time, int status, int suspended_days = 0);
  void send_GMsg_LoginAck(LmConnection* conn, time_t conn_time, int status, char* ip_address, int port_num);
  void send_GMsg_LoginAck(LmConnection* conn, GsPlayer* player, 
	  short max_minutes_online, short session_minutes, unsigned char gamesite = 'L', lyra_id_t gamesite_id = 0);
  void send_GMsg_PreLoginAck(LmConnection* conn, int version, int status,
	  TCHAR* challenge);
  void send_GMsg_ServerDown(int status);

  void send_SMsg_ServerStatus(LmConnection* conn);
  void send_SMsg_ConnStatus(LmConnection* conn);
  void send_SMsg_PlayerStatus(LmConnection* conn, lyra_id_t playerid);

  // auxiliary message handling routines
  void handle_SMsg_GS_Action_Exit();
  void handle_SMsg_GS_Action_Restart();

  GsPlayerThread* start_player_thread(GsPlayer* player);

  void open_log();
  void close_log();
  void register_handlers();

  GsMain* main_;    // game server main object

  // LmLogFile logf_;  // game thread log

};

#ifdef USE_INLINE
#include "GsGameThread.i"
#endif

#endif /* INCLUDED_GsGameThread */
