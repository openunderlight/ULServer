// LsLevelThread.cpp  -*- C++ -*-
// $Id: LsLevelThread.cpp,v 1.65 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsLevelThread.h"
#endif

#include <stdio.h>
#include <signal.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LsLevelThread.h"
#include "LyraDefs.h"
#include "LsMain.h"
#include "LmConnection.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmDebug.h"
#include "RMsg_All.h"
#include "SMsg_All.h"
#include "LsRoomState.h"
#include "LsLevelState.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "LsOutputDispatch.h"
#include "LmMesg.h"
#include "LmThreadPool.h"
#include "LmMesgBufPool.h"
#include "LsUtil.h"
#include "LmConnectionSet.h"
#include "LsInputDispatch.h"
#include "LsSignalThread.h"
#include "LsMacros.h"
#include "LmSrvMesgBuf.h"
#include "LsPlayerList.h"
#include "LmServerDBC.h"
#include "LmPlayerDB.h"
#include "LmConnectionList.h"
#include "LsCodexText.h"
#include "LmItemDBC.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef USE_INLINE
#include "LsLevelThread.i"
#endif

DECLARE_TheFileName;

////
// Constructor
////

LsLevelThread::LsLevelThread(LsMain* lsmain)
  : LmThread(lsmain->BufferPool(), lsmain->Log(), strdup("LsLevelThread") ),
    main_(lsmain)
{
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread constructor"));
  // initialize members
  dbc_ = main_->LevelDBC();
  state_ = main_->LevelState();
  saved_to_disk_ = false;
  open_log();
  register_handlers();
}

////
// Destructor
////

LsLevelThread::~LsLevelThread()
{
  DECLARE_TheLineNum;
  close_log();
}

////
// Run
////

void LsLevelThread::Run()
{
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::Run"));
  LmUtil::BlockAllSignals();
  LmThread::Run();
  TLOG_Warning(_T("LsLevelThread exiting"));
}

////
// Dump
////

void LsLevelThread::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::Dump"));
  INDENT(indent, f);
 _ftprintf(f, _T("<LsLevelThread[%p,%d]: main=%p leveldb=%p levelstate=%p>\n"), this, sizeof(LsLevelThread),
	  main_, dbc_, state_);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void LsLevelThread::open_log()
{
  TLOG_Debug(_T("In LsLevelThread::open_log"));
  // logf_.Init("ls", "level", dbc_->LevelID());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void LsLevelThread::close_log()
{
  TLOG_Debug(_T("In LsLevelThread::close_log"));
  // logf_.Close();
}

////
// register_handlers
////


void LsLevelThread::register_handlers()
{
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::register_handlers"));
  // default message handler
  SetDefaultHandler((MsgHandler) &LsLevelThread::handle_Default);
  // register SMsg message handlers
  RegisterHandler(SMsg::LOGIN, (MsgHandler)&LsLevelThread::handle_SMsg_Login);
  RegisterHandler(SMsg::LOGOUT, (MsgHandler)&LsLevelThread::handle_SMsg_Logout);
  RegisterHandler(SMsg::SERVERERROR, (MsgHandler)&LsLevelThread::handle_SMsg_Error);
  RegisterHandler(SMsg::LEVELLOGIN, (MsgHandler)&LsLevelThread::handle_SMsg_LevelLogin);
  RegisterHandler(SMsg::PING, (MsgHandler)&LsLevelThread::handle_SMsg_Ping);
  RegisterHandler(SMsg::GETSERVERSTATUS, (MsgHandler)&LsLevelThread::handle_SMsg_GetServerStatus);
  RegisterHandler(SMsg::DUMP_STATE, (MsgHandler)&LsLevelThread::handle_SMsg_DumpState);
  RegisterHandler(SMsg::ROTATE_LOGS, (MsgHandler)&LsLevelThread::handle_SMsg_RotateLogs);
  RegisterHandler(SMsg::RESETPORT, (MsgHandler)&LsLevelThread::handle_SMsg_ResetPort);
  // register SMsg_LS message handlers
  RegisterHandler(SMsg::LS_ACTION, (MsgHandler)&LsLevelThread::handle_SMsg_LS_Action);
  RegisterHandler(SMsg::UNIVERSEBROADCAST, (MsgHandler)&LsLevelThread::handle_SMsg_UniverseBroadcast);
}


////
// handle_Default
////

void LsLevelThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  TLOG_Debug(_T("In LsLevelThread::handle_Default"));
  DEFMETHOD(LsLevelThread, handle_Default);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_Ping
////

void LsLevelThread::handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_Ping"));
  DEFMETHOD(LsLevelThread, handle_SMsg_Ping);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Ping, false); // don't send error
  // process
  if (msg.PingType() == SMsg_Ping::PING) {
    // TLOG_Debug(_T("%s: ping (%d) from conn [%p] (%c,%d)"), method, msg.Nonce(), conn, conn->Type(), conn->ID());
    // return a pong
    msg.InitPong(msg.Nonce());
    main_->OutputDispatch()->SendMessage(&msg, conn);
  }
  else if (msg.PingType() == SMsg_Ping::PONG) {
    int dt = time(NULL) - msg.Nonce();
    // TLOG_Debug(_T("%s: pong (%d) from conn [%p] (%c,%d); dt=%d"), method, msg.Nonce(), conn, conn->Type(), conn->ID(), dt);
  }
  else {
    TLOG_Warning(_T("%s: unknown ping %d from conn [%p] (%c,%d)"), method, msg.PingType(), conn, conn->Type(), conn->ID());
  }
}

