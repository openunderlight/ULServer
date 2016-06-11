// GsGameThread.cpp  -*- C++ -*-
// $Id: GsGameThread.cpp,v 1.68 1998/04/18 00:40:23 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsGameThread.h"
#endif

#include <stdio.h>
#include <signal.h> 
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "GsGameThread.h"
#include "GsMain.h"
#include "LmConnection.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmDebug.h"
#include "GMsg_All.h"
#include "SMsg_All.h"
#include "GsPlayer.h"
#include "GsPlayerSet.h"
#include "GsOutputDispatch.h"
#include "LmMesg.h"
#include "LmThreadPool.h"
#include "LmSrvMesgBuf.h"
#include "LmMesgBufPool.h"
#include "GsUtil.h"
#include "LmConnectionSet.h"
#include "GsPlayerThread.h"
#include "RMsg.h"
#include "RMsg_LoginAck.h"
#include "RMsg_Error.h"
#include "GsMacros.h"
#include "GsPlayerList.h"
#include "LmConnectionList.h"
#include "GsPlayerThreadSet.h"
#include "LmPlayerNameMap.h"
#include "LmLogFile.h"
#include "LmServerDBC.h"
#include "LmPlayerDBC.h"
#include "LmBillingDBC.h"
#include "LmRand.h"
#include "LmThread.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef USE_INLINE
#include "GsGameThread.i"
#endif


DECLARE_TheFileName;


////
// Constructor
////

GsGameThread::GsGameThread(GsMain* gsmain)
  : LmThread(gsmain->BufferPool(), gsmain->Log() /* &logf_ */ ),
    main_(gsmain)
{
  DECLARE_TheLineNum;
  open_log();
  register_handlers();
}

////
// Destructor
////

GsGameThread::~GsGameThread()
{
  DECLARE_TheLineNum;
  close_log();
}

////
// Run
////

void GsGameThread::Run()
{
  LmUtil::BlockAllSignals();
  //TLOG_Debug(_T("Game thread pid = %d"), getpid());
  DECLARE_TheLineNum;
  LmThread::Run();
}

////
// Dump
////

void GsGameThread::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  INDENT(indent, f);
 _ftprintf(f, _T("<GsGameThread[%p,%d]: main=%p>\n"), this, sizeof(GsGameThread), main_);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void GsGameThread::open_log()
{
  // logf_.Init("gs", "game", main_->ServerPort());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void GsGameThread::close_log()
{
  // logf_.Close();
}

////
// start_player_thread
////

GsPlayerThread* GsGameThread::start_player_thread(GsPlayer* player)
{
  DEFMETHOD(GsGameThread, start_player_thread);
  DECLARE_TheLineNum;
  // start or get inactive player thread
  lyra_id_t playerid = player->DB().PlayerID();
  GsPlayerThread* pthr = main_->PlayerThreadSet()->GetPlayerThread();
  if (!pthr) {
    TLOG_Error(_T("%s: could not allocate thread for player %u"), method, playerid);
    return 0;
  }
  // add player thread to thread pool, with player's id
  main_->ThreadPool()->AddThread(playerid, pthr);
  //TLOG_Debug(_T("%s: player %u allocated thread [%p], pthread %lu"), method, playerid, pthr, pthr->Thread());
  return pthr;
}

////
// register_handlers
////


void GsGameThread::register_handlers()
{
  DECLARE_TheLineNum;
  // default message handler
  SetDefaultHandler((MsgHandler)&GsGameThread::handle_Default);
  // register GMsg message handlers
  RegisterHandler(GMsg::SERVERERROR, (MsgHandler)&GsGameThread::handle_GMsg_Error);
  RegisterHandler(GMsg::LOGIN, (MsgHandler)&GsGameThread::handle_GMsg_Login);
  RegisterHandler(GMsg::PRELOGIN, (MsgHandler)&GsGameThread::handle_GMsg_PreLogin);
  RegisterHandler(GMsg::AGENTLOGIN, (MsgHandler)&GsGameThread::handle_GMsg_AgentLogin);
  //  registerHandler(GMsg::CREATEPLAYER, (MsgHandler)&GsGameThread::handle_GMsg_CreatePlayer);
  RegisterHandler(GMsg::PING, (MsgHandler)&GsGameThread::handle_GMsg_Ping);
  // register RMsg message handlers
  RegisterHandler(RMsg::SERVERERROR, (MsgHandler)&GsGameThread::handle_RMsg_Error);
  // register SMsg_GS message handlers
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsGameThread::handle_SMsg_GS_Action);
  RegisterHandler(SMsg::GS_LOGOUT, (MsgHandler)&GsGameThread::handle_SMsg_GS_Logout);
  // register SMsg message handlers
  RegisterHandler(SMsg::PING, (MsgHandler)&GsGameThread::handle_SMsg_Ping);
  RegisterHandler(SMsg::LOGOUT, (MsgHandler)&GsGameThread::handle_SMsg_Logout);
  RegisterHandler(SMsg::LOGIN, (MsgHandler)&GsGameThread::handle_SMsg_Login);
  RegisterHandler(SMsg::GETSERVERSTATUS, (MsgHandler)&GsGameThread::handle_SMsg_GetServerStatus);
  RegisterHandler(SMsg::DUMP_STATE, (MsgHandler)&GsGameThread::handle_SMsg_DumpState);
  RegisterHandler(SMsg::ROTATE_LOGS, (MsgHandler)&GsGameThread::handle_SMsg_RotateLogs);
  RegisterHandler(SMsg::SERVERERROR, (MsgHandler)&GsGameThread::handle_SMsg_Error);
}


