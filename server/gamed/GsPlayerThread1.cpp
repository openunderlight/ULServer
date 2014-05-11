// GsPlayerThread1.cpp  -*- C++ -*-
// $Id: GsPlayerThread1.cpp,v 1.53 1998/06/18 01:36:41 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// handle_GMsg_* methods

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
#include "GsPlayerSet.h"
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
#include "LmItemDBC.h"
#include "LmPlayerDBC.h"
#include "GsPlayerSet.h"

////
// handle_GMsg_Ping
////

void GsPlayerThread::handle_GMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Ping);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_Ping, true); // send error
  // process
  switch (msg.PingType()) {
    case GMsg_Ping::PING_PLAYER_DB: {
      // perform some player db op, ignoring any results/error
      main_->PlayerDBC()->GetLoginStatus(player_->PlayerID());
    }
    break;
    case GMsg_Ping::PING_ITEM_DB: {
      // perform some item db op, ignoring any results/error
      LmInventory inv;
      main_->ItemDBC()->GetPlayerInventory(player_->PlayerID(), inv);
    }
    break;
    case GMsg_Ping::PING_GUILD_DB: {
      // perform some guild db op, ignoring any results/error
      main_->GuildDBC()->InAcceptees(666, player_->PlayerID());
    }
    break;
    case GMsg_Ping::PING_PLAYER_THREAD:
    default:
      // don't do any db op
      break;
  }
  // send ping back
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// handle_GMsg_GetPlayerName
////

void GsPlayerThread::handle_GMsg_GetPlayerName(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetPlayerName);
  DECLARE_TheLineNum;
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetPlayerName, true); // send error
  // look up player
  const TCHAR* pname = main_->PlayerNameMap()->PlayerName(msg.PlayerID());
  if (pname == 0) {
    send_GMsg_RcvPlayerName(conn, 0, _T("unknown"), msg.RequestID());
  }
  else {
    send_GMsg_RcvPlayerName(conn, msg.PlayerID(), pname, msg.RequestID());
  }
}

////
// handle_GMsg_GrantPPoint
////

void GsPlayerThread::handle_GMsg_GrantPPoint(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Logout);
  HANDLER_ENTRY(false);

  if (!player_) {
    return;
  }

  if (conn) {
    // pre-conditions
    CHECK_CONN_ISCLIENT();
    CHECK_CONN_ID();
  }
  // accept message
  ACCEPT_MSG(GMsg_GrantPPoint, true); 

  int retval;

  if ((player_->DB().AccountType() != LmPlayerDB::ACCT_PLAYER) && 
      (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN)) {
    retval = GMsg_PPointAck::UNKNOWN_ERR;
  } else { 
    retval = main_->PlayerDBC()->GrantPP(player_->PlayerID(), msg.PlayerID(), (char*)msg.Why(), player_->DB ().AccountType ());
  }
  
  send_GMsg_PPointAck(conn, GMsg_PPointAck::GRANT_ACK, retval);

  if (retval == GMsg_PPointAck::GRANT_OK) { // cool!
	// change to player message, forward to lucky player
	int reward_pool = player_->DB().Stats().PPPool();
	reward_pool--;
	player_->SetPPPool(reward_pool);
	send_RMsg_PlayerMsg_GrantPPoint(msg.PlayerID());
  if (player_->DB ().AccountType () == LmPlayerDB::ACCT_ADMIN) {
    SECLOG(-4, _T("%s: player %u is a GM and granted PP to player %u for reason %s"), method,
	      player_->PlayerID(), msg.PlayerID(), (char*)msg.Why());
  } else {
	  SECLOG(-4, _T("%s: player %u: granted PP to player %u for reason %s"), method,
	      player_->PlayerID(), msg.PlayerID(), (char*)msg.Why());
  }

  } 

}
 
////
// handle_GMsg_UsePPoint
////


void GsPlayerThread::handle_GMsg_UsePPoint(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Logout);
  HANDLER_ENTRY(false);

  if (!player_) {
    return;
  }

  if (conn) {
    // pre-conditions
    CHECK_CONN_ISCLIENT();
    CHECK_CONN_ID();
  }
  // accept message
  ACCEPT_MSG(GMsg_UsePPoint, true); 

  if ((player_->DB().AccountType() != LmPlayerDB::ACCT_PLAYER) &&
      (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN)) {
    send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::UNKNOWN_ERR);
    return;
  }

  int cost = 0;
  int pps = player_->DB().Stats().PP();

//	USE_STAT_MAX	= 'M', // trying to increase maxed out stat
	//USE_ART_MAX		= 'A', // trying to plat maxed out art
	//USE_CANT_TRAIN	= 'T', // can't train - maybe not max level before plat?
	//USE_CANT_SPHERE	= 'S', // can't sphere - maybe not ready?

  switch (msg.How()) {
  case GMsg_UsePPoint::GAIN_XP: { // sphere
	  int xp_gain = msg.Var1();
	  int xp_gain2 = LmStats::XPPPCost(player_->DB().Stats().XP());
	  if (xp_gain != xp_gain2) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::UNKNOWN_ERR);
			return;
	  }
	  cost = 1;
	  TCHAR why[256];
	  _stprintf(why, _T("Player %d gained %d XP via a Personality Point"), player_->DB().PlayerID(), xp_gain2);
	  adjust_xp(xp_gain2, why, player_->DB().PlayerID(), true);
	}
	break;

  case GMsg_UsePPoint::BYPASS_TRAIN:  { // train
	  int art_id = msg.Var2();
	  int skill = msg.Var3();
	  if (!player_->CanBeTrained(art_id, skill) || 
		  (GsPlayer::PPMultiplier(art_id) < 1)) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_CANT_TRAIN);
			return;
	  }
	  cost = LmStats::TrainPPCost(art_id, skill, GsPlayer::MinOrbit(art_id), 1 /*GsPlayer::PPMultiplier(art_id)*/);
	  if (cost > pps) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_NOT_ENOUGH);
			return;
		}

      int old_skill = player_->DB().Arts().Skill(art_id);
      if (player_->ChangeSkill(art_id, skill, true)) {
		// get actual skill level
		skill = player_->DB().Arts().Skill(art_id);
		SECLOG(-4, _T("%s: player %u: trained in art %d, skill %d -> %d, by %d personality points"), method,
	       player_->PlayerID(), art_id, old_skill, skill, cost);
		// if we've just gotten ordained, we get a quest XP pool
		if ((art_id == Arts::TRAIN) && (old_skill == 0)) {
	      player_->SetQuestPoolXP(Lyra::QUEST_XP_POOL);		
		}
      }
      else {
	// log unsuccessful train
		SECLOG(-4, _T("%s: player %u: unsuccessful tried to train with pps in art %d, skill %d -> %d, cost %u"), method,
	       player_->PlayerID(), art_id, old_skill, skill, cost);
		// set skill to 0, so client knows an attempt was made
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_CANT_TRAIN);
			return;
		}

		break;
		}
  case GMsg_UsePPoint::BYPASS_SPHERE: { // sphere
	  int old_sphere = player_->DB().Stats().Sphere();
	  if (old_sphere == Stats::SPHERE_MAX) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_CANT_SPHERE);
			return;
		}
		cost = LmStats::SphereIncreaseCost(old_sphere+1);
		if (cost > pps) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_NOT_ENOUGH);
			return;
		}

		if (!player_->DB().Stats().ReadyToAdvance(old_sphere + 1)) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_CANT_SPHERE);
			return;

		}
		GMsg_ChangeStat msg_stat;
		player_->AdvanceToNextSphere(msg_stat);
		if (msg_stat.NumChanges() > 0) {
		int new_sphere = player_->DB().Stats().Sphere();
			SECLOG(-3, _T("%s: player %u: used %d ppoints to advance to sphere %d"), method,
			player_->PlayerID(), cost, old_sphere +1);
			main_->OutputDispatch()->SendMessage(&msg_stat, player_->Connection());
		}
		break;
	}
  case GMsg_UsePPoint::USE_ART: { // art
	  int art_id = msg.Var2();
	  int skill = msg.Var3();
	  if (GsPlayer::PPMultiplier(art_id) < 1) {
		send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::UNKNOWN_ERR);
		return;
	  }
	  cost = LmStats::EvokePPCost(art_id, skill, GsPlayer::MinOrbit(art_id), GsPlayer::PPMultiplier(art_id));
	  if (cost > pps) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_NOT_ENOUGH);
			return;
	  }
	  player_->SetPPEvoking(art_id);
	  player_->SetPPSkill(skill);
	  break;
	}
  case GMsg_UsePPoint::STAT_INCREASE: { // stat
	    int stat = msg.Var1();
		int curstat = player_->DB().Stats().MaxStat(stat);
		if (curstat == Stats::STAT_MAX) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_STAT_MAX);
			return;
		}
		cost = LmStats::StatIncreaseCost(player_->DB().Stats().FocusStat(),
			stat, player_->DB().Stats().Orbit(), curstat);
		if (cost > pps) {
			send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_NOT_ENOUGH);
			return;
		}
		// success!
		player_->ChangeMaxStat(stat, curstat + 1);
		SECLOG(-3, _T("%s: player %u: used %d pponts to increase stat %d to %d"), method,
	       player_->PlayerID(), cost, stat, curstat + 1);
		break;
	}
	default:
		send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::UNKNOWN_ERR);
		return;
  }
  // if we get here, were were successful!
  int new_pp = pps - cost;
  player_->SetPPoints(new_pp);
  main_->PlayerDBC()->UsePP(player_->DB().PlayerID(), cost, msg.How(), msg.Var1(), msg.Var2(), msg.Var3());
  send_GMsg_PPointAck(conn, GMsg_PPointAck::USE_ACK, GMsg_PPointAck::USE_OK);

  return;	
}