void LsLevelThread::handle_SMsg_UniverseBroadcast(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{	
	DEFMETHOD(LsLevelThread, handle_SMsg_UniverseBroadcast);
	DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_UniverseBroadcast"));
	HANDLER_ENTRY(false);
	//TLOG_Warning(_T("%s: recv bcast msg"), method);
	CHECK_CONN_NONNULL();
	CHECK_CONN_ISGAME();
        //TLOG_Warning(_T("%s: cxn not null, is game"), method);
	ACCEPT_MSG(SMsg_UniverseBroadcast, true);
        //TLOG_Warning(_T("%s: got msg"), method);
	LmConnectionList list;
	main_->ConnectionSet()->GetConnectionList(list);
	for(LmConnectionList::iterator c = list.begin(); !(bool)(c == list.end()); ++c) {
		LmConnection* conn2 = *c;
		if(conn2->Type() != LmConnection::CT_GSRV)
			continue;
		//TLOG_Warning(_T("%s: dispatching to gamed"), method);
		main_->OutputDispatch()->SendMessage(&msg, conn2);
	}
		
}

////
// handle_SMsg_LevelLogin
////

void LsLevelThread::handle_SMsg_LevelLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_LevelLogin);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_LevelLogin"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_LevelLogin, true); // send error
  // process
  //  TLOG_Debug(_T("%s: player %u logging in from connection [%p]"), method, msg.PlayerID(), conn);
  // check that level contains target room
  LsRoomState* room = state_->RoomState(msg.RoomID());
  if (!room) {
    TLOG_Warning(_T("%s: player %u entering room %u, not in level"), method, msg.PlayerID(), msg.RoomID());
    send_RMsg_LevelLoginAck(conn, msg.PlayerID(), RMsg_LoginAck::LOGIN_ROOMNOTFOUND, msg.RoomID());
    return;
  }
  // check that room isn't full
  if (room->IsFull()) {
    TLOG_Warning(_T("%s: player %u tried to enter room %u, room full"), method, msg.PlayerID(), msg.RoomID());
    send_RMsg_LevelLoginAck(conn, msg.PlayerID(), RMsg_LoginAck::LOGIN_ROOMFULL, msg.RoomID());
    return;
  }
  // check that player isn't already in level
  if (main_->PlayerSet()->IsInLevel(msg.PlayerID())) {
    TLOG_Warning(_T("%s: player %u already in level"), method, msg.PlayerID());
    send_RMsg_LevelLoginAck(conn, msg.PlayerID(), RMsg_LoginAck::LOGIN_ALREADYIN, msg.RoomID());
    return;
  }
  // get player object from player set
  LsPlayer* player = main_->PlayerSet()->AllocatePlayer(msg.PlayerID());
  if (!player) {
    TLOG_Error(_T("%s: player set is full!"), method);
    send_RMsg_LevelLoginAck(conn, msg.PlayerID(), RMsg_LoginAck::LOGIN_ROOMFULL, msg.RoomID());
    return;
  }
  // update player with rest of info
  player->Init(conn, msg.PlayerName(), msg.AccountType(), msg.RoomID(), msg.PeerUpdate(), msg.IPAddress(),
	       msg.ServerPort(), msg.Avatar(), msg.Description(), msg.NewlyAlert(), msg.NewlyAwakened());
  player->SetHidden((bool)msg.Hidden());
  // if a normal player, add
  if ((msg.AccountType() == LmPlayerDB::ACCT_PLAYER) ||
	  (msg.AccountType() == LmPlayerDB::ACCT_ADMIN && !msg.Avatar().Hidden()) ||
	  (msg.AccountType() == LmPlayerDB::ACCT_PMARE))
	main_->ItemDBC()->ChangeNumDreamers(main_->LevelNum(), 1);

  // send login acknowledge
  send_RMsg_LevelLoginAck(conn, msg.PlayerID(), RMsg_LoginAck::LOGIN_OK, msg.RoomID(),
		     main_->SocketTCP()->SockName().IPAddress(), main_->ServerPort());

  player->SetRealtimeID(state_->GetRealtimeID(player->PlayerID()));
  TLOG_Debug(_T("%s: player(%u) '%s' logged in to room %d, rtid %d"), method, msg.PlayerID(), player->PlayerName(), msg.RoomID(), player->RealtimeID());

  // if player is newly awakened, alert those with the newly alert set 
  // notify room thread of new player
  bool alerts = false;
  if (msg.NewlyAwakened() & !msg.Avatar().Hidden()) {
	LsPlayerList players;
	main_->PlayerSet()->GetPlayerList(players);
	for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
		alerts = (*p)->GetNewlyAlerts();
	    if (alerts && ((*p)->PlayerID() != msg.PlayerID())) { // send alert message
			send_RMsg_NewlyAwakened((*p)->Connection(), (*p)->PlayerID(), msg.PlayerName(), msg.RoomID());
	    }
	}
  }

