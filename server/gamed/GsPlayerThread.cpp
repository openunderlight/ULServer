// GsPlayerThread.cpp  -*- C++ -*-
// $Id: GsPlayerThread.cpp,v 1.83 1998/05/02 00:09:10 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsPlayerThread.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "GsPlayerThread.h"
#include "LyraDefs.h"
#include "LmParty.h"
#include "GsMain.h"
#include "LmLevelDBC.h"
#include "LmGlobalDB.h"
#include "LmServerDBC.h"
#include "GMsg_All.h"
#include "SMsg_All.h"
#include "GsPlayer.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmSrvMesgBuf.h"
#include "GsOutputDispatch.h"
#include "GsUtil.h"
#include "LmTimer.h"
#include "LmSockAddrInet.h"
#include "LmMesgBufPool.h"
#include "RMsg_All.h"
#include "GsLevelSet.h"
#include "GsMacros.h"
#include "LyraMessage.h"
#include "LmLogFile.h"
#include "LmGuildDBC.h"
#include "LmPlayerNameMap.h"
#include "LmGoalInfo.h"
#include "LmThreadPool.h"
#include "GsPlayerThreadSet.h"
#include "GsPlayerSet.h"
#include "LmPlayerDBC.h"
#include "LmItemDBC.h"
#include "LmDatabase.h"

#ifndef USE_INLINE
#include "GsPlayerThread.i"
#endif

////
// Constructor
////

GsPlayerThread::GsPlayerThread(GsMain* gsmain)
  : LmThread(gsmain->BufferPool(), &logf_),
    main_(gsmain),
    num_uses_(0),
    num_save_sigs_(0),
    player_(0)
{
  // register message handlers
  register_handlers();
}

////
// Destructor
////

GsPlayerThread::~GsPlayerThread()
{
  // empty
}

////
// Login
////

void GsPlayerThread::Login(GsPlayer* player)
{
  player_ = player;
  num_uses_++;
  num_save_sigs_ = 0;
#ifdef RELEASE
  // no player logs in release build
  player_->SetLog(0);
#else
  // no logs for monsters on dev server, but players do have them
  if (player_->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) {
    open_log();
    player_->SetLog(Log());
  }
  else {
    player_->SetLog(0);
  } 
#endif
}

////
// Logout
////

void GsPlayerThread::Logout()
{
#ifndef RELEASE
  // close logs; logs only there on dev server
  // no logs for monsters
  if (player_->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) {
    close_log();
  }
#endif
  player_ = 0;
  //TLOG_Debug(_T("Logout called"));
}

////
// Run
////

void GsPlayerThread::Run()
{
  LmUtil::BlockAllSignals();
  LmThread::Run();
}

////
// Dump
////

void GsPlayerThread::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<GsPlayerThread[%p,%d]: main=[%p] player=[%p] uses=%d>\n"), this, sizeof(GsPlayerThread),
	  main_, player_, num_uses_);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void GsPlayerThread::open_log()
{
  // no logs for monsters
  if (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return;
  }
#ifndef RELEASE
  logf_.Init(_T("player"), _T(""), player_->PlayerID());
  logf_.Open(main_->GlobalDB()->LogDir());
#endif /* !RELEASE */
}

////
// close_log
////

void GsPlayerThread::close_log()
{
  // no logs for monsters
  if (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return;
  }
#ifndef RELEASE
  logf_.Close();
#endif /* !RELEASE */
}

////
// register_handlers
////