////
// handle_GMsg_Logout
////

void GsPlayerThread::handle_GMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Logout);
  HANDLER_ENTRY(false);

  // player may be null if fake logout race condition, so don't print error
  if (!player_) {
    return;
  }
  // CHECK_PLAYER_NONNULL();
  // connection can be null, if logout message was faked
  if (conn) {
    // pre-conditions
    CHECK_CONN_ISCLIENT();
    CHECK_CONN_ID();
  }
  // accept message
  ACCEPT_MSG(GMsg_Logout, true); // send error
  // process message
  lyra_id_t playerid = player_->PlayerID();
  int online = player_->Online();
  TLOG_Debug(_T("%s: player logging out, status %c"), method, msg.Status());
  // log deaths/kills if any
  if ((player_->NumKills() > 0) || (player_->NumDeaths() > 0)) {
    SECLOG(-3, _T("%s: player %u: kills=%d deaths=%d"), method, playerid, player_->NumKills(), player_->NumDeaths());
  }
  // check if player has fired a weapon, but was never hit
  //  if (player_->HasFired() && !player_->BeenHit()) {
    //    SECLOG(1, _T("%s: player %u: (warning) fired a weapon while online, but was never hit"), method, playerid);
  //  }
  // check if player was hit, but dreamsoul never decreased
  if (player_->BeenHit() && !player_->DreamsoulDecreased()) {
    SECLOG(1, _T("%s: player %u: (warning) was hit but DS never decreased"), method, playerid);
  }
  // check if player failed any weapon checks
  if (player_->NumWeaponChecksFailed() > 0) {
    SECLOG(1, _T("%s: player %u: failed %d weapon checks"), method, playerid, player_->NumWeaponChecksFailed());
  }
  // log connection statistics
  int time_online = player_->Online();
  if (time_online == 0) 
	  time_online = 1;
  int num_updates = player_->NumUpdates();
  if (conn) {
    SECLOG(-1, _T("%s: player %u: TCP: in=%d/%d (%d/%d per sec) out=%d/%d (%d/%d per sec), UDP: updates=%d (%f per sec)"), method,
  	   playerid, conn->MessagesIn(), conn->BytesIn(), conn->MessagesInRate(), conn->BytesInRate(),
	   conn->MessagesOut(), conn->BytesOut(), conn->MessagesOutRate(), conn->BytesOutRate(), 
	   num_updates, (float)((float)num_updates/(float)time_online));
  }
  // do the rest of the logout thing
  perform_logout(msg);
  // NOTE: player_ no longer valid at this point
  // log the logout
	  SECLOG(-1, _T("%s: player %u: logged out of game, status %c, %d seconds online"), method,
	 playerid, msg.Status(), time_online);
  // signal game thread that player is logged out
  //SMsg_GS_Logout smsg;
  //smsg.Init(playerid, online, msg.Status());
  //GsUtil::SendInternalMessage(main_, smsg, GsMain::THREAD_GAMESERVER);
}

////
// handle_GMsg_LocateAvatar
////

void GsPlayerThread::handle_GMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_LocateAvatar);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_LocateAvatar, true); // send error
  // TODO: check that player has LOCATEAVATAR art? (everyone has it, though)
  // if there is more than one player to look up, only use the database (no level server)
  if (msg.NumPlayers() > 1) {
    perform_locateavatar_group(msg);
    return;
  }
  // check playername
  const TCHAR* playername = msg.PlayerName(0);
  if (_tcslen(playername) == 0) {
    send_GMsg_LocateAvatarAck(conn, _T("(unknown)"), GMsg_LocateAvatarAck::LOCATE_PLAYERNOTFOUND, 0, 0);
    return;
  }
  // look up playerid; if not found, return status
  lyra_id_t targetid = main_->PlayerNameMap()->PlayerID(playername);
  if (targetid == Lyra::ID_UNKNOWN) {
    send_GMsg_LocateAvatarAck(conn, playername, GMsg_LocateAvatarAck::LOCATE_PLAYERNOTFOUND, 0, 0);
    return;
  }
  // if not in level, do database lookup and return result
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    perform_locateavatar(targetid, playername);
    return;
  }
  // send request to level server for player info; result dealt with in handle_SMsg_LocateAvatar
  send_SMsg_LocateAvatar(player_->LevelConnection(), targetid);
}

////
// handle_GMsg_GetItem
////

void GsPlayerThread::handle_GMsg_GetItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetItem, true); // send error
  // process message
  LmItem tmpitem;
  tmpitem.Init(msg.ItemHeader());
  // check if player could add item to inventory
  if (!player_->CanAddItem(msg.ItemHeader())) {
    TLOG_Warning(_T("%s: player cannot add item"), method);
    send_GMsg_ItemPickup(conn, tmpitem, GMsg_ItemPickup::PICKUP_ERROR);
    return;
  }
  // check that player is in a level, and is connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    send_GMsg_ItemPickup(conn, tmpitem, GMsg_ItemPickup::PICKUP_ERROR);
    return;
  }
  // check that source room is in player's level
  if (!player_->LevelDBC()->ContainsRoom(msg.RoomID())) {
    TLOG_Warning(_T("%s: room %u not in level"), method, msg.RoomID());
    send_GMsg_ItemPickup(conn, tmpitem, GMsg_ItemPickup::PICKUP_ERROR);
    return;
  }
  // provisionally pick up item
  player_->StartItemPickup(msg.ItemHeader());
  // send to level server
  send_SMsg_GetItem(player_->LevelConnection(), msg.RoomID(), msg.ItemHeader());
  // response: handle_SMsg_ItemPickup
}

////
// handle_GMsg_PutItem
////

void GsPlayerThread::handle_GMsg_PutItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_PutItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_PutItem, true); // send error
  // check if player can drop the item
  if (!player_->CanDropItem(msg.ItemHeader())) {
    TLOG_Warning(_T("%s: player %u cannot drop item"), method, player_->PlayerID());
    send_GMsg_ItemDrop(conn, msg.ItemHeader(), GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // check that player is in level, and connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    send_GMsg_ItemDrop(conn, msg.ItemHeader(), GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // room 0?
  if (msg.RoomID() == 0) {
    TLOG_Warning(_T("%s: room 0 detected"), method, msg.RoomID());
    send_GMsg_ItemDrop(conn, msg.ItemHeader(), GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // check that target room is in player's level
  if (!player_->LevelDBC()->ContainsRoom(msg.RoomID())) {
    TLOG_Warning(_T("%s: room %u not in level"), method, msg.RoomID());
    send_GMsg_ItemDrop(conn, msg.ItemHeader(), GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // check that item ttl isn't too large (only if regular player)
  if (((player_->DB().AccountType() == LmPlayerDB::ACCT_PLAYER) ||
       (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE)) &&
      (msg.TTL() > GMsg_PutItem::MAX_TTL)) {
    TLOG_Warning(_T("%s: player %u dropping item with ttl %d > max"), method, player_->PlayerID(), msg.TTL());
    msg.SetTTL(GMsg_PutItem::DEFAULT_TTL); // reset to default and continue
  }
  // get actual item from inventory (with state info)
  LmItem tmpitem = ((class LmInventory&)player_->DB().Inventory()).Item(msg.ItemHeader());
  // provisionally drop item
  player_->StartItemDrop(tmpitem);
  // send request to level server
  send_SMsg_PutItem(player_->LevelConnection(), player_->PlayerID(), msg.RoomID(), tmpitem, msg.Position(), msg.TTL());
  // response: handle_SMsg_ItemDrop
}

////
// handle_GMsg_ShowItem
////

void GsPlayerThread::handle_GMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_ShowItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_ShowItem, true); // send error
  // check if player has the item
  if (!player_->DB().Inventory().HasItem(msg.ItemHeader())) {
    TLOG_Warning(_T("%s: player %u does not have item"), method, player_->PlayerID());
    return;
  }
  // check that player is in level, and connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    return;
  }
  // get actual item from inventory
  LmItem tmpitem = ((class LmInventory&)player_->DB().Inventory()).Item(msg.ItemHeader());
  // send showitem to target, through level server
  send_SMsg_ShowItem(player_->LevelConnection(), player_->PlayerID(), msg.TargetID(), tmpitem);
}

////
// handle_GMsg_SetAvatarDescription
////

void GsPlayerThread::handle_GMsg_SetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_SetAvatarDescription);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_SetAvatarDescription, true); // send error
  // update description
  ((LmPlayerDB&)player_->DB()).SetAvatarDescrip(msg.Description());
  // send to level server if connected
  if (player_->LevelConnection()) {
    send_SMsg_SetAvatarDescription(player_->LevelConnection(), player_->PlayerID(), player_->DB().AvatarDescrip());
  }
}

////
// handle_GMsg_GetItemDescription
////