////
// handle_Default
////

void GsGameThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, GsGameThread::handle_Default);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_GMsg_Error
////

void GsGameThread::handle_GMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_GMsg_Error);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(GMsg_Error, false); // don't send error
  // print error to log
  TLOG_Error(_T("%s: mtype=%d, error='%s'"), method, msg.MsgType(), msg.ErrorString());
}

////
// handle_GMsg_Ping
////

void GsGameThread::handle_GMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_GMsg_Ping);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(GMsg_Ping, true); // send error
  // send ping right back
  main_->OutputDispatch()->SendMessage(&msg, conn);
}


////
// handle_GMsgPre_Login
////


void GsGameThread::handle_GMsg_PreLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_GMsg_PreLogin);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISUNKNOWN();
  // accept message
  ACCEPT_MSG(GMsg_PreLogin, true); // send error

  TCHAR challenge[Lyra::CHALLENGE_LENGTH];

  memset((void*)challenge, 0, sizeof(challenge));

  // challenge consists of: time-ip addr-port-pid-ppid-uptime--version-num logins-rand-rand-rand  
  _stprintf(challenge, _T("%u%s%d%d%d%u%u%u%d%d%d"), 
	    time(NULL), main_->HostIP(), main_->ServerPort(), main_->ServerPid(), main_->ParentPid(), 
            main_->Uptime(), GsMain::GAME_VERSION, main_->NumLogins(), rand(), rand(), rand());

  conn->SetChallenge(challenge);
  conn->SetChallengeTime();

  // check if IP is "0.0.0.0", and don't continue
  if (_tcscmp(_T("0.0.0.0"), conn->Socket().PeerName().AddressString()) == 0) {
    TLOG_Warning(_T("%s: connection from 0.0.0.0"), method);
    send_GMsg_PreLoginAck(conn, GMsg_LoginAck::LOGIN_UNKNOWNERROR, GsMain::GAME_VERSION, challenge);
    return;
  }

  //SECLOG(-1, _T("%s: player being challenged, logging in from %s:%d"), method,
	// conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());

  //send_GMsg_PreLoginAck(conn, GMsg_PreLoginAck::GAMEFULL);
  
  send_GMsg_PreLoginAck(conn, GMsg_PreLoginAck::PRELOGIN_OK, GsMain::GAME_VERSION, challenge);
}

////
// handle_GMsg_Login
////