//	TLOG_Debug(_T("%s: logged in %s; id = %d"), method, msg.PlayerName(), main_->LevelNum());


  if (main_->LevelNum() == 46) {  // send Dreamer's Cup entrance alerts
	LsPlayerList players;
	main_->PlayerSet()->GetPlayerList(players);
	for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
	    if ((*p)->PlayerID() != msg.PlayerID()) { // send alert message
//		    TLOG_Debug(_T("%s: sending cup summons for %s!"), method, msg.PlayerName());
			send_RMsg_CupSummons((*p)->Connection(), (*p)->PlayerID(), msg.PlayerName());
	    }
	}
  }

  // Send alert to house members when a visitor enters their house plane
  if (!alerts && !msg.Avatar().Hidden() && !msg.Hidden()) { // Don't send alert if NewlyAwakened msg already sent, or if GM INVIS or Mind Blanked
	  const int guildlevels[NUM_GUILDS] = { 25,22,24,26,21,17,23,18 };
	  for (int guild_id = 0; guild_id < NUM_GUILDS; guild_id++){
		  if ((main_->LevelNum() == guildlevels[guild_id]) && (msg.Avatar().GuildID() != guild_id)) {
			  LsPlayerList players;
			  main_->PlayerSet()->GetPlayerList(players);
			  alerts = true;

			  for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
				  if (((*p)->RoomID() == msg.RoomID()) && ((*p)->Avatar().GuildID() == guild_id)) {
					  alerts = false; // House member is already in entry room with crest displayed, no need to alert everyone
					  break;
				  }
			  }

			  if (alerts){
				  for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
					  if ((*p)->Avatar().GuildID() == guild_id)  // send alert message
						  send_RMsg_NewlyAwakened((*p)->Connection(), (*p)->PlayerID(), msg.PlayerName(), msg.RoomID());
				  }
			  }
		  }
	  }
  }

  SMsg_LS_Login msg2;
  msg2.Init(msg.PlayerID());
  if (LsUtil::SendInternalMessage(main_, msg2, LsMain::THREAD_ROOMSERVER) < 0) {
    TLOG_Error(_T("%s: could not send login message to room thread"), method);
    // TODO: abort?
  }
}

