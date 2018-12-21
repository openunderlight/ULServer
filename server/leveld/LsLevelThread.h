// LsLevelThread.h  -*- C++ -*-
// $Id: LsLevelThread.h,v 1.24 1998-02-03 23:40:14-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message handling thread for level-wide messages

#ifndef INCLUDED_LsLevelThread
#define INCLUDED_LsLevelThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmThread.h"
#include "LmLogFile.h"

// forward declarations

class LsMain;
class LmLevelDBC;
class LsLevelState;
class LmMesg;

// the class

class LsLevelThread : public LmThread {

public:

  LsLevelThread(LsMain* lsmain);
  ~LsLevelThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();

private:

  // not implemented
  LsLevelThread(const LsLevelThread&);
  //operator=(const LsLevelThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_* message handlers
  void handle_SMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LevelLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GetServerStatus(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_DumpState(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_RotateLogs(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_ResetPort(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_UniverseBroadcast(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_LS_* message handlers
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // message sending routines
  void send_RMsg_LevelLoginAck(LmConnection* conn, lyra_id_t playerid, int status, lyra_id_t roomid);
  
  void send_RMsg_LevelLoginAck(LmConnection* conn, lyra_id_t playerid, int status, lyra_id_t roomid,
			  unsigned long server_ip, int server_port);

  void send_RMsg_NewlyAwakened(LmConnection* conn, lyra_id_t playerid, 
	  const TCHAR* newly_name, lyra_id_t roomid);

  void send_RMsg_CupSummons(LmConnection* conn, lyra_id_t playerid, 
	  const TCHAR* name);

  void send_SMsg_ServerStatus(LmConnection* conn);
  void send_SMsg_ConnStatus(LmConnection* conn);
  void send_SMsg_PlayerStatus(LmConnection* conn, lyra_id_t playerid);

  // auxiliary message handling routines
  void handle_SMsg_LS_Action_Exit();
  void handle_SMsg_Logout_GSRV(LmConnection* gsconn);

  void open_log();
  void close_log();
  void register_handlers();

  LsMain* main_;         // level server main object

  const LmLevelDBC* dbc_;  // level properties
  LsLevelState* state_;  // level state
  bool saved_to_disk_; // true if last DB saved failed and we saved to disk instead

  // LmLogFile logf_;       // level thread logfile

};

#ifdef USE_INLINE
#include "LsLevelThread.i"
#endif

#endif /* INCLUDED_LsLevelThread */