void GsGameThread::handle_GMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_GMsg_Login);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISUNKNOWN();
  // accept message
  ACCEPT_MSG(GMsg_Login, true); // send error

  short session_minutes = 0; // # of minutes spent in the pmare session today
  short max_minutes_online = 0; // set for pmares with CASH accounts
  lyra_id_t gamesite_id = 0;
  unsigned char gamesite = GMsg_LoginAck::GAMESITE_LYRA;
 
  // get connection time for later use
  time_t conn_time = conn->ConnectTime();

  // check if IP is "0.0.0.0", and don't continue
  if (_tcscmp(_T("0.0.0.0"), conn->Socket().PeerName().AddressString()) == 0) {
    TLOG_Warning(_T("%s: connection from 0.0.0.0; player '%s'"), method, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    return;
  }

  // log connection
  //  SECLOG(-1, _T("%s: player 0: '%s' connecting from %s:%d"), method,
  //	 msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());

  // determine if GM build
  bool gm_build = false;
  unsigned int version = (unsigned) msg.Version();
  if (version > Lyra::GM_DELTA) {
    gm_build = true;
    version -= Lyra::GM_DELTA;
  }

  int pmare_type = 0;
  bool pmare_build = false;
  // determine if PMare build
  if ((version < Lyra::GM_DELTA) && 
      (version > Lyra::PMARE_DELTA)) {
    pmare_build = true;
    pmare_type = msg.PMareType();
    version -= Lyra::PMARE_DELTA;
  }

//	gm_build = false; // use for debugging billing stuff

  // check version
  if (version != GsMain::GAME_VERSION) {
    SECLOG(1, _T("%s: player 0: player '%s' using version %d, not %d"), method,
	   msg.PlayerName(), version, GsMain::GAME_VERSION);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_WRONGVERSION);
    return;
  }

  // check sub-version
  if (msg.SubVersion() != GsMain::GAME_SUBVERSION) {
    SECLOG(1, _T("%s: player 0: player '%s' subversion mismatch: %d, not %d"), method,
	   msg.PlayerName(), msg.SubVersion(), GsMain::GAME_SUBVERSION);
    // don't return, let the player assume they weren't detected
  }

  // check that game server isn't full
  //  SECLOG(1, _T("%s: curr players = %d, max = %d"), method, main_->PlayerSet()->NumPlayers(), main_->PlayerSet()->MaxPlayers());

  // uncomment these lines to force a pathalogical server full condition
	//send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_GAMEFULL, 	main_->NextIP(), main_->NextPort());
    //return;

  if ((main_->Closed()) || 
	  (main_->PlayerSet()->NumPlayers() >= main_->PlayerSet()->MaxPlayers())) {
	  send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_GAMEFULL, 
		main_->NextIP(), main_->NextPort());
    return;
  }

  // check that player name is non-null
  if (_tcslen(msg.PlayerName()) == 0) {
    SECLOG(1, _T("%s: player 0: connection from %s using null player name"), method,
	   conn->Socket().PeerName().AddressString());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    return;
  }

  int rc, sc, lt; // db return codes
  // find playerid from name, and get "real" name
  TCHAR player_name[Lyra::PLAYERNAME_MAX];
  rc = main_->PlayerDBC()->GetPlayerID(msg.PlayerName(), player_name);
  sc = main_->PlayerDBC()->LastSQLCode();
  lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::GetPlayerID took %d ms"), method, lt);
  if (rc <= 0) {
    SECLOG(1, _T("%s: player 0: could not find id for player '%s'"), method, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    return;
  }
  lyra_id_t playerid = rc;
  // put into the name map
  main_->PlayerNameMap()->AddMapping(playerid, player_name);

  // check that player isn't already in this game server
  if (main_->PlayerSet()->IsInGame(playerid)) {
    SECLOG(1, _T("%s: player %u: %s already logged in"), method, playerid, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_ALREADYIN);
    return;
  }

  // check password 


  if (!main_->PlayerDBC()->CheckPassword(playerid, msg.HashPtr(), conn->Challenge())) {
    SECLOG(1, _T("%s: player %u: %s gave incorrect hash"), method, playerid, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_BADPASSWORD);
    return;
  }    

  // check that this account can log in to the player db
  int suspended_days = 0;
  rc = main_->PlayerDBC()->CanLogin(playerid, &suspended_days, pmare_type); 
  sc = main_->PlayerDBC()->LastSQLCode();
  lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::CanLogin took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: error from CanLogin: rc=%d, sqlcode=%d"), method, rc, sc);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    return;
  }
  else if (rc != GMsg_LoginAck::LOGIN_OK) {
    SECLOG(1, _T("%s: player %u: locked out / killed / suspended/ billing_id not matched; reason = %d"), method, playerid, rc);
    send_GMsg_LoginAck(conn, conn_time, rc, suspended_days);
    return;
  }

  // allocate player in player set
  GsPlayer* player = main_->PlayerSet()->AllocatePlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: could not allocate player for player %u"), method, playerid);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_GAMEFULL, 
		main_->NextIP(), main_->NextPort());
    return;
  }

  // load player database
  player->Init(conn, msg.ServerPort(), Log(), true, msg.TCPOnly());
  TLOG_Debug("%s: initialized player %d, firewall is true, TCPOnly is %d", method, playerid, msg.TCPOnly());
  if (player->Login(playerid, pmare_type) < 0) {
    TLOG_Error(_T("%s: could not load database for player %u"), method, playerid);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }

  // uncomment to force all build security checks at login