////
// handle_SMsg_Error
////

void LsLevelThread::handle_SMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_Error"));
  DEFMETHOD(LsLevelThread, handle_SMsg_Error);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Error, false); // don't send error
  // print message to log
  TLOG_Error(_T("%s: mtype=%d, error='%s'"), method, msg.MsgType(), msg.ErrorString());
}

////
// handle_SMsg_Login
////

void LsLevelThread::handle_SMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_Login);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_Login"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISUNKNOWN();
  // accept message
  ACCEPT_MSG(SMsg_Login, true); // send error
  // check that connection's peer IP is a server IP
  if (!main_->ServerDBC()->IsServerIP(conn->Socket().PeerName().IPAddress())) {
    TLOG_Warning(_T("%s: conn [%p] not a server; ip=%s"), method, conn, conn->Socket().PeerName().AddressString());
    return;
  }
  // process
  //  TLOG_Debug(_T("%s: logging in connection [%p] (%c,%u)"), method, conn, msg.ServerType(), msg.ID());
  // update connection type, message range
  main_->ConnectionSet()->UpdateConnection(conn, msg.ServerType(), msg.ID());
  conn->SetMessageRange(SMsg::MIN, SMsg::MAX);
}

////
// handle_SMsg_Logout
////

void LsLevelThread::handle_SMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_Logout);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  TLOG_Debug(_T("In handle_SMsg_Logout"));
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Logout, false); // don't send error
  // process
  //  TLOG_Debug(_T("%s: logging out connection [%p] (%c,%u)"), method, conn, conn->Type(), conn->ID());
  // if it's a gameserver, log out any players connected through it
  if (conn->Type() == LmConnection::CT_GSRV) {
    handle_SMsg_Logout_GSRV(conn);
  }
  // disconnect
  main_->ConnectionSet()->RemoveConnection(conn);
}

////
// handle_SMsg_Logout_GSRV: when a game server disconnects, fake logout messages
//   from any players that were connected through that game server
////

void LsLevelThread::handle_SMsg_Logout_GSRV(LmConnection* gsconn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_Logout_GSRV);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_Logout_GSRV"));
  // get players
  LsPlayerList players;
  main_->PlayerSet()->GetPlayerList(players);
  for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
    if ((*p)->Connection() != gsconn) { // not the same connection
      continue;
    }
    //    TLOG_Debug(_T("%s: logging out player %u"), method, (*p)->PlayerID());
    // create standard logout message
    RMsg_Logout msg_logout;
    msg_logout.InitLogout();
    // put it in a proxy message so the room thread thinks it came from the player
    SMsg_Proxy msg_proxy;
    msg_proxy.Init((*p)->PlayerID(), SMsg_Proxy::PROXY_PROCESS, msg_logout);
    // and then in a message buffer, so it can be dispatched
    LmSrvMesgBuf* buf_proxy = main_->BufferPool()->AllocateBuffer(msg_proxy.MessageSize());
    buf_proxy->ReadMessage(msg_proxy);
    // dispatch message as an internal message
    if (main_->InputDispatch()->DispatchMessage(buf_proxy, 0) < 0) {
      main_->BufferPool()->ReturnBuffer(buf_proxy);
      TLOG_Error(_T("%s: could not dispatch fake logout message!"), method);
      // TODO: abort?
    }
  }
}