void GsPlayerThread::handle_GMsg_GetItemDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetItemDescription);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetItemDescription, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check that player has item
  if (!player_->DB().Inventory().HasItem(hdr)) {
    TLOG_Warning(_T("%s: player %u does not have item %u"), method, player_->PlayerID(), hdr.Serial());
    return;
  }
  // check that item has a description
  if (!hdr.FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
    TLOG_Warning(_T("%s: item has no description"), method);
    return;
  }
  // get full item
  LmItem item = ((class LmInventory&)player_->DB().Inventory()).Item(hdr);
  // get description from database
  TCHAR desc[Lyra::MAX_ITEMDESC];
  int rc = main_->ItemDBC()->GetItemDescription(hdr.Serial(), desc);
  int sc = main_->ItemDBC()->LastSQLCode();
  if (rc < 0) {
    GsUtil::HandleItemError(main_, method, rc, sc);
    return;
  }
  // get creator name (scrolls only)
  const TCHAR* creator = _T("****"); 
  const TCHAR* target = _T(""); 
  if ((item.NumFields() == 1) && (LyraItem::StateFunction(item.StateField(0)) == LyraItem::SCROLL_FUNCTION)) {
    lyra_item_scroll_t scroll;
    memcpy(&scroll, item.StateField(0), sizeof(scroll));
    lyra_id_t creatorid = scroll.creatorid();
    if (creatorid != 0) { // generated codexes
      creator = main_->PlayerNameMap()->PlayerName(creatorid);
      if (!creator) {
	// *** STRING LITERAL ***
	creator = _T("The Dream itself");
      }
    }
    lyra_id_t targetid = scroll.targetid();
    if (targetid != 0) { 
      target = main_->PlayerNameMap()->PlayerName(targetid);
      if (!target) {
		target = _T("");
      }
    }
  }

  // send description to player
  send_GMsg_ItemDescription(player_->Connection(), hdr.Serial(), creator, desc, target);
}

////
// handle_GMsg_GiveItem
////

void GsPlayerThread::handle_GMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GiveItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GiveItem, true); // send error
  // check if player can give away ("drop") the item
  if (!player_->CanDropItem(msg.ItemHeader())) {
    TLOG_Warning(_T("%s: player %u cannot give item"), method, player_->PlayerID());
    send_GMsg_GiveItemAck(conn, msg.ItemHeader(), GMsg_GiveItemAck::GIVE_ERROR);
    return;
  }
  // check that player isn't giving themself an item
  if (player_->PlayerID() == msg.TargetID()) {
    TLOG_Warning(_T("%s: player giving item to self"), method);
    send_GMsg_GiveItemAck(conn, msg.ItemHeader(), GMsg_GiveItemAck::GIVE_ERROR);
    return;
  }
  // check that player is in level, and connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    send_GMsg_GiveItemAck(conn, msg.ItemHeader(), GMsg_GiveItemAck::GIVE_ERROR);
    return;
  }
  TLOG_Debug(_T("%s: giving item %d to player %u"), method, msg.ItemHeader().Serial(), msg.TargetID());
  // get actual item from inventory (with state info)
  LmItem item = ((class LmInventory&)player_->DB().Inventory()).Item(msg.ItemHeader());
  // provisionally drop item
  player_->StartItemDrop(item);
  // send request to level server
  send_SMsg_GiveItem(player_->LevelConnection(), msg.TargetID(), item);
  // response: handle_SMsg_GiveItemAck
}

////
// handle_GMsg_TakeItemAck
////

void GsPlayerThread::handle_GMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_TakeItemAck);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_TakeItemAck, true); // send error
  // check that player is in a level, and is connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    send_GMsg_TakeItemAck(conn, msg.ItemHeader(), GMsg_TakeItemAck::TAKE_ERROR);
    return;
  }
  // get playerid of item giver
  lyra_id_t giverid = player_->ItemGiver(msg.ItemHeader());
  if (giverid == Lyra::ID_UNKNOWN) {
    TLOG_Warning(_T("%s: item giver or given item not found"), method);
    send_GMsg_TakeItemAck(conn, msg.ItemHeader(), GMsg_TakeItemAck::TAKE_ERROR);
    return;
  }
  TLOG_Debug(_T("%s: take item %d from player %u: %c"), method, msg.ItemHeader().Serial(), giverid, msg.Status());

  if (msg.Status() != GMsg_TakeItemAck::TAKE_YES) {
	  // rejected, remove from take list
	player_->EndItemTake(msg.ItemHeader(), false);
  }
  // send ack to level server, which will send SMsg_TakeItemAck
  send_SMsg_TakeItemAck(player_->LevelConnection(), giverid, msg.ItemHeader(), msg.Status());
  // protocol continued in handle_SMsg_TakeItemAck
}

////
// handle_GMsg_CreateItem
////

void GsPlayerThread::handle_GMsg_CreateItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_CreateItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_CreateItem, true); // send error
  // put created item into a string for further use
  LmItem item = msg.Item();
  TCHAR itemstr[80];
  item.UnParse(itemstr, sizeof(itemstr));
  // debug
  // msg.Dump(((LmLogFile*) main_->Log())->Stream(), 1);
  // if (item.FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
  // TLOG_Debug(_T("%s: item description: '%s'"), method, msg.Description());
  // }
  // check if item can be added to inventory
  if (!player_->CanAddItem(item.Header())) {
    TLOG_Warning(_T("%s: player could not add item to inventory"), method);
    send_GMsg_ItemPickup(conn, item, GMsg_ItemPickup::PICKUP_ERRORCREATE);
    return;
  }
  // check if player is allowed to create given item
  if (!player_->CanCreateItem(item)) {
    SECLOG(6, "%s: player %u: not allowed to create item: %s", method, player_->PlayerID(), itemstr);
    send_GMsg_ItemPickup(conn, item, GMsg_ItemPickup::PICKUP_ERRORCREATE);
    return;
  }
  player_->SetPPEvoking(Arts::NONE); // if it's a pp evoke, only let it happen once!

  // create item in database
  int serial = 0;
  int rc = main_->ItemDBC()->CreateItem(player_->PlayerID(), item, serial, msg.Description());
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::CreateItem took %d ms"), method, lt);
  if ((rc < 0) || (serial == 0)) {
    TLOG_Error(_T("%s: could not create item; serial=%d, rc=%d, sql=%d"), method, serial, rc, sqlcode);
    send_GMsg_ItemPickup(conn, item, GMsg_ItemPickup::PICKUP_ERRORCREATE);
    GsUtil::HandleItemError(main_, method, rc, sqlcode);
    return;
  }
  // update serial
  // TLOG_Debug(_T("%s: database returned serial %d"), method, serial);
  item.Header().SetSerial(serial);
  // log creation
  item.UnParse(itemstr, sizeof(itemstr));
  if (msg.Description ())
    SECLOG(-6, "%s: player %u: created item: %s with text: %s", method, player_->PlayerID(), itemstr, msg.Description ());
  else
    SECLOG (-6, "%s: player %u: created item: %s with NULL text",method,player_->PlayerID (), itemstr);
  // add to player's inventory
  player_->AddItem(item);
  // send ack
  send_GMsg_ItemPickup(conn, item, GMsg_ItemPickup::PICKUP_CREATE);
}

////
// handle_GMsg_DestroyItem
////

void GsPlayerThread::handle_GMsg_DestroyItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_DestroyItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_DestroyItem, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check that player has item
  if (!player_->DB().Inventory().HasItem(hdr)) {
    SECLOG(6, "%s: player %u: cannot destroy item [%u/%u], not in inventory", method,
	   player_->PlayerID(), hdr.ItemID(), hdr.Serial());
    send_GMsg_ItemDrop(conn, hdr, GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // get item, and item string
  LmItem item = ((class LmInventory&)player_->DB().Inventory()).Item(hdr);
  TCHAR itemstr[80];
  item.UnParse(itemstr, sizeof(itemstr));
  // check that item can be destroyed
  if (!player_->CanDestroyItem(hdr)) {
    SECLOG(6, "%s: player %u: cannot destroy item: %s", method, player_->PlayerID(), itemstr);
    send_GMsg_ItemDrop(conn, hdr, GMsg_ItemDrop::DROP_ERROR);
    return;
  }
  // remove from database
  int rc = main_->ItemDBC()->DeleteItem(hdr.Serial());
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::DeleteItem took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: item %d could not be deleted from database"), method, hdr.Serial());
    send_GMsg_ItemDrop(conn, hdr, GMsg_ItemDrop::DROP_ERROR);
    GsUtil::HandleItemError(main_, method, rc, sqlcode);
    return;
  }
  SECLOG(-6, "%s: player %u: destroyed item: %s", method, player_->PlayerID(), itemstr);
  // remove from inventory
  player_->RemoveItem(hdr);
  // send ack to client
  send_GMsg_ItemDrop(conn, hdr, GMsg_ItemDrop::DROP_DESTROY);
}

////
// handle_GMsg_UpdateItem
////

void GsPlayerThread::handle_GMsg_UpdateItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_UpdateItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_UpdateItem, true); // send error
  // process
  LmItem item = msg.Item();
  // can item be updated (check that it's in the inventory, flags, function, etc)
  if (!player_->CanUpdateItem(item)) {
    LmItem oitem = ((class LmInventory&)player_->DB().Inventory()).Item(item.Header());
    TCHAR itemstr[80];
    TCHAR oitemstr[80];
    oitem.UnParse(oitemstr, sizeof(oitemstr));
    item.UnParse(itemstr, sizeof(itemstr));
    SECLOG(6, "%s: player %u: cannot update item: old: %s  new: %s", method,
	   player_->PlayerID(), oitemstr, itemstr);
    return;
  }
  // no entry for regular updates, these would fill the log
  // update item in inventory
  player_->UpdateItem(item);
}

////
// handle_GMsg_UpdateStats
////

void GsPlayerThread::handle_GMsg_UpdateStats(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_UpdateStats);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_UpdateStats, true); // send error
  // process
  player_->UpdateStats(msg.PlayerStats(), msg.Arts());
}

////
// handle_GMsg_ChangeStat
////