//#define RELEASE
  // check account type, and build type
  switch (player->DB().AccountType()) {
  case LmPlayerDB::ACCT_PMARE: // player mare or
#ifdef RELEASE
    // must use pmare build
    if (!pmare_build) {
      SECLOG(1, _T("%s: player %u: Non-pmare build used with pmare account"), method, playerid);
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
      main_->PlayerSet()->RemovePlayer(player, false);
      return;
    }
#endif
    // IMPORTANT: pmare code falls through, pmares use player filters 

  case LmPlayerDB::ACCT_PLAYER: // regular player
#ifdef RELEASE
    // cannot use GM build in release
    if (gm_build) {
      SECLOG(1, _T("%s: player %u: GM build used with non-GM account"), method, playerid);
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
      main_->PlayerSet()->RemovePlayer(player, false);
      return;
    }
#endif
#ifdef RELEASE // check result 
    // check that this account has not been disabled by the billing
    // system - but ONLY for release builds
    rc = main_->BillingDBC()->GetBillingStatus(playerid, player->DB().AccountType(), 
		player->DB().PMareBilling(), &max_minutes_online, &session_minutes,
		&gamesite, &gamesite_id); 
    //      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_NO_PMARE);
    sc = main_->PlayerDBC()->LastSQLCode();
    lt = main_->PlayerDBC()->LastCallTime();
    //  main_->Log()->Debug(_T("%s: LmPlayerDBC::CanLogin took %d ms"), method, lt);
    if (rc == -1) {
      TLOG_Warning(_T("%s: database error from GetBillingStatus: rc=%d, sqlcode=%d"), method, rc, sc);
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
      main_->PlayerSet()->RemovePlayer(player, false);
      GsUtil::HandlePlayerError(main_, method, rc, sc, false);
      return;
    }
    else if (rc != GMsg_LoginAck::LOGIN_OK) {
      SECLOG(1, _T("%s: player %u: account disabled by billing system; reason = %d"), method, playerid, rc);
      send_GMsg_LoginAck(conn, conn_time, rc);
      main_->PlayerSet()->RemovePlayer(player, false);
      return;
    }

#endif
    break;
  
  case LmPlayerDB::ACCT_ADMIN: // GM
    // must use GM build
    if (!gm_build) {
      SECLOG(1, _T("%s: player %u: non-GM build used with GM account"), method, playerid);
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
      main_->PlayerSet()->RemovePlayer(player, false);
      return;
    }
    break;
  case LmPlayerDB::ACCT_MONSTER: // agents, not allowed
  default: // all others -- unknown
    SECLOG(1, _T("%s: player %u: illegal account type %c"), method, playerid, player->DB().AccountType());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
    main_->PlayerSet()->RemovePlayer(player, false);
    break;
}

  // if connection time differs from stored value, then it is possible that the connection was actually
  // lost and re-used during this login processing -- in this case, we do not wish to continue
  if ((conn->ConnectTime() != conn_time) || !conn->IsConnected()) {
    TLOG_Warning(_T("%s: conn [%p] connection times differ!"), method, conn);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }

  // update connection type, message range
  main_->ConnectionSet()->UpdateConnection(conn, LmConnection::CT_CLIENT, playerid);
  // rely on the fact that RMsg's come before GMsg's
  conn->SetMessageRange(RMsg::MIN, GMsg::MAX);
  // create/start player thread

  GsPlayerThread* pthr = start_player_thread(player);
  if (!pthr) {
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }
  // send login message to thread
  SMsg_GS_Login msg_login;
  msg_login.Init(playerid);
  GsUtil::SendInternalMessage(main_, msg_login, pthr);
  // send successful loginack to client
  SECLOG(-1, _T("%s: player %u: %s logged in from %s:%d"), method,
	 playerid, msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());
  send_GMsg_LoginAck(conn, player, max_minutes_online, session_minutes, gamesite, gamesite_id);
  main_->SetNumLogins(main_->NumLogins() + 1);

  // closing mechanism disabled as unnecessary

  //if (main_->PlayerSet()->NumLogins() >= GsMain::MAX_LOGINS) { 
  //if (main_->PlayerSet()->NumLogins() > GsMain::CLOSING_THRESHHOLD) {
  //if (main_->PlayerSet()->NumLogins() >= 4) {
//	  int roll = LmRand::Generate(1, 100);
//	  if (roll < (main_->PlayerSet()->NumLogins()))
//	  if (roll < (main_->PlayerSet()->NumLogins() - GsMain::CLOSING_THRESHHOLD));
//		main_->TryToClose();
  //}
}

////
// handle_GMsg_AgentLogin - handle monster logins, and GM possession logins
////