void GsPlayerThread::register_handlers()
{
  // default message handler
  SetDefaultHandler((MsgHandler)&GsPlayerThread::handle_Default);

  // register GMsg message handlers
  RegisterHandler(GMsg::LOGOUT, (MsgHandler)&GsPlayerThread::handle_GMsg_Logout);
  RegisterHandler(GMsg::GETITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_GetItem);
  RegisterHandler(GMsg::PUTITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_PutItem);
  RegisterHandler(GMsg::CREATEITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_CreateItem);
  RegisterHandler(GMsg::DESTROYITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_DestroyItem);
  RegisterHandler(GMsg::UPDATEITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_UpdateItem);
  RegisterHandler(GMsg::UPDATESTATS, (MsgHandler)&GsPlayerThread::handle_GMsg_UpdateStats);
  RegisterHandler(GMsg::GOTOLEVEL, (MsgHandler)&GsPlayerThread::handle_GMsg_GotoLevel);
  RegisterHandler(GMsg::CHANGESTAT, (MsgHandler)&GsPlayerThread::handle_GMsg_ChangeStat);
  RegisterHandler(GMsg::DESTROYROOMITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_DestroyRoomItem);
  RegisterHandler(GMsg::GETLEVELPLAYERS, (MsgHandler)&GsPlayerThread::handle_GMsg_GetLevelPlayers);
  RegisterHandler(GMsg::CHANGEAVATAR, (MsgHandler)&GsPlayerThread::handle_GMsg_ChangeAvatar);
  RegisterHandler(GMsg::GOAL, (MsgHandler)&GsPlayerThread::handle_GMsg_Goal);
  RegisterHandler(GMsg::GETGOALHDRS, (MsgHandler)&GsPlayerThread::handle_GMsg_GetGoalHdrs);
  RegisterHandler(GMsg::GETREPORTHDRS, (MsgHandler)&GsPlayerThread::handle_GMsg_GetReportHdrs);
  RegisterHandler(GMsg::POSTGOAL, (MsgHandler)&GsPlayerThread::handle_GMsg_PostGoal);
  RegisterHandler(GMsg::POSTREPORT, (MsgHandler)&GsPlayerThread::handle_GMsg_PostReport);
  RegisterHandler(GMsg::GIVEITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_GiveItem);
  RegisterHandler(GMsg::TAKEITEMACK, (MsgHandler)&GsPlayerThread::handle_GMsg_TakeItemAck);
  RegisterHandler(GMsg::LOCATEAVATAR, (MsgHandler)&GsPlayerThread::handle_GMsg_LocateAvatar);
  RegisterHandler(GMsg::PING, (MsgHandler)&GsPlayerThread::handle_GMsg_Ping);
  RegisterHandler(GMsg::SHOWITEM, (MsgHandler)&GsPlayerThread::handle_GMsg_ShowItem);
  RegisterHandler(GMsg::GETITEMDESCRIPTION, (MsgHandler)&GsPlayerThread::handle_GMsg_GetItemDescription);
  RegisterHandler(GMsg::SETAVATARDESCRIPTION, (MsgHandler)&GsPlayerThread::handle_GMsg_SetAvatarDescription);
  RegisterHandler(GMsg::GETPLAYERNAME, (MsgHandler)&GsPlayerThread::handle_GMsg_GetPlayerName);
  RegisterHandler(GMsg::GRANTPPOINT, (MsgHandler)&GsPlayerThread::handle_GMsg_GrantPPoint);
  RegisterHandler(GMsg::USEPPOINT, (MsgHandler)&GsPlayerThread::handle_GMsg_UsePPoint);

  // register RMsg message handlers (proxy)
  RegisterHandler(RMsg::LOGOUT, (MsgHandler)&GsPlayerThread::handle_RMsg_Logout);
  RegisterHandler(RMsg::GOTOROOM, (MsgHandler)&GsPlayerThread::handle_RMsg_GotoRoom);
  RegisterHandler(RMsg::PARTY, (MsgHandler)&GsPlayerThread::handle_RMsg_Party);
  RegisterHandler(RMsg::SPEECH, (MsgHandler)&GsPlayerThread::handle_RMsg_Speech);
  RegisterHandler(RMsg::PLAYERMSG, (MsgHandler)&GsPlayerThread::handle_RMsg_PlayerMsg);
  RegisterHandler(RMsg::GETAVATARDESCRIPTION, (MsgHandler)&GsPlayerThread::handle_RMsg_GetAvatarDescription);
  RegisterHandler(RMsg::GETROOMDESCRIPTION, (MsgHandler)&GsPlayerThread::handle_RMsg_GetRoomDescription);

  // register SMsg message handlers
  RegisterHandler(SMsg::PROXY, (MsgHandler)&GsPlayerThread::handle_SMsg_Proxy);
  RegisterHandler(SMsg::ITEMDROP, (MsgHandler)&GsPlayerThread::handle_SMsg_ItemDrop);
  RegisterHandler(SMsg::ITEMPICKUP, (MsgHandler)&GsPlayerThread::handle_SMsg_ItemPickup);
  RegisterHandler(SMsg::GIVEITEM, (MsgHandler)&GsPlayerThread::handle_SMsg_GiveItem);
  RegisterHandler(SMsg::GIVEITEMACK, (MsgHandler)&GsPlayerThread::handle_SMsg_GiveItemAck);
  RegisterHandler(SMsg::TAKEITEMACK, (MsgHandler)&GsPlayerThread::handle_SMsg_TakeItemAck);
  RegisterHandler(SMsg::LOCATEAVATAR, (MsgHandler)&GsPlayerThread::handle_SMsg_LocateAvatar);
  RegisterHandler(SMsg::PARTYLEADER, (MsgHandler)&GsPlayerThread::handle_SMsg_PartyLeader);
  RegisterHandler(SMsg::SHOWITEM, (MsgHandler)&GsPlayerThread::handle_SMsg_ShowItem);

  // register SMsg_GS message handlers
  RegisterHandler(SMsg::GS_LOGIN, (MsgHandler)&GsPlayerThread::handle_SMsg_GS_Login);
  RegisterHandler(SMsg::GS_ACTION, (MsgHandler)&GsPlayerThread::handle_SMsg_GS_Action);
}