void GsPlayerThread::handle_GMsg_ChangeStat(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_ChangeStat);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_ChangeStat, true); // send error
  //msg.Dump(TLOG_Stream()); TLOG_FlushLog();
  // process each change request, keeping track of those that succeed and logging those that don't
  int updates_made = 0;
  GMsg_ChangeStat out_msg; // ack message
  out_msg.Init(msg.NumChanges());
  // TLOG_Debug(_T("%s: number of updates to make: %d"), method, msg.NumChanges());
  for (int i = 0; i < msg.NumChanges(); ++i) {
    int req_type = msg.RequestType(i);
    int stat = msg.Stat(i);
    int value = msg.Value(i);
    TLOG_Debug(_T("%s: req=%d stat=%d val=%d"), method, req_type, stat, value);
    bool update_ok = false; // default: update doesn't succeed
    bool send_update = true; // default: send update back to client
    switch (msg.RequestType(i)) {
    case GMsg_ChangeStat::SET_STAT_CURR:
      update_ok = player_->ChangeCurrentStat(stat, value);
      send_update = false; // don't send current stat values back to client
      break;
    case GMsg_ChangeStat::SET_SKILL: {
      int old_skill = player_->DB().Arts().Skill(stat);
      update_ok = player_->ChangeSkill(stat, value);
      if (update_ok) { // && (old_skill != value)) {
	SECLOG(-4, "%s: player %u: art %d skill %d -> %d", method, player_->PlayerID(), stat, old_skill, value);
      }
    }
    break;
    case GMsg_ChangeStat::SET_STAT_MAX:
    case GMsg_ChangeStat::SET_XP:
      update_ok = false; // client cannot send these
      break;
    default:
      TLOG_Error(_T("%s: unknown request type %d"), method, req_type);
      break;
    }
    // log unsuccessful update requests
    if (!update_ok) {
      SECLOG(4, "%s: player %u: illegal update: req=%d stat=%d val=%d", method,
	     player_->PlayerID(), req_type, stat, value);
    }
    // add update to S->C message?
    if (update_ok && send_update) {
      out_msg.InitChange(updates_made, req_type, stat, value);
      updates_made++;
    }
  }
  // TLOG_Debug(_T("%s: %d/%d updates acknowledged"), method, updates_made, msg.NumChanges());
  out_msg.SetNumChanges(updates_made);
  // send acknowledges for successful updates back to client
  if (updates_made > 0) {
    main_->OutputDispatch()->SendMessage(&out_msg, conn);
  }
}

////
// handle_GMsg_GotoLevel
////

void GsPlayerThread::handle_GMsg_GotoLevel(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GotoLevel);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GotoLevel, true); // send error
  // process
  lyra_id_t levelid = msg.LevelID();
  lyra_id_t roomid = msg.RoomID();
  // if already in a level, send a logout message
  if (player_->InLevel()) {
    // TLOG_Warning(_T("%s: gotolevel before logging out of current level"), method);
    send_RMsg_Logout(player_->LevelConnection(), RMsg_Logout::LOGOUT);
  }
  // check that player can goto the level, either via a return, portal, or entry
  // (also checks that target level/room exist)
  if (!player_->CanGotoLevel(levelid, roomid) &&
      !main_->LevelSet()->CanGoto(player_->LevelID(), player_->RoomID(), levelid, roomid)) {
    SECLOG(1, _T("%s: player %u: illegal entry into level %u room %u from level %u room %u"), method,
	   player_->PlayerID(), levelid, roomid, player_->LevelID(), player_->RoomID());
    // TODO: eventually don't continue
    // GsUtil::Send_RMsg_LoginAck(main_, conn, RMsg_LoginAck::LOGIN_LEVELNOTFOUND, msg.RoomID(), msg.LevelID());
    // return;
  }
  // player isn't in level until the end
  player_->SetInLevel(false);
  // look up level database, store in db cache
  const LmLevelDBC* ldb = main_->LevelSet()->LevelDBC(levelid);
  if (!ldb) {
    TLOG_Warning(_T("%s: could not find level %u"), method, levelid);
    GsUtil::Send_RMsg_LevelLoginAck(main_, conn, RMsg_LoginAck::LOGIN_LEVELNOTFOUND, roomid, levelid);
    return;
  }
  // check that room is in level
  if (!ldb->ContainsRoom(roomid)) {
    TLOG_Warning(_T("%s: could not find room %u in level %u"), method, roomid, levelid);
    GsUtil::Send_RMsg_LevelLoginAck(main_, conn, RMsg_LoginAck::LOGIN_ROOMNOTFOUND, roomid, levelid);
    return;
  }
  // connect to level server
  LmConnection* lsconn = GsUtil::ConnectToLevelServer(main_, ldb);
  if (!lsconn) {
    TLOG_Error(_T("%s: could not connect to level %u server"), method, levelid);
    GsUtil::Send_RMsg_LevelLoginAck(main_, conn, RMsg_LoginAck::LOGIN_SERVERDOWN, roomid, levelid);
    return;
  }
  // log movement
  //  SECLOG(-1, _T("%s: player %u: entering level %u, room %u"), method, player_->PlayerID(), levelid, roomid);
  player_->ReceivedUpdate(msg.PeerUpdate()); // so timeout doesn't happen immediately
  player_->GotoLevel(lsconn, ldb, roomid);
  player_->SetInLevel(true);
  // send player login to level server
  // TLOG_Debug(_T("%s: sending login message to level server (conn %p)"), method, lsconn);
  send_SMsg_LevelLogin(lsconn, roomid, msg.PeerUpdate());
  // update player database
  if (player_->IsHidden()) {
    levelid += Lyra::HIDDEN_DELTA; // player hidden from location?
  }
  int rc = main_->PlayerDBC()->UpdateLocation(player_->PlayerID(), levelid, roomid);
  int sc = main_->PlayerDBC()->LastSQLCode();
  // int lt = main_->PlayerDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmPlayerDBC::UpdateLocation took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not update player location; rc=%d, sqlcode=%d"), method, rc, sc);
    //    GsUtil::HandlePlayerError(main_, method, rc, sc);
  }
}

////
// handle_GMsg_DestroyRoomItem
////

void GsPlayerThread::handle_GMsg_DestroyRoomItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_DestroyRoomItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_DestroyRoomItem, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check if item can actually be destroyed
  if (msg.ItemHeader().FlagSet(LyraItem::FLAG_NOREAP)) {
    SECLOG(6, "%s: player %u: trying to destroy non-reapable room item %d", method, player_->PlayerID(), hdr.Serial());
    return;
  }
  // check if player is in level
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Error(_T("%s: player tried to destroy room item while not connected to lvl server"), method);
    return;
  }    
  // TODO: check that room is in level
  // TODO: check if player has the ability/skill to destroy the item
  // send request to level server
  send_SMsg_DestroyRoomItem(player_->LevelConnection(), msg.RoomID(), msg.ItemHeader());
}

////
// handle_GMsg_GetLevelPlayers
////

void GsPlayerThread::handle_GMsg_GetLevelPlayers(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetLevelPlayers);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetLevelPlayers, true); // send error
  // process
  TLOG_Debug(_T("%s: level=%u"), method, msg.LevelID());
  // check that player is anything other than a normal player
  if (player_->DB().AccountType() == LmPlayerDB::ACCT_PLAYER) {
    SECLOG(5, "%s: player %u: attempted getlevelplayers", method, player_->PlayerID());
    return;
  }
  // check that player is anything other than a normal player
  if (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE) {
    SECLOG(5, "%s: pmare %u: attempted getlevelplayers", method, player_->PlayerID());
    return;
  }
  // connect to level server
  const LmLevelDBC* ldb = main_->LevelSet()->LevelDBC(msg.LevelID());
  if (!ldb) {
    TLOG_Error(_T("%s: could not open level database for level %u"), method, msg.LevelID());
    GsUtil::Send_Error(main_, conn, msg_type, "level %u database not found", msg.LevelID());
    return;
  }
  LmConnection* lsconn = GsUtil::ConnectToLevelServer(main_, ldb);
  if (!lsconn) {
    TLOG_Error(_T("%s: could not connect to level server %u"), method, msg.LevelID());
    GsUtil::Send_Error(main_, conn, msg_type, "level %u server connect error", msg.LevelID());
    return;
  }
  // send request to level server
  send_SMsg_GetLevelPlayers(lsconn);
}

////
// handle_GMsg_ChangeAvatar
////

void GsPlayerThread::handle_GMsg_ChangeAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_ChangeAvatar);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_ChangeAvatar, true); // send error
  // process
  TLOG_Debug(_T("%s: player changing avatar '%c'"), method, msg.Which());
  // change player's avatar, return if new avatar was valid (from LmPlayerDB::FixAvatar())
  int av_rc = player_->ChangeAvatar(msg.Avatar(), msg.Which());
  // check if new avatar is valid
  if (av_rc != 0) {
    // 1: house shield, 2: sphere, 3: teacher
    const TCHAR* reason = _T("unknown");
    switch (av_rc) {
    case 1: reason = _T("house shield"); break;
    case 2: reason = _T("sphere"); break;
    case 3: reason = _T("teacher"); break;
    default: reason = _T("unknown"); break;
    }
    SECLOG(1, _T("%s: player %u: illegal avatar change; reason(%d): illegal %s"), method, player_->PlayerID(), av_rc, reason);
    // continue along, since avatar was fixed
  }
  // if connected to level server, send RMsg_ChangeAvatar
  if (player_->InLevel() && player_->LevelConnection()) {
    send_RMsg_ChangeAvatar(player_->LevelConnection(), player_->Avatar());
  }
}