void GsGameThread::handle_GMsg_AgentLogin(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_GMsg_AgentLogin);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISUNKNOWN();
  // accept message
  ACCEPT_MSG(GMsg_AgentLogin, true); // send error

  // store connection time for later use
  time_t conn_time = conn->ConnectTime();

  // check if IP is "0.0.0.0", and don't continue
  if (_tcscmp(_T("0.0.0.0"), conn->Socket().PeerName().AddressString()) == 0) {
    TLOG_Warning(_T("%s: connection from 0.0.0.0; player '%s'"), method, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    return;
  }

  // log connection
  SECLOG(-1, _T("%s: player 0: '%s' connecting from %s:%d"), method,
	 msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());

  // determine if GM build
  bool gm_build = false;
  unsigned int version = (unsigned) msg.Version();
  if (version > Lyra::GM_DELTA) {
    gm_build = true;
    version -= Lyra::GM_DELTA;
  }

  // check version (no sub-version in this message)
  if (version != GsMain::GAME_VERSION) {
    SECLOG(1, _T("%s: player 0: player '%s' using version %d, not %d"), method,
	   msg.PlayerName(), version, GsMain::GAME_VERSION);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_WRONGVERSION);
    return;
  }

  // check that player name is non-null
  if (_tcslen(msg.PlayerName()) == 0) {
    SECLOG(1, _T("%s: player 0: connection from %s using null player name"), method,
	   conn->Socket().PeerName().AddressString());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    return;
  }

  // check that game server isn't _really_ full
  if (main_->PlayerSet()->NumPlayers() >= main_->PlayerSet()->MaxPlayers()) {
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_GAMEFULL, 
		main_->NextIP(), main_->NextPort());
    return;
  }

  int rc, sc; //, lt; // db return codes
  lyra_id_t billing_id = msg.BillingID(); // this is the id of the admin, not the monster

  // find playerid from name, and get "real" name
  TCHAR player_name[Lyra::PLAYERNAME_MAX];
  rc = main_->PlayerDBC()->GetPlayerID(msg.PlayerName(), player_name);
  sc = main_->PlayerDBC()->LastSQLCode();
  //  lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::GetPlayerID took %d ms"), method, lt);
  if (rc < 0) {
    SECLOG(1, _T("%s: player 0: could not find id for player '%s'"), method, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    return;
  }
  lyra_id_t playerid = rc;
  // put into the name map
  main_->PlayerNameMap()->AddMapping(playerid, player_name);

  // check that player isn't already in this game server
  if (main_->PlayerSet()->IsInGame(playerid)) {
    SECLOG(1, _T("%s: player %u: %s already logged in"), method, playerid, msg.PlayerName());
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_ALREADYIN);
    return;
  }

  // if billing_id = 0, check that connection is from an agent server (actually any server, but it will do)
  if (billing_id == 0) {
    if (!main_->ServerDBC()->IsServerIP(conn->Socket().PeerName().IPAddress())) {
      SECLOG(1, _T("%s: player 0: illegal non-server agent login (player '%s', IP %s:%d)"), method,
	     msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
      return;
    }
  }
  else { // billing_id != 0, must be a GM build, and billing_id must be that of a GM
    if (!gm_build) {
      SECLOG(1, _T("%s: player 0: illegal agent possession by mplayer %u (player '%s', IP %s:%d)"), method,
	     billing_id, msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_MISMATCH);
      return;
    }
    // log and check later
    SECLOG(-1, _T("%s: player 0: agent '%s' being possessed by mplayer %u"), method, msg.PlayerName(), billing_id);
  }

  // check password (always, since this is for agents)
  //
  if (!main_->PlayerDBC()->CheckPassword(playerid, msg.HashPtr(), conn->Challenge())) {
      SECLOG(1, _T("%s: nightmare %u: %s gave incorrect password"), method, playerid, msg.PlayerName());
      send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_BADPASSWORD);
      return;
  }    

  // allocate player in player set
  GsPlayer* player = main_->PlayerSet()->AllocatePlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: could not allocate player for player %u"), method, playerid);
#if 0 // deprecated BMP 10/03 - round robin only
	main_->TryToClose();
#endif
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_GAMEFULL, 
		main_->NextIP(), main_->NextPort());
    return;
  }
  // load player database
  player->Init(conn, msg.ServerPort(), Log(), false, false); // mare server shouldn't be behind a firewall
  if (player->Login(playerid, 0) < 0) {
    TLOG_Error(_T("%s: could not load database for player %u"), method, playerid);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }

  // check that player type is agent
  if (player->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) {
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_USERNOTFOUND);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }

  // if connection time differs from stored value, then it is possible that the connection was actually
  // lost and re-used during this login processing -- in this case, we do not wish to continue
  if ((conn->ConnectTime() != conn_time) || !conn->IsConnected()) {
    TLOG_Warning(_T("%s: conn [%p] connection times differ!"), method, conn);
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }

  // set player description (agentlogin doesn't have a description field)
  if (msg.BillingID() < 600){
	((class LmPlayerDB&)player->DB()).SetAvatarDescrip(msg.PlayerName());
  } else {
	((class LmPlayerDB&)player->DB()).SetAvatarDescrip("Revenant");
  }
  // update connection type, message range
  main_->ConnectionSet()->UpdateConnection(conn, LmConnection::CT_CLIENT, playerid);
  // rely on the fact that RMsg's come before GMsg's
  conn->SetMessageRange(RMsg::MIN, GMsg::MAX);
  // create/start player thread
  GsPlayerThread* pthr = start_player_thread(player);
  if (!pthr) {
    send_GMsg_LoginAck(conn, conn_time, GMsg_LoginAck::LOGIN_UNKNOWNERROR);
    main_->PlayerSet()->RemovePlayer(player, false);
    return;
  }
  // send login message to thread
  SMsg_GS_Login msg_login;
  msg_login.Init(playerid);
  GsUtil::SendInternalMessage(main_, msg_login, pthr);
  // send successful loginack to client
  SECLOG(-1, _T("%s: player %u: %s logged in from %s:%d"), method,
	 playerid, msg.PlayerName(), conn->Socket().PeerName().AddressString(), conn->Socket().PeerName().Port());
  send_GMsg_LoginAck(conn, player, 0, 0);
  // NOTE: agent logins do not count towards the max limit for closing, 
  // because for some reason agents do not cause memory leaks in gamed
}