////
// handle_Default
////

void GsPlayerThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, GsPlayerThread::handle_Default);
  HANDLER_ENTRY(true);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// guild_error: deal with error returned from guild database
////

void GsPlayerThread::guild_error(int rc, int sqlcode, int err_status, lyra_id_t id)
{
  DEFMETHOD(GsPlayerThread, guild_error);
  switch (rc) {
  case LmGuildDBC::MYSQL_ERROR:
    TLOG_Error(_T("%s: database connection error: sqlcode=%d"), method, sqlcode);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::DB_UNAVAILABLE, 0);
    break;
  default: // unknown
    TLOG_Error(_T("%s: unknown database error: rc=%d sqlcode=%d"), method, rc, sqlcode);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::DB_UNAVAILABLE, 0);
    break;
  }
}

////
// get_goalinfo: return 0 if ok, < 0 if error; handles sending an error message to client
////

int GsPlayerThread::get_goalinfo(LmGoalInfo& goalinfo, lyra_id_t goalid, int err_status)
{
  DEFMETHOD(GsPlayerThread, get_goalinfo);
  // get goal information
  int rc = main_->GuildDBC()->GetGoalInfo(goalid, goalinfo);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, err_status, goalid);
  }
  return rc;
}

////
// perform_logout: handle logout stuff
////

void GsPlayerThread::perform_logout(const GMsg_Logout& msg)
{
  DEFMETHOD(GsPlayerThread, perform_logout);
  int status = msg.Status();
  lyra_id_t playerid = player_->PlayerID();
  LmConnection* conn = player_->Connection();

  if ((player_->DB().AccountType() == LmPlayerDB::ACCT_PLAYER) ||
      (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE)) {

    player_->UndoDrops();
    
    const LmInventory& inv = player_->DB().Inventory();
    lyra_id_t roomid = player_->RoomID();
    lyra_id_t levelid = player_->LevelID();
    LmPosition pos;
    pos.SetX(player_->PlayerUpdate().X());
    pos.SetY(player_->PlayerUpdate().Y());
    
    // Remove Artifacts from player inventory
    // first count the artifacts, then do a separate loop to delete
    // or else some will be missed
    int artifact_count = 0;
    int i = 0;
    for (i = 0; i < inv.NumItems(); ++i) {
      LmItem item = inv.ItemByIndex(i);
      if (item.FlagSet(LyraItem::FLAG_ALWAYS_DROP)) {
	artifact_count++;
      }
    }


    // now remove artifacts from inventory
    // artifact_count == 0
    int dropped = 0;

    while (artifact_count > 0) {
      for (i = 0; i< inv.NumItems(); ++i) {
	LmItem item = inv.ItemByIndex(i);
	TLOG_Debug(_T("checking item %d, i = %d"), item.Serial(), i);
	if (item.FlagSet(LyraItem::FLAG_ALWAYS_DROP)) {
	  // drop into room, with playerid set to 0 so that room knows it must accept item
	  TLOG_Debug(_T("%s: player dropping always_drop item %d"), method, item.Serial());
	  dropped++;
	  artifact_count--;
	  if (player_->InLevel() && player_->LevelConnection()) 
	    send_SMsg_PutItem(player_->LevelConnection(), Lyra::ID_UNKNOWN, roomid, item, pos, GMsg_PutItem::DEFAULT_TTL);
	  else // set item owned by the 
	    main_->ItemDBC()->SetItemOwners(item.Serial(), levelid, roomid, 0);
	  
	  //player_->SetItemOwners(item.Serial(),  levelid, roomid, 0);
	  
	  // remove from inventory
	  player_->RemoveItem(item.Header());
	}
      }
    }

    int k = dropped;
  }

  if (player_->InLevel() && player_->LevelConnection()) {
    // send proxied rmsg_logout to room server if player hasn't logged out of level
    TLOG_Debug(_T("%s: player in level, sending RMsg_Logout"), method);
    send_RMsg_Logout(player_->LevelConnection(), RMsg_Logout::FINAL);
  }
  
  // if player was a soulsphere, then lose xp (unless logout is server-initiated)
  if ((player_->PlayerUpdate().FlagSet(LmPeerUpdate::LG_SOULSPHERE)) &&
      (status != GMsg_Logout::LOGOUT_SERVERDOWN)) {
    // determine how much XP to lose
    int xp_adj = - (int) ((double) player_->DB().Stats().XP() * 0.01); // lose up to 1%
    // don't send to player, they've logged out
    adjust_xp(xp_adj, _T("exiting while a soulsphere"), Lyra::ID_UNKNOWN, false);
  }
  // if any items in logout message, store their positions in the inventory
  if (msg.NumItems() > 0) {
    for (int i = 0; i < msg.NumItems(); ++i) {
	  //short itemx = msg.ItemX(i) | (msg.ItemFlags(i) << 8);
      player_->SetItemX(msg.ItemSerial(i), msg.ItemX(i));
    }
  }

  // log player out, save player database, remove player from player set
  main_->PlayerSet()->RemovePlayer(player_);
  // NOTE: at this point, player_ is not valid
  // disconnect player if ids match
  if (conn && (conn->ID() == playerid)) {
	main_->ConnectionSet()->RemoveConnection(conn); //*****
//	conn->Disable();
  }
  // remove self from thread pool, don't delete, put back into spare thread set
  main_->ThreadPool()->RemoveThread(playerid, false);
  
  //TLOG_Debug(_T("%s: removing self from thread pool for player %u"), method, playerid);
  
  main_->PlayerThreadSet()->ReturnPlayerThread(this);
  // go back to inactive state
  Logout();
}