////
// handle_GMsg_Goal
////

void GsPlayerThread::handle_GMsg_Goal(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_Goal, true); // send error
  // process
  //TLOG_Debug(_T("%s: message dump:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // switch on message type
  switch (msg.RequestType()) {
  case GMsg_Goal::ACCEPT_GOAL:
    handle_GMsg_Goal_AcceptGoal(msg);
    break;
  case GMsg_Goal::REMOVE_GOAL:
    handle_GMsg_Goal_RemoveGoal(msg);
    break;
  case GMsg_Goal::VOTE_YES:
  case GMsg_Goal::VOTE_NO:
    handle_GMsg_Goal_Vote(msg);
    break;
  case GMsg_Goal::EXPIRE_GOAL:
    handle_GMsg_Goal_ExpireGoal(msg);
    break;
  case GMsg_Goal::COMPLETE_GOAL:
    handle_GMsg_Goal_CompleteGoal(msg);
    break;
  case GMsg_Goal::COMPLETE_QUEST:
    handle_GMsg_Goal_CompleteQuest(msg);
    break;
  case GMsg_Goal::DOES_HAVE_CODEX:
	handle_GMsg_Goal_DoesHaveCodex(msg);
	break;
  case GMsg_Goal::GET_GOAL_TEXT:
    handle_GMsg_Goal_GetGoalText(msg);
    break;
  case GMsg_Goal::GET_GOAL_DETAILS:
    handle_GMsg_Goal_GetGoalDetails(msg);
    break;
  case GMsg_Goal::GET_REPORT_TEXT:
    handle_GMsg_Goal_GetReportText(msg);
    break;
  case GMsg_Goal::GET_GOALBOOK_HEADERS:
    handle_GMsg_Goal_GetGoalbookHeaders(msg);
    break;
  case GMsg_Goal::DELETE_REPORT:
    handle_GMsg_Goal_DeleteReport(msg);
    break;
  case GMsg_Goal::GET_GUARDIAN_FLAG:
    handle_GMsg_Goal_GetGuardianFlag(msg);
    break;
  default:
    TLOG_Error(_T("%s: unknown request %d"), method, msg.RequestType());
    break;
  }
}

////
// handle_GMsg_Goal_GetGuardianFlag
////

void GsPlayerThread::handle_GMsg_Goal_GetGuardianFlag(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_GetGuardianFlag);
  lyra_id_t goalid = gmsg.ID();
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::GOAL_NOTFOUND) < 0) {
    return;
  }
  // return result
  int result = GMsg_Goal::GUARDIAN_FLAG_FALSE;
  if (goalinfo.IsGuardianManaged()) {
    result = GMsg_Goal::GUARDIAN_FLAG_TRUE;
  }
  send_GMsg_Goal(player_->Connection(), result, goalid);
}

////
// handle_GMsg_Goal_DeleteReport
////

void GsPlayerThread::handle_GMsg_Goal_DeleteReport(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_DeleteReport);
  lyra_id_t reportid = gmsg.ID();
  TLOG_Debug(_T("%s: player deleting report %u"), method, reportid);
  // report deletion routine checks creator id
  int rc = main_->GuildDBC()->DeleteReport(player_->PlayerID(), gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::DELETE_REPORT_ERROR, reportid);
    return;
  }
  // report deleted
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::DELETE_REPORT_ACK, reportid);
}

////
// handle_GMsg_Goal_AcceptGoal
////

void GsPlayerThread::handle_GMsg_Goal_AcceptGoal(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_AcceptGoal);
  lyra_id_t goalid = gmsg.ID();
  TLOG_Debug(_T("%s: player accepting goal %u"), method, goalid);
  // check goalbook that it's not full
  if (player_->DB().GoalBook().IsFull()) {
    TLOG_Warning(_T("%s: player's goalbook is full"), method);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::ACCEPT_GOAL_ERROR, goalid);
    return;
  }
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::GOAL_NOTFOUND) < 0) {
    return;
  }

 
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player is allowed to accept this goal
  if (!player_->CanAcceptGoal(goalinfo)) {
    SECLOG(7, _T("%s: player %u: not allowed to accept goal %u"), method, player_->PlayerID(), goalid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::ACCEPT_GOAL_ERROR, goalid);
    return;
  }
  // update database
  int rc = main_->GuildDBC()->AcceptGoal(player_->PlayerID(), gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::ACCEPT_GOAL_ERROR, goalid);
    return;
  }
  SECLOG(-7, _T("%s: player %u: accepted goal %u"), method, player_->PlayerID(), goalid);
  // goal accepted
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::ACCEPT_GOAL_ACK, goalid);
  // update player database
  player_->AcceptGoal(goalid);
}

////
// handle_GMsg_Goal_RemoveGoal
////

void GsPlayerThread::handle_GMsg_Goal_RemoveGoal(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_RemoveGoal);
  lyra_id_t goalid = gmsg.ID();
  TLOG_Debug(_T("%s: player removing goal %u"), method, goalid);
  // check that goalbook contains goal
  if (!player_->DB().GoalBook().HasMember(goalid)) {
    TLOG_Warning(_T("%s: goal %u not in goalbook"), method);
    return;
  }
  // goal removed - update player database
  player_->RemoveGoal(goalid);
  // update database
  int rc = main_->GuildDBC()->RemoveGoal(player_->PlayerID(), gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::ACCEPT_GOAL_ERROR, goalid);
    return;
  }

}

////
// handle_GMsg_Goal_Vote
////

void GsPlayerThread::handle_GMsg_Goal_Vote(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_Vote);
  lyra_id_t goalid = gmsg.ID();
  int vote = 0; // unknown
  switch (gmsg.RequestType()) {
  case GMsg_Goal::VOTE_YES:
    vote = Guild::YES_VOTE;
    break;
  case GMsg_Goal::VOTE_NO:
    vote = Guild::NO_VOTE;
    break;
  default: // should never occur
    TLOG_Error(_T("%s: illegal vote type %d"), method, gmsg.RequestType());
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::VOTE_ERROR, goalid);
    return;
    break;
  }
  TLOG_Debug(_T("%s: player voting %d on goal %u"), method, vote, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::VOTE_ERROR) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player can vote on this goal (guild/rank)
  if (!player_->CanVoteOnGoal(goalinfo)) {
    SECLOG(7, _T("%s: player %u: not allowed to vote on goal %u"), method, player_->PlayerID(), goalid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::VOTE_ERROR, goalid);
    return;
  }
  // update database
  int rc = main_->GuildDBC()->VoteGoal(player_->PlayerID(), vote, gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::VOTE_ERROR, goalid);
    return;
  }
  SECLOG(-7, _T("%s: player %u: voted %d on goal %u"), method, player_->PlayerID(), vote, goalid);
  // vote accepted
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::VOTE_ACK, goalid);
}

////
// handle_GMsg_Goal_ExpireGoal
////

void GsPlayerThread::handle_GMsg_Goal_ExpireGoal(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_ExpireGoal);
  lyra_id_t goalid = gmsg.ID();
  // TLOG_Debug(_T("%s: player expiring goal %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::EXPIRE_GOAL_ERROR) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player can expire goal (must be goal creator)
  if (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN) {
	  if (player_->PlayerID() != goalinfo.CreatorID()) {
	    SECLOG(7, _T("%s: player %u: not allowed to expire goal %u"), method, player_->PlayerID(), goalid);
	    send_GMsg_Goal(player_->Connection(), GMsg_Goal::EXPIRE_GOAL_ERROR, goalid);
	    return;
	}
  }

  // update database
  int rc = main_->GuildDBC()->ExpireGoal(player_->PlayerID(), gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::EXPIRE_GOAL_ERROR, goalid);
    return;
  }
  SECLOG(-7, _T("%s: player %u: expired goal %u"), method, player_->PlayerID(), goalid);
  // goal expired
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::EXPIRE_GOAL_ACK, goalid);
}

////
// handle_GMsg_Goal_CompleteGoal
////

void GsPlayerThread::handle_GMsg_Goal_CompleteGoal(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_CompleteGoal);
  lyra_id_t goalid = gmsg.ID();
  TLOG_Debug(_T("%s: player completing goal %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::COMPLETE_GOAL_ERROR) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player can complete goal (must be goal creator)
  if (player_->PlayerID() != goalinfo.CreatorID()) {
    SECLOG(7, _T("%s: player %u: not allowed to complete goal %u"), method, player_->PlayerID(), goalid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_GOAL_ERROR, goalid);
    return;
  }
  // update database
  int rc = main_->GuildDBC()->CompleteGoal(player_->PlayerID(), gmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::COMPLETE_GOAL_ERROR, goalid);
    return;
  }
  SECLOG(-7, _T("%s: player %u: marked goal %u as complete"), method, player_->PlayerID(), goalid);
  // goal completed
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_GOAL_ACK, goalid);
}

////
// handle_GMsg_Goal_CompleteQuest
////

void GsPlayerThread::handle_GMsg_Goal_CompleteQuest(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_CompleteQuest);
  lyra_id_t goalid = gmsg.ID();