////
// handle_SMsg_GetServerStatus
////

void LsLevelThread::handle_SMsg_GetServerStatus(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_GetServerStatus);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_GetServerStatus"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_GetServerStatus, true); // send error
  // check that connection is a monitor or game server
  if ((conn->Type() != LmConnection::CT_MONITOR) && (conn->Type() != LmConnection::CT_GSRV)) {
    TLOG_Warning(_T("%s: conn [%p] not a monitor/game server"), method, conn);
    return;
  }
  // process
  switch (msg.Status()) {
  case SMsg_GetServerStatus::STATUS_SERVER:
    send_SMsg_ServerStatus(conn);
    break;
  case SMsg_GetServerStatus::STATUS_PLAYER:
    send_SMsg_PlayerStatus(conn, msg.ID());
    break;
  case SMsg_GetServerStatus::STATUS_CONN:
    send_SMsg_ConnStatus(conn);
    break;
  default:
    TLOG_Warning(_T("%s: illegal request %d"), method, msg.Status());
    LsUtil::Send_SMsg_Error(main_, conn, msg.MessageType(), _T("unknown status request %d"), msg.Status());
    break;
  }
}


////
// handle_SMsg_DumpState
////

void LsLevelThread::handle_SMsg_DumpState(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_DumpState);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_DumpState"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_DumpState, true); // send error
  // check that connection is of type CT_MONITOR
  if (conn->Type() != LmConnection::CT_MONITOR) {
    TLOG_Warning(_T("%s: conn [%p] not a server monitor"), method, conn);
    return;
  }


  //TLOG_Log(_T("%s: dumping state"), method);
  // open up a dumpfile
  TCHAR df[80];
 _stprintf(df, _T("ls_%u_dump.%lu"), main_->LevelDBC()->LevelID(), time(NULL));
  TCHAR dfname[FILENAME_MAX];
  main_->GlobalDB()->GetDumpFile(dfname, df);
  FILE* dumpf =_tfopen(dfname, _T("w"));
  if (!dumpf) {
    TLOG_Error(_T("%s: could not open dump file '%s'"), method, dfname);
    return;
  }
  // dump server state to it
  main_->Dump(dumpf);
  fclose(dumpf);
  TLOG_Log(_T("%s: server state dumped to '%s'"), method, dfname);

}



////
// handle_SMsg_RotateLogs
////

void LsLevelThread::handle_SMsg_RotateLogs(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_RotateLogs);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_RotateLogs"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_RotateLogs, true); // send error
  // check that connection is of type CT_MONITOR
  if (conn->Type() != LmConnection::CT_MONITOR) {
    TLOG_Warning(_T("%s: conn [%p] not a server monitor"), method, conn);
    return;
  }

  //  TLOG_Log(_T("%s: rotating log"), method);
  close_log();
  open_log();
  // rotate main logs
  main_->RotateLogs();
}

////
// handle_SMsg_ResetPorts
////

void LsLevelThread::handle_SMsg_ResetPort(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_ResetPort);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_ResetPort"));
  HANDLER_ENTRY(false);

  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_ResetPort, true); // send error

  LsPlayer* player = main_->PlayerSet()->GetPlayer(msg.PlayerID());
  if (!player) {
    TLOG_Warning(_T("%s: got reset port message for player %u not in level"), method, msg.PlayerID());
    // nothing to send back
    return;
  }
   
  TLOG_Debug(_T("leveld reset peer update port for player %u to %d"), msg.PlayerID(), msg.Port());
  player->SetUpdateAddress((unsigned long)player->UpdateAddress().IPAddress(), msg.Port());
  return;
}

////
// handle_SMsg_LS_Action
////