////
// adjust_xp - adjust player's xp by given amount (+ or -), optionally send out
//   changestat message to player, log xp change and reason to security log
////

void GsPlayerThread::adjust_xp(int xp_adj, const TCHAR* why, lyra_id_t why_id, bool send_msg)
{
  DEFMETHOD(GsPlayerThread, adjust_xp);
  // monsters and admins do not ever gain or lose XP
  if ((player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN) ||
      (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER)) {
    return;
  }

  lyra_id_t playerid = player_->DB().PlayerID();
  // changestat message that will be sent
  GMsg_ChangeStat msg;
  // update player record/stats
  int old_orbit = player_->DB().Stats().Orbit();
  xp_adj = player_->AdjustXP(xp_adj, msg);
  int new_orbit = player_->DB().Stats().Orbit();

  if  (player_->DB().AccountType() == LmPlayerDB::ACCT_PLAYER) {
    
    // was there an actual change?
    if (xp_adj != 0) {
      if (why_id != Lyra::ID_UNKNOWN) {
	SECLOG(-3, _T("%s: player %u: change of %d in xp due to %s %u"), method, playerid, xp_adj, why, why_id);
      }
      else {
	SECLOG(-3, _T("%s: player %u: change of %d in xp due to %s"), method, playerid, xp_adj, why);
      }
      if (old_orbit != new_orbit) {
	SECLOG(-3, _T("%s: player %u: orbit %d -> %d"), method, playerid, old_orbit, new_orbit);
      }
    }
  }  else if (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE) {
    bool avatar_changed = false;
    int old_avatar_type = player_->Avatar().AvatarType();
    int new_avatar_type;
    int new_dreamsoul = 99;
    if (new_orbit != old_orbit) { // evolve/devolve?
      if (new_orbit < 5) {
	  new_avatar_type = Avatars::BOGROM; 
	  new_dreamsoul = 50; 
      } else if (new_orbit < 15)
	new_avatar_type = Avatars::AGOKNIGHT;
      else if (new_orbit > 44)
	new_avatar_type = Avatars::HORRON;
      else 
	new_avatar_type = Avatars::SHAMBLIX;
      
      if (new_avatar_type != old_avatar_type)
	avatar_changed = true;
    } 
    if (avatar_changed) {
      LmAvatar avatar = player_->Avatar();
      avatar.SetAvatarType(new_avatar_type);
      ((class LmPlayerDB&)player_->DB()).SetAvatar(avatar);
      player_->ChangeAvatar(avatar, GMsg_ChangeAvatar::AVATAR_CURRENT);
      GMsg_ChangeAvatar camsg;
      camsg.Init(avatar, GMsg_ChangeAvatar::AVATAR_CURRENT);
      main_->OutputDispatch()->SendMessage(&camsg, player_->Connection());
      if (player_->InLevel() && player_->LevelConnection()) {
	send_RMsg_ChangeAvatar(player_->LevelConnection(), player_->Avatar());
      }
      if (send_msg) {
	main_->OutputDispatch()->SendMessage(&camsg, player_->Connection());
      }      
      // now change stat message for new max dreamsoul
      msg.SetNumChanges(2); // XP + dreamsoul
      msg.InitChange(1, GMsg_ChangeStat::SET_STAT_MAX, 0, new_dreamsoul);
    }
  }

  // send ChangeStat message to client, if anything changed
  if ((msg.NumChanges() > 0) && send_msg) {
    main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
  }
}