//  TLOG_Debug(_T("%s: player completing quest %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::COMPLETE_GOAL_ERROR) < 0) {
    return;
  }
  // get from database
  GMsg_RcvGoalDetails detailmsg;
  int rc = main_->GuildDBC()->GetGoalDetails(goalid, detailmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
	send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_QUEST_ERROR, goalid);
    return;
  }

  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player can complete quest (must have accepted it, have item in inventory)

  // Quest must be in the goalbook to be completed
  if (!player_->DB().GoalBook().HasMember(goalid)) { 
	SECLOG(-7, _T("%s: player %u: trying to complete unaccepted quest %u"), method, player_->PlayerID(), goalid);
	send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_QUEST_ERROR, goalid);
	return;
  }
	

  // Check that item is in inventory
  const LmInventory& inv = player_->DB().Inventory();
  bool completed = false;
  for (int i = 0; i< inv.NumItems(); ++i) {
	LmItem item = inv.ItemByIndex(i);
	TLOG_Debug(_T("checking item %d, i = %d"), item.Serial(), i);
	if (player_->CanCompleteQuest(item, detailmsg))
		completed = true;	
  }

  if (!completed )
  {
	send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_QUEST_ERROR, goalid);
	return;
  }

  adjust_xp(detailmsg.QuestXP(), _T("Completed quest"), goalid, true);
  main_->GuildDBC()->CompleteQuest(player_->PlayerID(), goalid);

  // update database
  SECLOG(-7, _T("%s: player %u: completed talisman quest %u; gained %u XP"), method, player_->PlayerID(), detailmsg.QuestXP(), goalid);
  // goal completed
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::COMPLETE_QUEST_ACK, goalid);
}


////
// handle_GMsg_Goal_DoesHaveCodex
////

void GsPlayerThread::handle_GMsg_Goal_DoesHaveCodex(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_DoesHaveCodex);
  lyra_id_t goalid = gmsg.ID();
  //TLOG_Debug(_T("%s: player completing quest %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::DOES_HAVE_CODEX_ERROR) < 0) {
    return;
  }
  // get from database
  GMsg_RcvGoalDetails detailmsg;
  int rc = main_->GuildDBC()->GetGoalDetails(goalid, detailmsg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
	send_GMsg_Goal(player_->Connection(), (int)GMsg_Goal::DOES_HAVE_CODEX_ERROR, (lyra_id_t)0);
    return;
  }
	
  // Check scrolls in inventory for match
  //const LmInventory& inv = ;
  bool completed = false;
  LmItem item;
  TCHAR buffer[Lyra::QUEST_KEYWORDS_LENGTH];
  for (int i = 0; i< player_->DB().Inventory().NumItems(); ++i) {
	item = player_->DB().Inventory().ItemByIndex(i);

	const void* state = item.StateField(0);
	int function = (*((unsigned char*)state));
	if (function == LyraItem::SCROLL_FUNCTION)
	{
		lyra_item_scroll_t scroll;
		state = item.StateField(0);
		memcpy(&scroll, state, sizeof(scroll));

		// scrolls we write can't satisfy this quest
		if (scroll.creatorid() == player_->DB().PlayerID())
			continue;

		main_->ItemDBC()->GetItemDescription(item.Header().Serial(), buffer);

		TCHAR* keyword;
		TCHAR* descrip = (TCHAR*)detailmsg.Keywords();

		completed = true;
		keyword = _tcstok(descrip, _T(" "));
		while (keyword != NULL)
		{	
			if (NULL == _tcsstr(buffer, keyword))
			{
				completed = false;
				break;
			}
			keyword = _tcstok(NULL, _T(" "));
		}
	 }

	if (completed == true)
		break;
  }

  if (!completed)
	send_GMsg_Goal(player_->Connection(), GMsg_Goal::DOES_HAVE_CODEX_ERROR, goalid);
  else {  // has completed it
	adjust_xp(detailmsg.QuestXP(), _T("Completed quest"), goalid, true);
	main_->GuildDBC()->CompleteQuest(player_->PlayerID(), goalid);
	// update database
	SECLOG(-7, _T("%s: player %u: completed codex quest %u; gained %u XP"), method, player_->PlayerID(), detailmsg.QuestXP(), goalid);
	send_GMsg_Goal(player_->Connection(), GMsg_Goal::DOES_HAVE_CODEX_ACK, goalid);
  }
}



////
// handle_GMsg_Goal_GetGoalText
////

void GsPlayerThread::handle_GMsg_Goal_GetGoalText(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_GetGoalText);
  lyra_id_t goalid = gmsg.ID();
  TLOG_Debug(_T("%s: player getting goal text for goal %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::GOAL_NOTFOUND) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check if player can read goal text
  if (!player_->CanGetGoalText(goalinfo)) {
    SECLOG(7, _T("%s: player %u: not allowed to read goal %u text"), method, player_->PlayerID(), goalid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::GOAL_NOTFOUND, goalid);
    return;
  }
  // get goal text from database
  GMsg_RcvGoalText msg;
  int rc = main_->GuildDBC()->GetGoalText(goalid, msg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::GOAL_NOTFOUND, goalid);
    return;
  }
  // fill in creator name
  const TCHAR* creator_name = main_->PlayerNameMap()->PlayerName(msg.CreatorID());
  if (!creator_name) {
    TLOG_Warning(_T("%s: goal %u creator %u(%d) not in player database"), method, goalid, msg.CreatorID(), msg.CreatorID());
	// *** STRING LITERAL ***
    creator_name = _T("(unknown)");
  }
  msg.SetCreator(creator_name);
  // send to player
  //TLOG_Debug(_T("%s: result message:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// handle_GMsg_Goal_GetGoalDetails
////

void GsPlayerThread::handle_GMsg_Goal_GetGoalDetails(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_GetGoalDetails);
  lyra_id_t goalid = gmsg.ID();
  TLOG_Debug(_T("%s: player getting goal details for goal %u"), method, goalid);
  // get goal information
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, goalid, GMsg_Goal::GOAL_NOTFOUND) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check that player can read goal details
  if (!player_->CanGetGoalDetails(goalinfo)) {
    SECLOG(7, _T("%s: player %u: not allowed to read goal %u details"), method, player_->PlayerID(), goalid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::GOAL_NOTFOUND, goalid);
    return;
  }
  // get from database
  GMsg_RcvGoalDetails msg;
  int rc = main_->GuildDBC()->GetGoalDetails(goalid, msg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::GOAL_NOTFOUND, goalid);
    return;
  }
  // msg.Dump(((LmLogFile*)Log())->Stream());
  int num_acceptees = msg.NumAcceptees();
  // voting goal?
  if ((msg.StatusFlags() >= Guild::GOAL_PENDING_VOTE) && (msg.StatusFlags() <= Guild::GOAL_RULER_FAILED)) {
    // voting goal, fill in acceptee list with list of voters, not acceptees
    num_acceptees = msg.NumberYes() + msg.NumberNo();
    TLOG_Debug(_T("%s: goal is a voting goal, voters=%d"), method, num_acceptees);
  }
  if (num_acceptees < 0) {
    num_acceptees = 0;
  }
  msg.SetNumAcceptees(num_acceptees);
  // fill in acceptee list
  for (int i = 0; i < num_acceptees; ++i) {
    const TCHAR* acc_name = main_->PlayerNameMap()->PlayerName(msg.AccepteeID(i));
    if (!acc_name) {
      TLOG_Warning(_T("%s: goal %u acceptee/voter %u not in player database"), method, goalid, msg.AccepteeID(i));
	// *** STRING LITERAL ***
      acc_name = _T("(unknown)");
    }
    msg.SetAcceptee(i, acc_name);
  }
  // send to player
  // msg.Dump(((LmLogFile*)Log())->Stream());
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// handle_GMsg_Goal_GetReportText
////

void GsPlayerThread::handle_GMsg_Goal_GetReportText(const GMsg_Goal& gmsg)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_GetReportText);
  lyra_id_t reportid = gmsg.ID();
  TLOG_Debug(_T("%s: player getting report text for report %u"), method, reportid);
  // get from database
  GMsg_RcvReportText msg;
  int rc = main_->GuildDBC()->GetReportText(reportid, msg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    guild_error(rc, sqlcode, GMsg_Goal::REPORT_NOTFOUND, reportid);
    return;
  }
#if 0
  // NOTE: no longer valid with guardian-managed goals
  // check if player is creator or recipient
  if ((msg.RecipientID() != player_->PlayerID()) && (msg.CreatorID() != player_->PlayerID())) {
    SECLOG(7, _T("%s: player %u: not allowed to read report %u text"), method, player_->PlayerID(), reportid);
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::REPORT_NOTFOUND, reportid);
    return;
  }
#endif
  // fill in creator/recipient names
  const TCHAR* creator_name = main_->PlayerNameMap()->PlayerName(msg.CreatorID());
  if (!creator_name) {
    TLOG_Warning(_T("%s: report %u creator %u not in player database"), method, reportid, msg.CreatorID());
	// *** STRING LITERAL ***
    creator_name = _T("(unknown)");
  }
  msg.SetCreator(creator_name);
  const TCHAR* rec_name = main_->PlayerNameMap()->PlayerName(msg.RecipientID());
  if (!rec_name) {
    TLOG_Warning(_T("%s: report %u target %u not in player database"), method, reportid, msg.RecipientID());
	// *** STRING LITERAL ***
    rec_name = _T("(unknown)");
  }
  msg.SetRecipient(rec_name);
  // send to player
  //TLOG_Debug(_T("%s: message dump:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
  // special handling for when report recipient is reading - remove xp, mark as read
  if (player_->PlayerID() == msg.RecipientID()) {
    // if there was XP awarded, add to player's XP and remove from database (in opposite order)
    if (msg.AwardXP() > 0) {
      // check if player is 1 xp away from next sphere, don't award xp in that case
      if (!player_->DB().PeggedBelowSphere()) { 
	// remove xp from report
	rc = main_->GuildDBC()->RemoveReportXP(reportid);
	sqlcode = main_->GuildDBC()->LastSQLCode();
	if (rc < 0) {
	  TLOG_Error(_T("%s: could not remove xp from report %u; rc=%d, sc=%d"), method, reportid, rc, sqlcode);
	  return;
	}
	// give to player, update database, etc.
	adjust_xp(msg.AwardXP(), _T("report from player"), msg.CreatorID(), true);
      }
    }
    // if report has not been flagged as read, and it's recipient is doing the reading, mark it
    if (!(msg.Flags() & Guild::REPORT_READ)) {
      int new_flags = msg.Flags() | Guild::REPORT_READ;
      rc = main_->GuildDBC()->SetReportFlags(reportid, new_flags);
      sqlcode = main_->GuildDBC()->LastSQLCode();
      if (rc < 0) {
	TLOG_Error(_T("%s: could not set flags for report %u; rc=%d, sc=%d"), method, reportid, rc, sqlcode);
      }
    }
  } // end: recipient matches playerid
}