void LsLevelThread::handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsLevelThread, handle_SMsg_LS_Action);
  DECLARE_TheLineNum;
  TLOG_Debug(_T("In LsLevelThread::handle_SMsg_LS_Action"));
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_LS_Action, false); // don't send error
  // switch on action to perform
  //  TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  switch (msg.Action()) {
  case SMsg_LS_Action::ACTION_EXIT:
  case SMsg_LS_Action::ACTION_RESTART: // deprecated
    TLOG_Debug(_T("In handle_SMsg_LS_Action, we were told to exit or restart"));
    handle_SMsg_LS_Action_Exit();
    break;
  case SMsg_LS_Action::ACTION_HEARTBEAT:
#ifdef WIN32
//    TLOG_Log(_T("%s: level thread running"), method);
#else
//    TLOG_Log(_T("%s: level thread running, sbrk=[%p]"), method, sbrk(0));
#endif
    break;
  case SMsg_LS_Action::ACTION_SAVE_STATE_FILE:
    // TLOG_Debug(_T("%s: saving level state to disk"), method);
	main_->LevelState()->SaveToDisk();
	saved_to_disk_ = true;
    // TLOG_Debug(_T("%s: level state saved"), method);
    break;
  case SMsg_LS_Action::ACTION_SAVE_STATE_DB:
    // TLOG_Debug(_T("%s: saving level state to database"), method);
	  if (main_->LevelState()->SaveToDB() < 0) {
		TLOG_Error(_T("%s: could not save level state to db; trying disk"), method);
		if (0 == main_->LevelState()->SaveToDisk())
			saved_to_disk_ = true;
	  } else {
		if (saved_to_disk_)
		{ // if we saved to disk before, remove state file now
			TCHAR statefile[FILENAME_MAX];
			main_->GlobalDB()->GetLevelState(statefile, dbc_->LevelID());
			_tunlink(statefile);
			TLOG_Debug(_T("%s: level state saved to db; disk file removed"), method);
		}
		saved_to_disk_ = false;
	  }
    break;
  case SMsg_LS_Action::ACTION_READCODEX:
    main_->CodexText()->Load();
    break;
  case SMsg_LS_Action::ACTION_FREEBUFS: {
    int oldbufs = main_->BufferPool()->FreeOldBuffers();
    // TLOG_Debug(_T("%s: freed %d old buffers"), method, oldbufs);
  }
  break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}


////
// handle_SMsg_LS_Action_Exit: gracefully shut down server
////

void LsLevelThread::handle_SMsg_LS_Action_Exit()
{
  DEFMETHOD(LsLevelThread, handle_SMsg_LS_Action_Exit);
  DECLARE_TheLineNum;
  TLOG_Log(_T("%s: shutting down threads"), method);

  // initialize exit message
  SMsg_LS_Action msg;
  msg.Init(SMsg_LS_Action::ACTION_EXIT);
  // send to all server threads
  LsUtil::BroadcastInternalMessage(main_, msg, LsMain::THREAD_LEVELSERVER);
  // need to send a signal to the signal thread to wake it up, otherwise it
  // may not get the message.  use SIGUSR2
  //sigsend(P_PID, P_MYID, SIGUSR2);
  // wait a few seconds
#ifdef WIN32
  Sleep(5000);
#else
  st_sleep(5);
#endif

  if (main_->LevelState()->SaveToDB() < 0) {
  	// save level state (to disk and to database)
		main_->LevelState()->SaveToDisk();
  } else {
	TCHAR statefile[FILENAME_MAX];
	main_->GlobalDB()->GetLevelState(statefile, dbc_->LevelID());
	_tunlink(statefile);
  }

  // then exit ourselves
  SetDone();
}

////
// send_RMsg_LevelLoginAck
////

void LsLevelThread::send_RMsg_LevelLoginAck(LmConnection* conn, lyra_id_t playerid, int status, lyra_id_t roomid)
{
  DECLARE_TheLineNum;
  RMsg_LoginAck msg;
  TLOG_Debug( _T( "RMsg_LoginAck maxSize is %d" ), msg.MaxMessageSize() );
  msg.Init(status, roomid, main_->LevelDBC()->LevelID());
  LsUtil::Send_SMsg_Proxy(main_, conn, playerid, msg);
}