////
// adjust_xp - adjust player's offline  xp by given amount (+ or -), optionally send out
//   changestat message to player, log xp change and reason to security log
////

void GsPlayerThread::adjust_offline_xp(int xp_adj, const TCHAR* why, lyra_id_t why_id, bool send_msg)
{
  DEFMETHOD(GsPlayerThread, adjust_xp);
  // monsters and admins do not ever gain or lose XP
  if ((player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN) ||
      (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER)) {
      //      (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE)) 
    return;
  }
  lyra_id_t playerid = player_->DB().PlayerID();
  // log it
  SECLOG(5, "%s: Player %u: Offline XP Adjusted due to %s %u, value: %i", method, playerid, why, why_id, xp_adj);
  // update player record/stats
  xp_adj = player_->AdjustOfflineXP(xp_adj);

}

////
// perform_locateavatar - look up player's location from database, and send message to client
////

void GsPlayerThread::perform_locateavatar(lyra_id_t playerid, const TCHAR* playername)
{
  DEFMETHOD(GsPlayerThread, perform_locateavatar);
  bool gm = (player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN);
  int status = GMsg_LocateAvatarAck::LOCATE_FOUND;
  lyra_id_t levelid = 0;
  lyra_id_t roomid = 0;
  int acct_type = 0;
  // make db transaction
  int rc = main_->PlayerDBC()->GetLocation(playerid, levelid, roomid, acct_type);
  int sc = main_->PlayerDBC()->LastSQLCode();
  // int lt = main_->PlayerDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmPlayerDBC::GetLocation took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: could not get player %u location"), method, playerid);
    status = GMsg_LocateAvatarAck::LOCATE_NOTLOGGEDIN; // best fit for this
    //GsUtil::HandlePlayerError(main_, method, rc, sc);
  }
  else if ((levelid == Lyra::ID_UNKNOWN) || (roomid == Lyra::ID_UNKNOWN)) {
    status = GMsg_LocateAvatarAck::LOCATE_NOTLOGGEDIN;
  }
  // reset roomid to unknown, since database was used
  roomid = 0;
  // player hidden?
  bool hidden = false;
  if (levelid > Lyra::HIDDEN_DELTA) {
    hidden = true;
    levelid -= Lyra::HIDDEN_DELTA;
  }
  // if hidden, reset location, unless player is GM
  if (hidden) {
    if (gm)
	status = GMsg_LocateAvatarAck::LOCATE_FOUND_HIDDEN;
    else {
	 status = GMsg_LocateAvatarAck::LOCATE_HIDDEN;
	 levelid = roomid = Lyra::ID_UNKNOWN;
    }
  }
  // if looking for a monster, don't give any useful information
  if ((acct_type == LmPlayerDB::ACCT_MONSTER) && !gm) {
    status = GMsg_LocateAvatarAck::LOCATE_AGENT;
    levelid = player_->LevelID();
    roomid = player_->RoomID();
  }
  // initialize and send message to client
  send_GMsg_LocateAvatarAck(player_->Connection(), playername, status, levelid, roomid);
}

////
// perform_locateavatar_group - look up a group of player locations from db, send to client
////