////
// handle_GMsg_Goal_GetGoalbookHeaders
////

void GsPlayerThread::handle_GMsg_Goal_GetGoalbookHeaders(const GMsg_Goal& /* gmsg */)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_Goal_GetGoalbookHeaders);
  TLOG_Debug(_T("%s: player getting goalbook headers"), method);
  // for each goal in the goalbook, get the data from the database
  LmIdSet goalbk = player_->DB().GoalBook();
  TLOG_Debug(_T("%s: player has %d goals in goalbook"), method, goalbk.Size());
  for (int i = 0; i < goalbk.Size(); ++i) {
    lyra_id_t goalid = goalbk.Member(i);
    TLOG_Debug(_T("%s: getting goalbook header for goal %u"), method, goalid);
    GMsg_RcvGoalbookHdr msg;
    // get from database
    int rc = main_->GuildDBC()->GetGoalbookHeader(goalid, msg);
    int sqlcode = main_->GuildDBC()->LastSQLCode();
    if (rc < 0) {
      // if data error, goal not found, so remove from goalbook and continue
      if (rc == LmGuildDBC::MYSQL_NODATA) {
	player_->RemoveGoal(goalid);
      }
      guild_error(rc, sqlcode, -1, goalid); // don't send error status unless DB_UNAVAILABLE
      //return; // continue to next goal
    }
    else {
      //TLOG_Debug(_T("%s: returned goalbook header:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
      main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
    }
  }
}

////
// handle_GMsg_GetGoalHdrs
////

void GsPlayerThread::handle_GMsg_GetGoalHdrs(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetGoalHdrs);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetGoalHdrs, true); // send error
  // process
  //TLOG_Debug(_T("%s: message dump:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // check if player is allowed to request the goal headers
  if (!player_->CanGetGoalHeaders(msg.Guild(), msg.LevelNum())) {
    SECLOG(7, _T("%s: player %u: not allowed to get headers; guild=%d rank=%d"), method,
	   player_->PlayerID(), msg.Guild(), msg.LevelNum());
    return;
  }
  // declare vars
  int rc, sqlcode;
  unsigned goal_id[10];
  lyra_guild_sum goal_summary[10];
  unsigned goal_status[10];
  int goal_playeroption[10];


  // determine if requester is ruler, or  higher rank than the goal
  int hirank = 0;
  // if ruler, pass hirank = 2
  int player_rank = player_->DB().Stats().GuildRank(msg.Guild());
  if (player_rank == Guild::RULER)
    hirank = 2;
  else if (player_rank > msg.LevelNum()) 
    hirank = 1;


  // init status for non-hirank goals
  { 
    for (int i = 0; i < 10; ++i) {
      goal_status[i] = Guild::GOAL_ACTIVE;
      goal_playeroption[i] = 0;
    }
  }
  // check for hirank or not
  if (msg.LevelNum() < player_->DB().Stats().GuildRank(msg.Guild())) {
    // requesting from lower-rank goal posting board, send hirank
    // read from database
    rc = main_->GuildDBC()->HirankGoalHeaders(msg, player_->PlayerID(), goal_id, goal_summary, goal_status, hirank);
    sqlcode = main_->GuildDBC()->LastSQLCode();
    if (rc < 0) {
      TLOG_Warning(_T("%s: could not get hirank goal headers; rc=%d sql=%d"), method, rc, sqlcode);
      guild_error(rc, sqlcode, -1, -1);
      // TODO: error message?
      return;
    }
  }
  else {
    // requesting from equal-rank goal posting board, send regular headers
    int sphere = player_->DB().Stats().Sphere(); // TODO: fill this in correctly
    int focus_stat = player_->DB().Stats().FocusStat();
    // read from database
    rc = main_->GuildDBC()->GoalHeaders(msg, sphere, focus_stat, goal_id, goal_summary, 
		player_->PlayerID(), player_->DB().GoalBook());
    sqlcode = main_->GuildDBC()->LastSQLCode();
    if (rc < 0) {
      TLOG_Warning(_T("%s: could not get goal headers; rc=%d sql=%d"), method, rc, sqlcode);
      guild_error(rc, sqlcode, -1, -1);
      // TODO: error message?
      return;
    }
  }
  TLOG_Debug(_T("%s: database returned %d goal headers"), method, rc);
  // for each returned goal, copy into RcvGoalHdr message, send
  int i = 0;
  for (i = 0; i < rc; ++i) {
    TLOG_Debug(_T("%s: sending session %d, goal %u - '%s'"), method, msg.SessionID(), goal_id[i], goal_summary[i]);
    // if goal status indicates a voting goal, then check if player was in voters
    if ((goal_status[i] >= Guild::GOAL_PENDING_VOTE) && (goal_status[i] <= Guild::GOAL_RULER_FAILED)) {
      int rc2 = main_->GuildDBC()->InVoters(goal_id[i], player_->DB().PlayerID());
      sqlcode = main_->GuildDBC()->LastSQLCode();
      if (rc2 < 0) {
	TLOG_Warning(_T("%s: could not get voter info; rc=%d sql=%d"), method, rc2, sqlcode);
	// ignore errors
	rc2 = 0;
      }
      goal_playeroption[i] = rc2;
    }
    send_GMsg_RcvGoalHdr(player_->Connection(), msg.SessionID(), goal_id[i], goal_summary[i], goal_status[i], goal_playeroption[i]);
    //  TLOG_Debug(_T("%s: sent rcv goal header for goal %d"), method, goal_id[i]);
  }

  lyra_id_t goal_id_dgh[10];
  rc = main_->GuildDBC()->DetailGoalHeaders(msg.Guild(), msg.LevelNum(), msg.LastGoal(), player_->PlayerID(), goal_id_dgh, hirank);
  sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    TLOG_Warning(_T("%s: could not get report goals; rc=%d sql=%d"), method, rc, sqlcode);
    guild_error(rc, sqlcode, -1, -1);
    return;
  }
  
  TLOG_Debug(_T("%s: database returned %d report headers"), method, rc);
  // for each returned report, copy into RcvReportGoals message, send
  for (i = 0; i < rc; ++i) {
    send_GMsg_RcvReportGoals(player_->Connection(), goal_id_dgh[i]);
  }

  
}

////
// handle_GMsg_GetReportHdrs
////

void GsPlayerThread::handle_GMsg_GetReportHdrs(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_GetReportHdrs);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_GetReportHdrs, true); // send error
  // process
  //TLOG_Debug(_T("%s: message dump:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // if goalid is 0, means "get all unread reports", so only do this when it's not zero
  LmGoalInfo goalinfo;
  if (msg.GoalID() != 0) {
    // get goal information
    if (get_goalinfo(goalinfo, msg.GoalID(), GMsg_Goal::GOAL_NOTFOUND) < 0) {
      return;
    }
    //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
    // check that player can get report headers for this goal
    if (!player_->CanGetReportHeaders(goalinfo)) {
      SECLOG(7, _T("%s: player %u: not allowed to read goal %u reports"), method, player_->PlayerID(), msg.GoalID());
      send_GMsg_Goal(player_->Connection(), GMsg_Goal::GOAL_NOTFOUND, msg.GoalID());
      return;
    }
  }
  // declare vars
  unsigned report_id[10];
  unsigned goal_id[10];
  unsigned short flags[10];
  lyra_guild_sum report_summary[10];
  // determine if requester is ruler, or  higher rank than the goal
  int hirank = 0;
  // if ruler, pass hirank = 2
  int player_rank = player_->DB().Stats().GuildRank(msg.Guild());
  if (player_rank == Guild::RULER)
    hirank = 2;
  else if (player_rank > msg.LevelNum()) 
    hirank = 1;

  // read from database
  int rc = main_->GuildDBC()->ReportHeaders(msg, player_->PlayerID(), report_id, goal_id, report_summary, flags, hirank);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    TLOG_Warning(_T("%s: could not get report headers; rc=%d sql=%d"), method, rc, sqlcode);
    guild_error(rc, sqlcode, -1, -1);
    return;
  }
  TLOG_Debug(_T("%s: database returned %d report headers"), method, rc);
  // for each returned report, copy into RcvReportHdr message, send
  int i = 0;
  for (i = 0; i < rc; ++i) {
    // TLOG_Debug(_T("%s: sending session %d, report %u - '%s'"), method, msg.SessionID(), report_id[i], report_summary[i]);
    send_GMsg_RcvReportHdr(player_->Connection(), msg.SessionID(), report_id[i], goal_id[i], report_summary[i], flags[i]);
  }

  // now get the list of all goal headers we can detail or read reports on
  if ((hirank > 0) && (msg.GoalID() == 0)) {
    int rc = main_->GuildDBC()->DetailGoalHeaders(msg.Guild(), msg.LevelNum(), msg.LastReport(), player_->PlayerID(), goal_id, hirank);
    int sqlcode = main_->GuildDBC()->LastSQLCode();
    if (rc < 0) {
      TLOG_Warning(_T("%s: could not get report goals; rc=%d sql=%d"), method, rc, sqlcode);
      guild_error(rc, sqlcode, -1, -1);
      return;
    }
    TLOG_Debug(_T("%s: database returned %d report headers"), method, rc);
    // for each returned report, copy into RcvReportGoals message, send
    for (int i = 0; i < rc; ++i) {
      TLOG_Debug(_T("%s: sending session %d, report %u - '%s'"), method, msg.SessionID(), report_id[i], report_summary[i]);
      send_GMsg_RcvReportGoals(player_->Connection(), goal_id[i]);
    }
  }
}