void LsLevelThread::send_RMsg_LevelLoginAck(LmConnection* conn, lyra_id_t playerid, int status, lyra_id_t roomid,
				       unsigned long server_ip, int server_port)
{
  DECLARE_TheLineNum;
  RMsg_LoginAck msg;
  TLOG_Debug( _T( "RMsg_LoginAck maxSize is %d" ), msg.MaxMessageSize() );
  msg.Init(status, roomid, main_->LevelDBC()->LevelID(), server_ip, server_port);
  LsUtil::Send_SMsg_Proxy(main_, conn, playerid, msg);
}

void LsLevelThread::send_RMsg_NewlyAwakened(LmConnection* conn, lyra_id_t playerid, 
	  const TCHAR* newly_name, lyra_id_t roomid)
{
  DECLARE_TheLineNum;
  RMsg_NewlyAwakened msg;
  msg.Init();
  msg.SetPlayerName(newly_name);
  msg.SetLocation(roomid);
  LsUtil::Send_SMsg_Proxy(main_, conn, playerid, msg);
}

void LsLevelThread::send_RMsg_CupSummons(LmConnection* conn, lyra_id_t playerid, 
	  const TCHAR* name)
{
  DECLARE_TheLineNum;
  RMsg_CupSummons msg;
  msg.Init();
  msg.SetPlayerName(name);
  LsUtil::Send_SMsg_Proxy(main_, conn, playerid, msg);
}


////
// send_SMsg_ServerStatus
////

void LsLevelThread::send_SMsg_ServerStatus(LmConnection* conn)
{
  TLOG_Debug(_T("In LsLevelThread::send_SMsg_ServerStatus"));
  SMsg_ServerStatus msg;
  // get playerlist
  LsPlayerList plist;
  main_->PlayerSet()->GetPlayerList(plist);
  // initialize message
  msg.Init(main_->Uptime(), main_->PlayerSet()->NumLogins(), plist.size(),
           main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_PLAYER),
           main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_MONSTER),
           main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_ADMIN),
           main_->PlayerSet()->MaxPlayers(), main_->ConnectionSet()->NumConnections(),
           main_->ConnectionSet()->MaxConnections(), main_->ServerPid(), main_->ParentPid(), LmUtil::GetCPULoad());
  // copy playerids
  int pi = 0;
  for (LsPlayerList::iterator i = plist.begin(); !(bool)(i == plist.end()); ++i) {
    msg.SetPlayerID(pi, (*i)->PlayerID());
    ++pi;
  }
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_SMsg_ConnStatus
////

void LsLevelThread::send_SMsg_ConnStatus(LmConnection* conn)
{
  TLOG_Debug(_T("In LsLevelThread::send_SMsg_ConnStatus"));
  DEFMETHOD(LsLevelThread, send_SMsg_ConnStatus);
  SMsg_ConnStatus msg;
  LmConnectionList conn_list;
  main_->ConnectionSet()->GetConnectionList(conn_list);
  msg.Init(conn_list.size());
  int i = 0;
  for (LmConnectionList::iterator c = conn_list.begin(); !(bool)(c == conn_list.end()); ++c, ++i) {
    LmConnection* conn2 = *c;
    msg.InitConnection(i, conn2);
  }
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_SMsg_PlayerStatus
////

void LsLevelThread::send_SMsg_PlayerStatus(LmConnection* conn, lyra_id_t playerid)
{
  TLOG_Debug(_T("In LsLevelThread::send_SMsg_PlayerStatus"));
  SMsg_PlayerStatus msg;
  // get player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (player) {
    msg.Init(player->PlayerID(), player->PlayerName(), main_->LevelDBC()->LevelID(), player->RoomID(),
             player->PlayerUpdate().X(), player->PlayerUpdate().Y(), player->AccountType(),
	     player->Online(), player->IPAddress(), player->IdleTime());
  }
  else {
    msg.Init(playerid, _T("(unknown)"), 0, 0, 0, 0, 'U', 0, INADDR_ANY, 0);
  }
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