////
// handle_RMsg_Error
////

void GsGameThread::handle_RMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_RMsg_Error);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(RMsg_Error, false); // don't send error
  // print error to log
  TLOG_Error(_T("%s: mtype=%d, error='%s'"), method, msg.MsgType(), msg.ErrorString());
}

////
// handle_SMsg_Error
////

void GsGameThread::handle_SMsg_Error(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_Error);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Error, false); // don't send error
  // print error to log
  TLOG_Error(_T("%s: mtype=%d, error='%s'"), method, msg.MsgType(), msg.ErrorString());
}

////
// handle_SMsg_Login
////

void GsGameThread::handle_SMsg_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_Login);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Login, true); // send error
  // check that connection's peer IP is a server IP
  if (!main_->ServerDBC()->IsServerIP(conn->Socket().PeerName().IPAddress())) {
    TLOG_Warning(_T("%s: conn [%p] not a server; ip=%s"), method, conn, conn->Socket().PeerName().AddressString());
    return;
  }
  // change connection type, range
  main_->ConnectionSet()->UpdateConnection(conn, msg.ServerType(), msg.ID());
  conn->SetMessageRange(SMsg::MIN, SMsg::MAX);
}


////
// handle_SMsg_GetServerStatus
////

void GsGameThread::handle_SMsg_GetServerStatus(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_GetServerStatus);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_GetServerStatus, true); // send error
  // check that connection is of type CT_MONITOR
  if (conn->Type() != LmConnection::CT_MONITOR) {
    TLOG_Warning(_T("%s: conn [%p] not a server monitor"), method, conn);
    return;
  }
  // check request
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
    TLOG_Warning(_T("%s: unknown status request %d"), method, msg.Status());
    GsUtil::Send_SMsg_Error(main_, conn, msg.MessageType(), _T("unknown status request %d"), msg.Status());
    break;
  }
}


////
// handle_SMsg_DumpState
////

void GsGameThread::handle_SMsg_DumpState(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_DumpState);
  DECLARE_TheLineNum;
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
  TCHAR df[80];
 _stprintf(df, _T("gs_%u_dump.%lu"), main_->ServerPort(), time(NULL));
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

void GsGameThread::handle_SMsg_RotateLogs(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_RotateLogs);
  DECLARE_TheLineNum;
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
// handle_SMsg_Ping
////

void GsGameThread::handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_Ping);
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
        //TLOG_Debug(_T("%s: ping (%d) from conn [%p] (%c,%d)"), method, msg.Nonce(), conn, conn->Type(), conn->ID());
    // return a pong
    msg.InitPong(msg.Nonce());
    main_->OutputDispatch()->SendMessage(&msg, conn);
  }
  else if (msg.PingType() == SMsg_Ping::PONG) {
    int dt = time(NULL) - msg.Nonce();
        //TLOG_Debug(_T("%s: pong (%d) from conn [%p] (%c,%d); dt=%d"), method, msg.Nonce(), conn, conn->Type(), conn->ID(), dt);
  }
  else {
    TLOG_Warning(_T("%s: unknown ping %d from conn [%p] (%c,%d)"), method, msg.PingType(), conn, conn->Type(), conn->ID());
  }
}

////
// handle_SMsg_Logout
////

void GsGameThread::handle_SMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_Logout);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Logout, false); // don't send error
  // process
  //TLOG_Debug(_T("%s: logging out conn [%p] (%c,%u)"), method, conn, conn->Type(), conn->ID());
  // if connection is a level server, notify any players in the level
  if (conn->Type() == LmConnection::CT_LSRV) {
    GsPlayerList players;
    main_->PlayerSet()->GetPlayerList(players);
    for (GsPlayerList::iterator i = players.begin(); !(bool)(i == players.end()); ++i) {
      GsPlayer* player = *i;
      if ((player->LevelConnection() == conn) && (player->InLevel())) {
	// send loginack message
	//TLOG_Debug(_T("%s: player %u being logged out of level %d"), method, player->PlayerID(), conn->ID());
	GsUtil::Send_RMsg_LevelLoginAck(main_, player->Connection(), RMsg_LoginAck::LOGIN_SERVERDOWN,
				   player->LevelID(), player->RoomID());
	player->SetInLevel(false);
      }
    }
	  /// disconnect, remove from set
	main_->ConnectionSet()->RemoveConnection(conn);
  } else {
		main_->ConnectionSet()->RemoveConnection(conn); //*****
//		conn->Disable();
  }
  // TODO: check if this connection was associated with any players, and log them out if so?
}

////
// handle_SMsg_GS_Logout
////