void GsPlayerThread::perform_locateavatar_group(const GMsg_LocateAvatar& msg)
{
  DEFMETHOD(GsPlayerThread, perform_locateavatar_group);
  bool gm = (player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN);
  // output message
  GMsg_LocateAvatarAck outmsg;
  outmsg.SetNumPlayers(msg.NumPlayers());
  // loop through players
  for (int i = 0; i < msg.NumPlayers(); ++i) {
    const TCHAR* playername = msg.PlayerName(i);
    // essentially the same code as above, but reusing it would be difficult
    int status = GMsg_LocateAvatarAck::LOCATE_FOUND;
    lyra_id_t levelid = 0;
    lyra_id_t roomid = 0;
    // lookup playerid
    lyra_id_t playerid = main_->PlayerNameMap()->PlayerID(playername);
    if (playerid == Lyra::ID_UNKNOWN) {
      status = GMsg_LocateAvatarAck::LOCATE_PLAYERNOTFOUND;
    }
    else {
      int acct_type = 0;
      // make db transaction
      int rc = main_->PlayerDBC()->GetLocation(playerid, levelid, roomid, acct_type);
      int sc = main_->PlayerDBC()->LastSQLCode();
      if (rc < 0) {
	TLOG_Warning(_T("%s: could not get player %u location"), method, playerid);
	status = GMsg_LocateAvatarAck::LOCATE_NOTLOGGEDIN; // best fit for this
	//GsUtil::HandlePlayerError(main_, method, rc, sc);
      }
      else if ((levelid == Lyra::ID_UNKNOWN) || (roomid == Lyra::ID_UNKNOWN)) {
	status = GMsg_LocateAvatarAck::LOCATE_NOTLOGGEDIN;
      }
      // reset roomid to unknown, since database was used
      roomid = 0;
      // player hidden?
      bool hidden = false;
      if (levelid > Lyra::HIDDEN_DELTA) {
	hidden = true;
	levelid -= Lyra::HIDDEN_DELTA;
      }

      // if hidden, reset location, unless player is GM
      if (hidden) {
	if (gm)
	  status = GMsg_LocateAvatarAck::LOCATE_FOUND_HIDDEN;
	else {
	  status = GMsg_LocateAvatarAck::LOCATE_HIDDEN;
	  levelid = roomid = Lyra::ID_UNKNOWN;
	}
      }

      // if looking for a monster, don't give any useful information
      if ((acct_type == LmPlayerDB::ACCT_MONSTER) && !gm) {
	status = GMsg_LocateAvatarAck::LOCATE_AGENT;
	levelid = player_->LevelID();
	roomid = player_->RoomID();
      }
    }
    // copy into message
    outmsg.SetPlayerName(i, playername);
    outmsg.SetStatus(i, status);
    outmsg.SetLocation(i, levelid, roomid);
  }
  // send message to client
  main_->OutputDispatch()->SendMessage(&outmsg, player_->Connection());
}

////
// send_motd - open up motd file, send contents to player
////

void GsPlayerThread::send_motd()
{
  DEFMETHOD(GsPlayerThread, send_motd);
  // monsters don't get the motd
  if (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return;
  }
  // everyone else does
  TCHAR fname[FILENAME_MAX];
  main_->GlobalDB()->GetTextFile(fname, _T("motd.txt"));
  FILE* inf =_tfopen(fname, _T("r"));
  if (!inf) {
    // no message of the day, return
    return;
  }
  // read it into a speech-sized buffer
  TCHAR tbuf[Lyra::MAX_SPEECHLEN];
  int bytes_read = fread(tbuf, 1, sizeof(tbuf) - 1, inf);
  tbuf[bytes_read] = '\0';
  // close file
  fclose(inf);
  // TLOG_Debug(_T("%s: sending motd, len=%d"), method,_tcslen(tbuf));
  // create speech message
  RMsg_Speech msg;
  msg.InitServerText(0, tbuf);
  // send to player
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// hide/show player (mind blank)
////

void GsPlayerThread::hide_player(int hide)
{
  DEFMETHOD(GsPlayerThread, hide_player);
  // set the hidden flag
  player_->SetHidden(hide);
  // update location in db
  lyra_id_t levelid = player_->LevelID();
  lyra_id_t roomid = player_->RoomID();
  if (hide) {
    levelid += Lyra::HIDDEN_DELTA;
  }
  int rc = main_->PlayerDBC()->UpdateLocation(player_->PlayerID(), levelid, roomid);
  int sc = main_->PlayerDBC()->LastSQLCode();
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not update player location; rc=%d, sqlcode=%d"), method, rc, sc);
    //    GsUtil::HandlePlayerError(main_, method, rc, sc);
  }
  //  main_->Log()->Debug(_T("%s: player %u hidden state = %d"), method, player_->PlayerID(), hide);
}