////
// handle_GMsg_PostGoal
////

void GsPlayerThread::handle_GMsg_PostGoal(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_PostGoal);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_PostGoal, true); // send error
  // process
  int rc, sc;
//  TLOG_Debug(_T("%s: processing goal posting message"), method);
  //Log()->FlushLog();

	// log new quest posts
  //case RMsg_Speech::REPORT_QUEST: // eliminated unnecessary double sending of text
//    msg.RemoveNewlines();
  if (msg.Level() == Guild::QUEST) {
    SECLOG(-2, _T("%s: player %u posted quest; stat: %d sphere: %d summary: %s text: %s"), 
		method, player_->PlayerID(), msg.SugStat(), msg.SugSphere(), msg.Summary(), msg.GoalText());
	   //msg.SpeechText());
  }

  // if goalid != 0, this is an update message
  if (msg.GoalID() != Lyra::ID_UNKNOWN) {
    // get goal information
    LmGoalInfo goalinfo;
    if (get_goalinfo(goalinfo, msg.GoalID(), GMsg_Goal::GOAL_NOTFOUND) < 0) {
      return;
    }
//	TLOG_Debug(_T("%s: checking to see if player %u can update goal %u"), method, player_->PlayerID(), msg.GoalID());
//    Log()->FlushLog();
    // check if player can update this goal
    if (!player_->CanUpdateGoal(goalinfo)) {
      SECLOG(7, _T("%s: player %u: not allowed to update mission %u"), method,
	     player_->PlayerID(), msg.GoalID());
      send_GMsg_Goal(player_->Connection(), GMsg_Goal::GOAL_NOTFOUND, 0);
      return;
    }
    // update goal in db
	//TLOG_Debug(_T("%s: player %u updating goal %u in database"), method, player_->PlayerID(), msg.GoalID());
    //Log()->FlushLog();
    rc = main_->GuildDBC()->UpdateMission(player_->PlayerID(), msg);
    sc = main_->GuildDBC()->LastSQLCode();
    if (rc < 0) {
      TLOG_Error(_T("%s: could not update mission; rc=%d sqlcode=%d"), method, rc, sc);
      guild_error(rc, sc, GMsg_Goal::GOAL_NOTFOUND, 0);
      return;
    }
	//TLOG_Debug(_T("%s: player %u updated goal %u in database"), method, player_->PlayerID(), msg.GoalID());
    //Log()->FlushLog();
  }
  else { // posting a new one
    // check if player can post this goal
	//TLOG_Debug(_T("%s: checking if player %u can create new goal; guild %u, level %u"), method, 
	//	msg.Guild(), msg.Level());
    //Log()->FlushLog();
    if (!player_->CanPostGoal(msg.Guild(), msg.Level(), msg.QuestXP()*msg.MaxAccepted())) {
      SECLOG(7, _T("%s: player %u: not allowed to post goal; guild=%d rank=%d questxp=%d"), method,
	     player_->PlayerID(), msg.Guild(), msg.Level(), msg.QuestXP());
      send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTGOAL_ERROR, 0);
      return;
    }
    // mission, or goal?
    if ((msg.Level() == Guild::INITIATE) || 
		(msg.Level() == Guild::QUEST))
	{
      rc = main_->GuildDBC()->PostMission(player_->PlayerID(), msg);
      sc = main_->GuildDBC()->LastSQLCode();
      if (rc < 0) {
	TLOG_Error(_T("%s: could not post mission; rc=%d sqlcode=%d"), method, rc, sc);
	guild_error(rc, sc, GMsg_Goal::POSTGOAL_ERROR, 0);
	return;
      }
	//TLOG_Debug(_T("%s: player %u created new initiate goal; guild %u, level %u"), method, 
	//	msg.Guild(), msg.Level());
    //Log()->FlushLog();

    }
    else if (msg.Level() == Guild::KNIGHT) {
      rc = main_->GuildDBC()->PostGoal(player_->PlayerID(), msg);
      sc = main_->GuildDBC()->LastSQLCode();
      if (rc < 0) {
	TLOG_Error(_T("%s: could not post goal; rc=%d sqlcode=%d"), method, rc, sc);
	guild_error(rc, sc, GMsg_Goal::POSTGOAL_ERROR, 0);
	return;
      }
	//	TLOG_Debug(_T("%s: player %u created new knight goal; guild %u, level %u"), method, 
	//	msg.Guild(), msg.Level());
	//	Log()->FlushLog();
	}
    else {
      TLOG_Error(_T("%s: unknown goal level %d"), method, msg.Level());
      send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTGOAL_ERROR, 0);
      return;
    }
  }
  // goal posted/updated OK
  // TLOG_Debug(_T("%s: player posted goal"), method);
  if (msg.GoalID() != Lyra::ID_UNKNOWN) {
    SECLOG(-7, _T("%s: player %u: posted goal of rank %d in guild %d"), method,
	   player_->PlayerID(), msg.Level(), msg.Guild());
    // send message so client knows it is OK to get details, etc.
    send_GMsg_RcvReportGoals(player_->Connection(), msg.GoalID());
  }
  else {
    SECLOG(-7, _T("%s: player %u: updated goal %u"), method, player_->PlayerID(), msg.GoalID());
  }

  // if Quest XP awarded, remove from pool
  if (msg.QuestXP() > 0) {
    player_->PostQuest(msg.QuestXP()*msg.MaxAccepted());
    // int rc = main_->PlayerDBC()->AddOfflineXP(recipient, msg.AwardXP());
  }

  send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTGOAL_ACK, 0);

}

////
// handle_GMsg_PostReport
////

void GsPlayerThread::handle_GMsg_PostReport(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_GMsg_PostReport);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message
  ACCEPT_MSG(GMsg_PostReport, true); // send error
  // process
  //TLOG_Debug(_T("%s: message dump:"), method); msg.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // get goal info
  LmGoalInfo goalinfo;
  if (get_goalinfo(goalinfo, msg.GoalID(), GMsg_Goal::GOAL_NOTFOUND) < 0) {
    return;
  }
  //goalinfo.Dump(TLOG_Stream(), 1); TLOG_FlushLog();
  // get recipient id from message
  lyra_id_t recipient = main_->PlayerNameMap()->PlayerID(msg.Recipient());
  if (recipient == Lyra::ID_UNKNOWN) {
    TLOG_Warning(_T("%s: report recipient '%s' not in database"), method, msg.Recipient());
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTREPORT_ERROR, 0);
    return;
  }
  // determine if source/target are in acceptee list
  bool source_in_acceptees = (bool)(main_->GuildDBC()->InAcceptees(msg.GoalID(), player_->PlayerID()));
  bool target_in_acceptees = (bool)(main_->GuildDBC()->InAcceptees(msg.GoalID(), recipient));
  // check that player can post this report
  if (!player_->CanPostReport(goalinfo, msg.AwardXP(), source_in_acceptees, recipient, target_in_acceptees)) {
    SECLOG(7, _T("%s: player %u: not allowed to post report to player %u for goal %u, awardxp %d"), method,
	   player_->PlayerID(), recipient, msg.GoalID(), msg.AwardXP());
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTREPORT_ERROR, 0);
    return;
  }
#if 0
  // NOTE: no longer valid with guardian-managed goals
  // further check: that player is either goal creator, or in list of acceptees
  if ((player_->PlayerID() != goalinfo.CreatorID()) && !source_in_acceptees) {
    SECLOG(7, _T("%s: player %u: not goal creator/acceptee for goal %u"), method, player_->PlayerID(), msg.GoalID());
    send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTREPORT_ERROR, 0);
    return;
  }
#endif
  // put in database
  int rc = main_->GuildDBC()->PostReport(player_->PlayerID(), recipient, msg);
  int sqlcode = main_->GuildDBC()->LastSQLCode();
  if (rc < 0) {
    TLOG_Error(_T("%s: could not post report; rc=%d sqlcode=%d"), method, rc, sqlcode);
    guild_error(rc, sqlcode, GMsg_Goal::POSTREPORT_ERROR, 0);
    return;
  }
  // if XP awarded, remove from pool
  if (msg.AwardXP() > 0) {
    player_->PostReport(goalinfo.Guild(), msg.AwardXP(), recipient);
    // int rc = main_->PlayerDBC()->AddOfflineXP(recipient, msg.AwardXP());
  }
  // report posted
  send_GMsg_Goal(player_->Connection(), GMsg_Goal::POSTREPORT_ACK, 0);
  // give xp to report poster?
  if (!source_in_acceptees && target_in_acceptees) {
    int xp = (msg.AwardXP() > 0) ? 500 : 100; // 100 if no xp, 500 if some
    adjust_xp(xp, _T("posting report to player"), recipient, true);
  }
}