void GsGameThread::handle_SMsg_GS_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_GS_Logout);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Logout, false); // don't send error
  // process
  // SECLOG(-1, _T("%s: player %u: logging out of game, status %c, %d seconds online"), method,
  // msg.PlayerID(), msg.Status(), msg.Online());
}

////
// handle_SMsg_GS_Action
////

void GsGameThread::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsGameThread, handle_SMsg_GS_Action);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Action, false); // don't send error
  // switch on action to perform
  switch (msg.Action()) {
  case SMsg_GS_Action::ACTION_EXIT:
    TLOG_Debug(_T("%s: got message to exit"), method);
  case SMsg_GS_Action::ACTION_RESTART: // deprecated
    handle_SMsg_GS_Action_Exit();
    break;
  case SMsg_GS_Action::ACTION_FREEBUFS: {
    int oldbufs = main_->BufferPool()->FreeOldBuffers();
    // TLOG_Debug(_T("%s: freed %d old buffers"), method, oldbufs);
  }
  break;
  case SMsg_GS_Action::ACTION_HEARTBEAT:
#ifdef WIN32
    TLOG_Log(_T("%s: game thread running"), method);
#else
    TLOG_Log(_T("%s: game thread running, brk=[%p]"), method, sbrk(0));
#endif
    // also write some stats to the security log
    SECLOG(-9, _T("%s: players=%d monsters=%d admins=%d pmares=%d cpu=%.3lf"), method, 
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_PLAYER),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_MONSTER),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_ADMIN),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_PMARE),
	   LmUtil::GetCPULoad());
    break;
  default:
    TLOG_Error(_T("%s: illegal action code '%c' (%d)"), method, msg.Action(), msg.Action());
    break;
  }
}

////
// handle_SMsg_GS_Action_Exit
////

void GsGameThread::handle_SMsg_GS_Action_Exit()
{
  DEFMETHOD(GsGameThread, handle_SMsg_GS_Action_Exit);
  DECLARE_TheLineNum;
  TLOG_Log(_T("%s: shutting down threads"), method);
  // send "server down" message to clients
  send_GMsg_ServerDown(GMsg_ServerDown::STATUS_SHUTDOWN);
#ifdef WIN32
  Sleep(5000);
#else
  pth_sleep(5); // wait for those messages to get sent
#endif
  // initialize exit message
  SMsg_GS_Action msg;
  msg.Init(SMsg_GS_Action::ACTION_EXIT);
  // send to all server threads
  GsUtil::BroadcastInternalMessage(main_, msg, GsMain::THREAD_GAMESERVER);
  // wait a few seconds
#ifdef WIN32
  Sleep(5000);
#else
  pth_sleep(5); 
#endif
  // then wait until all player threads have exited, or up to a minute
  for (int i = 0; i < 30; ++i) {
    if (main_->PlayerSet()->NumPlayers() > 0) {
#ifdef WIN32
  Sleep(2000);
#else
  pth_sleep(2); 
#endif
    }
  }
#ifdef WIN32
  Sleep(5000);
#else
  pth_sleep(5); 
#endif
  // then exit ourselves
  SetDone();
}

////
// send_GMsg_LoginAck
////

void GsGameThread::send_GMsg_LoginAck(LmConnection* conn, time_t conn_time,  int status, int suspended_days)
{
  DECLARE_TheLineNum;
  GMsg_LoginAck msg;
  // this is only called if ack is a negative, ie. status != OK; thus, the
  // entire message is not initialized
  if (conn->ConnectTime() != conn_time) { // usurped connection
    status = GMsg_LoginAck::LOGIN_UNKNOWNERROR;
  }
  msg.SetVersion(GsMain::GAME_VERSION);
  msg.SetStatus(status);
  msg.SetNumItems(suspended_days);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_PreLoginAck
////

void GsGameThread::send_GMsg_PreLoginAck(LmConnection* conn, int version, int status, 
	  TCHAR* challenge)
{
  DECLARE_TheLineNum;
  GMsg_PreLoginAck msg;

  msg.SetVersion(GsMain::GAME_VERSION);
  msg.SetStatus(status);
  msg.SetChallenge(challenge);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}



// this version is sent only if the gamed is full and we want to redirect 
// the client elsewhere

void GsGameThread::send_GMsg_LoginAck(LmConnection* conn, time_t conn_time, int status, 
									  char* ip_address, int port_num)
{
  DECLARE_TheLineNum;
  GMsg_LoginAck msg;
  // this is only called if ack is a negative, ie. status != OK; thus, the
  // entire message is not initialized
  if (conn->ConnectTime() != conn_time) { // usurped connection
    status = GMsg_LoginAck::LOGIN_UNKNOWNERROR;
  }
  msg.SetVersion(GsMain::GAME_VERSION);
  msg.SetStatus(status);
  msg.SetServerPort(port_num);
  msg.SetDescription(ip_address);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

void GsGameThread::send_GMsg_LoginAck(LmConnection* conn, GsPlayer* player, 
		short max_minutes_online, short session_minutes,  unsigned char gamesite, lyra_id_t gamesite_id)
{
  DECLARE_TheLineNum;
  GMsg_LoginAck msg;
  int           version;
  // this is only called if login is sucessful
// max_minutes_online is sent to the client to disconnect after the appropriate interval
 
  if (player->DB().AccountType() == LmPlayerDB::ACCT_ADMIN)
    version = -GsMain::GAME_VERSION;
  else
    version = GsMain::GAME_VERSION;

  //   TLOG_Debug(_T("%s: about to create loginack; player gained %d xp, lost %d xp offline"), method, player->DB().XPGained(), player->DB().XPLost());
  //time_t UNIX_time = time(NULL);
  msg.Init(version, GMsg_LoginAck::LOGIN_OK, player->DB().PlayerID(), main_->ServerPort(),  player->DB().Stats(), player->Avatar(), player->DB().Arts(), player->DB().XPGained(), player->DB().XPLost(), player->DB().Inventory().NumItems(), 
	  max_minutes_online, session_minutes, /*UNIX_time,*/ player->DB().X(), 
	  player->DB().Y(), player->DB().LevelID(), gamesite, gamesite_id, 
	  player->DB().Stats().PP(), player->DB().Stats().PPPool(),
	  player->DB().AvatarDescrip());
  // fill in inventory
  for (int i = 0; i < player->DB().Inventory().NumItems(); ++i) {
    msg.SetItem(i, player->DB().Inventory().ItemByIndex(i), player->DB().Inventory().ItemX(i));
  }

  main_->OutputDispatch()->SendMessage(&msg, conn);

	//player->DumpInventory(_T("Forming loginack message"));

  //TLOG_Debug(_T("Login ack size = %d"), msg.MessageSize());
  //LmLogFile* dumpOut = (LmLogFile*)(main_->Log());
  //msg.Dump(dumpOut->Stream());

}

////
// send_GMsg_ServerDown
////

void GsGameThread::send_GMsg_ServerDown(int status)
{
  DECLARE_TheLineNum;
  GMsg_ServerDown msg;
  msg.Init(status);
  // send to all players
  GsPlayerList players;
  main_->PlayerSet()->GetPlayerList(players);
  for (GsPlayerList::iterator i = players.begin(); !(bool)(i == players.end()); ++i) {
    LmConnection* conn = (*i)->Connection();
    if (conn) {
      main_->OutputDispatch()->SendMessage(&msg, conn);
    }
  }
}


////
// send_SMsg_ServerStatus
////

void GsGameThread::send_SMsg_ServerStatus(LmConnection* conn)
{
  DEFMETHOD(GsGameThread, send_SMsg_ServerStatus);
  SMsg_ServerStatus msg;
  // get playerlist
  GsPlayerList plist;
  main_->PlayerSet()->GetPlayerList(plist);
  // initialize message
  unsigned short frac_value = 2112;
  msg.Init(main_->Uptime(), main_->PlayerSet()->NumLogins(), plist.size(),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_PLAYER),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_MONSTER),
	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_ADMIN),
	   //	   main_->PlayerSet()->NumPlayers(LmPlayerDB::ACCT_PMARE),
	   main_->PlayerSet()->MaxPlayers(), main_->ConnectionSet()->NumConnections(),
	   main_->ConnectionSet()->MaxConnections(), main_->ServerPid(), main_->ParentPid(),
	   LmUtil::GetCPULoad(&frac_value));
  // copy playerids
  int pi = 0;
  for (GsPlayerList::iterator i = plist.begin(); !(bool)(i == plist.end()); ++i) {
    msg.SetPlayerID(pi, (*i)->PlayerID());
    ++pi;
  }
  main_->OutputDispatch()->SendMessage(&msg, conn);
  // TLOG_Debug(_T("%s: frac_value = %d"), method, frac_value);
}

////
// send_SMsg_ConnStatus
////

void GsGameThread::send_SMsg_ConnStatus(LmConnection* conn)
{
  DEFMETHOD(GsGameThread, send_SMsg_ConnStatus);
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

void GsGameThread::send_SMsg_PlayerStatus(LmConnection* conn, lyra_id_t playerid)
{
  SMsg_PlayerStatus msg;
  // get player
  GsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (player) {
    msg.Init(player->PlayerID(), player->DB().PlayerName(), player->LevelID(), player->RoomID(),
	     player->PlayerUpdate().X(), player->PlayerUpdate().Y(), player->DB().AccountType(),
	     player->Online(), player->ClientAddress().IPAddress(), player->IdleTime());
  }
  else {
    msg.Init(playerid, _T("(unknown)"), 0, 0, 0, 0, 'U', 0, INADDR_ANY, 0);
  }
  main_->OutputDispatch()->SendMessage(&msg, conn);
}
