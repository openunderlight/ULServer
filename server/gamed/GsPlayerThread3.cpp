// GsPlayerThread3.cpp  -*- C++ -*-
// $Id: GsPlayerThread3.cpp,v 1.32 1998/04/17 02:08:10 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// handle_SMsg_* methods

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
#include "LmPlayerDBC.h"
#include "LmBillingDBC.h"

////
// handle_SMsg_PartyLeader
////

void GsPlayerThread::handle_SMsg_PartyLeader(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_PartyLeader);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_PartyLeader, true); // send error
  // process
  int leader_time = msg.LeaderTime(); // in seconds
  int xp_gain = leader_time; // 1 xp/sec of being leader
  // check if they are a knight/ruler in any house
  bool gets_xp = false;
  for (int guild = 0; guild < NUM_GUILDS; ++guild) {
    if (player_->DB().Stats().GuildRank(guild) >= Guild::KNIGHT) {
      gets_xp = true;
      break;
    }
  }
  // check if they are a teacher
  if ((player_->DB().Arts().Skill(Arts::TRAIN) > 0) || (player_->DB().Arts().Skill(Arts::LEVELTRAIN) > 0)) {
    gets_xp = true;
  }
  TLOG_Debug(_T("%s: gets_xp=%d xp_gain=%d"), method, gets_xp, xp_gain);
  if (xp_gain && gets_xp) {
    adjust_xp(xp_gain, _T("party leadership"), 0, true);
  }
}

////
// handle_SMsg_LocateAvatar
////

void GsPlayerThread::handle_SMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_LocateAvatar);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_LocateAvatar, true); // send error
  // process
  const TCHAR* pname = main_->PlayerNameMap()->PlayerName(msg.PlayerID());
  if (!pname) {
    TLOG_Warning(_T("%s: player %u not found?"), method, msg.PlayerID());
    return;
  }
  // check if player was found in level
   if (msg.RoomID() == Lyra::ID_UNKNOWN) { // not found
    perform_locateavatar(msg.PlayerID(), pname);
    return;
     }
  bool gm = (player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN);
  // return status
  int status = GMsg_LocateAvatarAck::LOCATE_FOUND;
  lyra_id_t levelid = conn->ID();
  lyra_id_t roomid = msg.RoomID();
  // player hidden?
  bool hidden = false;
  if (roomid > Lyra::HIDDEN_DELTA) {
    hidden = true;
    roomid -= Lyra::HIDDEN_DELTA;
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

  // player was found in level, copy info into return message
  send_GMsg_LocateAvatarAck(player_->Connection(), pname, status, levelid, roomid);
}

////
// handle_SMsg_ItemDrop
////

void GsPlayerThread::handle_SMsg_ItemDrop(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_ItemDrop);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_ItemDrop, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check return code from level server
  if (msg.Status() != SMsg_ItemDrop::DROP_OK) {
    TLOG_Warning(_T("%s: player could not drop item %u"), method, hdr.Serial());
    player_->EndItemDrop(hdr, false); // didn't drop it
    send_GMsg_ItemDrop(player_->Connection(), hdr, GMsg_ItemDrop::DROP_ERROR);
  }
  // item was dropped in room, database was updated
  player_->EndItemDrop(hdr, true);
  // send ack to player
  send_GMsg_ItemDrop(player_->Connection(), hdr, GMsg_ItemDrop::DROP_OK);
}

////
// handle_SMsg_ItemPickup
////

void GsPlayerThread::handle_SMsg_ItemPickup(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_ItemPickup);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_ItemPickup, true); // send error
  // process
  LmItem item = msg.Item();
  // check return code from level server
  if (msg.Status() != SMsg_ItemPickup::PICKUP_OK) {
    TLOG_Warning(_T("%s: player could not pick up item %u"), method, item.Header().Serial());
    player_->EndItemPickup(item, false); // didn't get it
    send_GMsg_ItemPickup(player_->Connection(), item, GMsg_ItemPickup::PICKUP_ERROR);
    return;
  }
  // item was picked up and database was updated
  player_->EndItemPickup(item, true);
  // send ack to player
  send_GMsg_ItemPickup(player_->Connection(), item, GMsg_ItemPickup::PICKUP_OK);
}

////
// handle_SMsg_GiveItem
////

void GsPlayerThread::handle_SMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_GiveItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_GiveItem, true); // send error
  // check if item would fit in player's inventory
  if (!player_->CanAddItem(msg.Item().Header())) {
    TLOG_Debug(_T("%s: cannot take item %d from player %u"), method, msg.Item().Serial(), msg.SourceID());
    // send ack back to source, through level server
    send_SMsg_TakeItemAck(conn, msg.SourceID(), msg.Item().Header(), GMsg_TakeItemAck::TAKE_NO);
    return;
  }
  TLOG_Debug(_T("%s: taking item %d from player %u"), method, msg.Item().Serial(), msg.SourceID());
  // provisionally take item
  player_->StartItemTake(msg.SourceID(), msg.Item());
  // send takeitem to player
  send_GMsg_TakeItem(player_->Connection(), msg.SourceID(), msg.Item());
  // response: handle_GMsg_TakeItemAck
}

////
// handle_SMsg_ShowItem
////

void GsPlayerThread::handle_SMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_ShowItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_ShowItem, true); // send error
  // send viewitem message to player
  send_GMsg_ViewItem(player_->Connection(), msg.SourceID(), msg.ItemHeader(), msg.ItemName());
}

////
// handle_SMsg_GiveItemAck
////

void GsPlayerThread::handle_SMsg_GiveItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_GiveItemAck);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_GiveItemAck, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check return code from level server
  switch (msg.Status()) {
  case GMsg_GiveItemAck::GIVE_YES:
    player_->EndItemDrop(hdr, true); // dropped it
    send_GMsg_GiveItemAck(player_->Connection(), hdr, GMsg_GiveItemAck::GIVE_YES);
    break;
  case GMsg_GiveItemAck::GIVE_NO:
    player_->EndItemDrop(hdr, false); // didn't drop it
    send_GMsg_GiveItemAck(player_->Connection(), hdr, GMsg_GiveItemAck::GIVE_NO);
    break;
  default:
    TLOG_Warning(_T("%s: unknown status '%c'"), method, msg.Status());
    break;
  }
  TLOG_Debug(_T("%s: gave item %d to player %u: %c"), method, hdr.Serial(), msg.SourceID(), msg.Status());
}

////
// handle_SMsg_TakeItemAck
////

void GsPlayerThread::handle_SMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_TakeItemAck);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_TakeItemAck, true); // send error
  // process
  LmItemHdr hdr = msg.ItemHeader();
  // check return code from level server (we only get this if we wanted to take the item)
  switch (msg.Status()) {
  case GMsg_TakeItemAck::TAKE_OK:
    player_->EndItemTake(hdr, true); // got it
    send_GMsg_TakeItemAck(player_->Connection(), hdr, GMsg_TakeItemAck::TAKE_OK);
    break;
  case GMsg_TakeItemAck::TAKE_ERROR:
    player_->EndItemTake(hdr, false); // didn't take it
    send_GMsg_TakeItemAck(player_->Connection(), hdr, GMsg_TakeItemAck::TAKE_ERROR);
    break;
  default:
    TLOG_Warning(_T("%s: unknown status '%c'"), method, msg.Status());
    break;
  }
  TLOG_Debug(_T("%s: took item %d from player %u: %c"), method, hdr.Serial(), msg.SourceID(), msg.Status());
}

////
// handle_SMsg_GS_Login
////

void GsPlayerThread::handle_SMsg_GS_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_GS_Login);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  CHECK_PLAYER_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Login, false); // don't send error
  lyra_id_t playerid = msg.PlayerID();
  TLOG_Debug(_T("%s: playerid=%u"), method, playerid);
  // get player object
  GsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    main_->Log()->Error(_T("GsPlayerThread::%s: could not get player %u!"), method, playerid);
    return;
  }
  // open log file, initialize
  Login(player);

  // read in message of the day, send to player
  send_motd();

  // make offline xp adjustments
    int xpdelta = player->DB().XPGained() - player->DB().XPLost();
  // TLOG_Debug(_T("%s: player gained %d xp, lost %d xp offline"), method, player->DB().XPGained(), player->DB().XPLost());
    if (xpdelta != 0) {
    adjust_offline_xp(0, _T("reset"), 0, true);
    if ((xpdelta < 0) || (!player->DB().PeggedBelowSphere())) { // if negative, or player can gain xp
      adjust_xp(xpdelta, _T("house activities"), 0, true);
    }
    else { // xpdelta is positive, but player cannot gain, do not grant it; put back into database
      int rc = main_->PlayerDBC()->AddOfflineXP(playerid, xpdelta);
      int sc = main_->PlayerDBC()->LastSQLCode();
      if (rc < 0) {
	GsUtil::HandlePlayerError(main_, method, rc, sc, false);
      }
    }
  }
  
}

////
// handle_SMsg_GS_Action
////

void GsPlayerThread::handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_GS_Action);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_GS_Action, false); // don't send error
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  // switch on action to perform
  switch (msg.Action()) {
  case SMsg_GS_Action::ACTION_EXIT:
    // log player out
    if (player_) {
      GMsg_Logout msg_logout;
      msg_logout.Init(GMsg_Logout::LOGOUT_SERVERDOWN, 0);
      perform_logout(msg_logout);
    }
    TLOG_Log(_T("%s: shutting down"), method);
    // signal end of thread's main loop
    SetDone();
    break;
  case SMsg_GS_Action::ACTION_HEARTBEAT:
    if (player_) {
      TLOG_Log(_T("%s: player %u thread running"), method, player_->PlayerID());
    }
    else {
      main_->Log()->Error(_T("GsPlayerThread::%s: received heartbeat message, player is null"), method);
      // remove self from thread pool, don't delete, put back into spare thread set
      TLOG_Debug(_T("%s: removing self from thread pool for player due to heartbeat but null player"), method);

      main_->ThreadPool()->RemoveThread(this, false);
      main_->PlayerThreadSet()->ReturnPlayerThread(this);
    }
    break;
    //  case SMsg_GS_Action::ACTION_ROTATELOG:
    //    if (player_) {
    //      TLOG_Log(_T("%s: rotating log"), method);
    //      close_log();
    //      open_log();
    //}
    //else {
    //main_->Log()->Error(_T("GsPlayerThread::%s: received logrotate message, player is null"), method);
      // remove self from thread pool, don't delete, put back into spare thread set
    //main_->ThreadPool()->RemoveThread(this, false);
    //main_->PlayerThreadSet()->ReturnPlayerThread(this);
    //}
    //break;
  case SMsg_GS_Action::ACTION_SAVE_PLAYER:
    if (player_) {
      num_save_sigs_++;
      // save to database or disk, but not both
      if ((num_save_sigs_ % SAVE_DB) == 0) {
	    // TLOG_Debug(_T("%s: saving to database"), method);
		player_->SaveToDB(false);
      } //else if ((num_save_sigs_ % SAVE_DISK) == 0) { // deprecated - BMP 6/01
	    // TLOG_Debug(_T("%s: saving to file"), method);
      	//player_->SaveToFile();
      //}
    }
    else {
      main_->Log()->Error(_T("GsPlayerThread::%s: received saveplayer message, player is null"), method);
      // remove self from thread pool, don't delete, put back into spare thread set
      TLOG_Debug(_T("%s: removing self from thread pool  due to save player but null player"), method);

      main_->ThreadPool()->RemoveThread(this, false);
      main_->PlayerThreadSet()->ReturnPlayerThread(this);
    }
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %d"), method, msg.Action());
    break;
  }
}

////
// handle_SMsg_Proxy
////

void GsPlayerThread::handle_SMsg_Proxy(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_Proxy);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISLEVEL();
  CHECK_PLAYER_NONNULL();
  // accept message
  ACCEPT_MSG(SMsg_Proxy, true); // send error
  // process
  TLOG_Debug(_T("%s: player=%u mtype=%s (%d) msize=%d"), method,
	     msg.PlayerID(), LyraMessage::MessageName(msg.EnclosedMessageType()),
	     msg.EnclosedMessageType(), msg.EnclosedMessageSize());
  // check that message's target is current player
  if (msg.PlayerID() != player_->PlayerID()) {
    TLOG_Error(_T("%s: target player %u not thread player %u"), method, msg.PlayerID(), player_->PlayerID());
    return;
  }
  // get new message buffer
  LmSrvMesgBuf* mbuf = main_->BufferPool()->AllocateBuffer(msg.EnclosedMessageSize());
  // copy message data into buffer (message data is in network order already)
  LmMesgHdr mhdr;
  mhdr.Init(msg.EnclosedMessageType(), msg.EnclosedMessageSize());
  mbuf->ReadHeader(mhdr);
  memcpy(mbuf->MessageAddress(), msg.MessageBytes(), msg.EnclosedMessageSize());
  // handle some enclosed message types (sub-methods will send to player)
  switch (msg.EnclosedMessageType()) {
  case RMsg::PLAYERMSG:
    handle_SMsg_Proxy_RMsg_PlayerMsg(mbuf);
    break;
  default:
    // no specific handling, forward along to player
    main_->OutputDispatch()->SendMessage(mbuf, player_->Connection());
    break;
  }
}

////
// handle_SMsg_Proxy_RMsg_PlayerMsg
////

void GsPlayerThread::handle_SMsg_Proxy_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf)
{
  DEFMETHOD(GsPlayerThread, handle_SMsg_Proxy_RMsg_PlayerMsg);
  PROXY_HANDLER_ENTRY(false);
  // accept message
  PROXY_ACCEPT_MSG(RMsg_PlayerMsg);
  // process
  TLOG_Debug(_T("%s: player %u receiving playermsg %d [%d,%d] from %u"), method,
	     msg.ReceiverID(), msg.MsgType(), msg.State1(), msg.State1(), msg.SenderID());
  // check receiver id (should either be player's id, or 0 for broadcast)
  if ((msg.ReceiverID() != Lyra::ID_UNKNOWN) && (player_->PlayerID() != msg.ReceiverID())) {
    TLOG_Warning(_T("%s: player %u != receiver %u"), method, player_->PlayerID(), msg.ReceiverID());
    // TODO: exit?
  }
  // depending on message type, do various things
  bool send_to_player = true; // default: forward message to player
  switch (msg.MsgType()) {

  //
  // Simple forward along to player, no processing
  //

  case RMsg_PlayerMsg::RESIST_FEAR:         // skill, not used
  case RMsg_PlayerMsg::RESIST_CURSE:        // skill, not used
  case RMsg_PlayerMsg::RESIST_PARALYSIS:    // skill, not used
  case RMsg_PlayerMsg::JUDGEMENT:           // skill, not used
  case RMsg_PlayerMsg::IDENTIFY_CURSE:      // skill, not used
  case RMsg_PlayerMsg::VISION:              // skill, not used
  case RMsg_PlayerMsg::BLAST:               // skill, not used
  case RMsg_PlayerMsg::BLAST_ACK:           // not used, not used
  case RMsg_PlayerMsg::RESTORE:             // skill, not used     
  case RMsg_PlayerMsg::PURIFY:              // skill, not used
  case RMsg_PlayerMsg::DRAIN_SELF:          // stat, amount
  case RMsg_PlayerMsg::ABJURE:              // skill, not used
  case RMsg_PlayerMsg::POISON:              // skill, not used
  case RMsg_PlayerMsg::ANTIDOTE:            // skill, not used     
  case RMsg_PlayerMsg::CURSE:               // skill, not used
  case RMsg_PlayerMsg::ENSLAVE:             // skill, not used         
  case RMsg_PlayerMsg::SCARE:               // skill, not used
  case RMsg_PlayerMsg::STAGGER:             // skill, not used
  case RMsg_PlayerMsg::DEAFEN:              // skill, not used
  case RMsg_PlayerMsg::BLIND:               // skill, not used
  case RMsg_PlayerMsg::DARKNESS:            // skill, not used
  case RMsg_PlayerMsg::PARALYZE:            // skill, not used
  case RMsg_PlayerMsg::RANDOM:              // random number, random number
  case RMsg_PlayerMsg::FIRESTORM:           // skill, not used
  case RMsg_PlayerMsg::RAZORWIND:           // skill, not used
  case RMsg_PlayerMsg::VAMPIRIC_DRAW:       // skill, not used
  case RMsg_PlayerMsg::VAMPIRIC_DRAW_ACK:   // amount, not used
  case RMsg_PlayerMsg::HYPNOTIC_WEAVE:      // skill, not used
  case RMsg_PlayerMsg::EARTHQUAKE:          // skill, not used
  case RMsg_PlayerMsg::TERROR:              // skill, not used     
  case RMsg_PlayerMsg::HEALING_AURA:        // skill, not used     
  case RMsg_PlayerMsg::TRAP_NIGHTMARE:      // skill, not used
  //case RMsg_PlayerMsg::TRIGGER_SOUND:       // sound id, not used
  case RMsg_PlayerMsg::NEWBIE_ENTERED:      // not used, not used
  case RMsg_PlayerMsg::ROGER_WILCO:         // special, special
  case RMsg_PlayerMsg::TRAIN_SELF:          // art, success
  case RMsg_PlayerMsg::SPHERE_REPLY:        // art, success
  case RMsg_PlayerMsg::SOUL_SHIELD:         // skill, not used     
  case RMsg_PlayerMsg::SUMMON:              // not used, not used     
  case RMsg_PlayerMsg::REFLECT_ART:         // art_id, not used
  case RMsg_PlayerMsg::EXPEL:				// not used, not used
  case RMsg_PlayerMsg::RADIANT_BLAZE:		// skill, not used
  case RMsg_PlayerMsg::POISON_CLOUD:		// skill, not used
  case RMsg_PlayerMsg::BREAK_COVENANT:		// skill, not used
  case RMsg_PlayerMsg::PEACE_AURA:			// skill, not used
  case RMsg_PlayerMsg::SABLE_SHIELD:		// skill, not used
  case RMsg_PlayerMsg::ENTRANCEMENT:		// skill, not used
  case RMsg_PlayerMsg::SHADOW_STEP:			// skill, not used
  case RMsg_PlayerMsg::DAZZLE:				// skill, not used
  case RMsg_PlayerMsg::TEHTHUS_OBLIVION_ACK:// unused, unused 
  case RMsg_PlayerMsg::TRAIN_ACK:			// skill, not used
  case RMsg_PlayerMsg::CHAOS_PURGE:			// skill, not used 
  case RMsg_PlayerMsg::CUP_SUMMONS:			// skill, not used 
  case RMsg_PlayerMsg::SCAN:				// skill, not used 
  case RMsg_PlayerMsg::HEAL:				// skill, not used 
  case RMsg_PlayerMsg::SANCTIFY:			// skill, not used 
  case RMsg_PlayerMsg::REMOVE_CURSE:		// skill, not used 
  case RMsg_PlayerMsg::HOLD_AVATAR:			// skill, not used 
  case RMsg_PlayerMsg::TEMPEST:         // skill, angle
  case RMsg_PlayerMsg::KINESIS:         // skill, angle
  case RMsg_PlayerMsg::MISDIRECTION:    // skill, unused
  case RMsg_PlayerMsg::CHAOTIC_VORTEX: // skill, unused
    // do nothing
    break;

  //
  // special handling
  //

  case RMsg_PlayerMsg::GRANT_PPOINT: {         // not used, not used
	  int newpp = player_->DB().Stats().PP()+1;
	  player_->SetPPoints(newpp);
	  // second logging unnecessary
	  //SECLOG(5, _T("%s: player %u: granted a personality point by player %u"), method, player_->PlayerID(), msg.SenderID());
	  //send_RMsg_PlayerMsg_GrantPPoint(msg.SenderID());
	}
	break;


  // player was reduced to dreamsoul
  case RMsg_PlayerMsg::FINGER_OF_DEATH: {   // not used, not used
    // log it
    SECLOG(5, _T("%s: player %u: given FINGER_OF_DEATH by player %u"), method, player_->PlayerID(), msg.SenderID());
  }
  break;

  // player was booted ("awakened")
  case RMsg_PlayerMsg::BOOT: {   // not used, not used
    // log it

	  // BMP DEBUGGING CODE START
#if 0
	GMsg_Goal gmsg;
    gmsg.Init(1, 0);
	main_->GuildDBC()->DeleteReport(2141, gmsg);
    gmsg.Init(2, 0);
	main_->GuildDBC()->RemoveGoal(2141, gmsg);
    gmsg.Init(3, 0);
	main_->GuildDBC()->ExpireGoal(2141, gmsg);
	  // BMP DEBUGGING CODE END
#endif


    SECLOG(5, _T("%s: player %u: BOOTed by player %u"), method, player_->PlayerID(), msg.SenderID());
    GsUtil::FakeLogout(main_, player_);
  }
  break;

  // player was permanently locked out of game
  case RMsg_PlayerMsg::TERMINATE: {         // not used, not used
    // modify player record, set account status to locked out and killer id to person who locked them out
    int rc = main_->PlayerDBC()->LockPlayerOut(player_->PlayerID(), msg.SenderID());
    if (rc < 0) {
      TLOG_Warning(_T("%s: player %u could not TERMINATE player %u and update acct_type"), method, msg.SenderID(), player_->PlayerID());
      GsUtil::HandlePlayerError(main_, method, rc, 0, false);
    } else {
      rc = main_->BillingDBC()->DisablePlayer(player_->PlayerID());
      if (rc < 0) {
	TLOG_Warning(_T("%s: player %u could not TERMINATE player and disable account in billing system %u"), method, msg.SenderID(), player_->PlayerID());
	GsUtil::HandlePlayerError(main_, method, rc, 0, false);
      } else {
	SECLOG(5, _T("%s: player %u: TERMINATEd by player %u"), method, player_->PlayerID(), msg.SenderID());
      }
    }
    // then boot the person from the game
    GsUtil::FakeLogout(main_, player_);
  }
  break;

  // player was temporarily locked out of game
  case RMsg_PlayerMsg::SUSPEND: {         // num_days, not used
    // modify player record, set account status to locked out and killer id to person who locked them out
    int rc = main_->PlayerDBC()->SuspendPlayer(player_->PlayerID(), msg.SenderID(), msg.State1());
    int sc = main_->PlayerDBC()->LastSQLCode();
    if (rc < 0) {
      TLOG_Warning(_T("%s: player %u could not SUSPEND player %u"), method, msg.SenderID(), player_->PlayerID());
      GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    }
    else {
      SECLOG(5, _T("%s: player %u: SUSPENDEd by player %u for %u days"), method, player_->PlayerID(), msg.SenderID(), msg.State1());
    }
    // then boot the person from the game
    GsUtil::FakeLogout(main_, player_);
  }
  break;

  // player was mindblanked
  case RMsg_PlayerMsg::MIND_BLANK_OTHER: {  // skill, not used
    //TLOG_Debug(_T("%s: being hidden by player %u"), method, msg.SenderID());
    //hide_player(1); // hide
  }
  break;

  // player being trained
  case RMsg_PlayerMsg::TRAIN: {             // art_id, teacher_skill (+ 100 if gm train)
    int art = msg.State1();
    int skill = msg.State2();
    if (player_->CanBeTrained(art, skill)) {
      int old_skill = player_->DB().Arts().Skill(art);
      if (player_->ChangeSkill(art, skill, true)) {
		// get actual skill level
		skill = player_->DB().Arts().Skill(art);
		int target = player_->PlayerID();
		int origin = msg.SenderID();
		SECLOG(-4, _T("%s: player %u: trained in art %d, skill %d -> %d, by player %u"), method,
	       target, art, old_skill, skill, origin);
		int rc = main_->PlayerDBC()->LogQuest(origin, target, art, skill);
		int sc = main_->PlayerDBC()->LastSQLCode();
		if (rc < 0) {
			TLOG_Warning(_T("%s: FAILED to log train and quest to DB for player %u training art %d to skill %d to player %u"), method, origin, art, skill, target);
			GsUtil::HandlePlayerError(main_, method, rc, sc, false);
		}
		// put actual skill back into message being sent to client
		msg.SetState2(skill);
		// if we've just gotten ordained, we get a quest XP pool
		if ((art == Arts::TRAIN) && (old_skill == 0)) {
	      player_->SetQuestPoolXP(Lyra::QUEST_XP_POOL);		
		}
      }
      else {
	// log unsuccessful train
	SECLOG(-4, _T("%s: player %u: unsuccessful train in art %d, skill %d -> %d, by player %u"), method,
	       player_->PlayerID(), art, old_skill, skill, msg.SenderID());
	// set skill to 0, so client knows an attempt was made
	msg.SetState2(0);
      }
    }
    else {
      // log unsuccessful train
      SECLOG(4, _T("%s: player %u: unsuccessful train in art %d, skill %d by player %u"), method, player_->PlayerID(), art, skill, msg.SenderID());
      // set skill to 0, so client knows an attempt was made
      msg.SetState2(0);
    }
  }
  break;

  // player being leveltrained
  case RMsg_PlayerMsg::TRAIN_SPHERE: {       // success, max sphere trainable
    int max_sphere = msg.State2();
    int old_sphere = player_->DB().Stats().Sphere();
    // set success to false
    msg.SetState1(0);
    // determine if player can advance, and if teacher can do the training
    if ((max_sphere > old_sphere) && player_->DB().Stats().ReadyToAdvance(old_sphere + 1)) {
      GMsg_ChangeStat msg_stat;
      player_->AdvanceToNextSphere(msg_stat);
      if (msg_stat.NumChanges() > 0) {
	int new_sphere = player_->DB().Stats().Sphere();
	SECLOG(-3, _T("%s: player %u: leveltrained sphere %d -> %d by player %u"), method,
	       player_->PlayerID(), old_sphere, new_sphere, msg.SenderID());
	int rc = main_->PlayerDBC()->LogQuest(msg.SenderID(), player_->PlayerID(), Arts::LEVELTRAIN, new_sphere);
	int sc = main_->PlayerDBC()->LastSQLCode();
	if (rc < 0) {
		TLOG_Warning(_T("%s: FAILED to log sphere and quest to DB for player %u sphering player %u to sphere %d"), method, msg.SenderID(), player_->PlayerID(), new_sphere);
			GsUtil::HandlePlayerError(main_, method, rc, sc, false);
		}
	main_->OutputDispatch()->SendMessage(&msg_stat, player_->Connection());
	msg.SetState1(1); // succeeded
      }
      else {
	TLOG_Warning(_T("%s: sphere advance failed?"), method);
      }
    }
    else {
      SECLOG(-3, _T("%s: player %u: could not be leveltrained to sphere %d by player %u"), method,
	     player_->PlayerID(), old_sphere + 1, msg.SenderID());
    }
  }
  break;

  // player being demoted
  case RMsg_PlayerMsg::DEMOTE: {            // guild, number tokens available
    int guild = msg.State1();
    int tokens = msg.State2();
    if (player_->CanBeDemoted(guild, tokens)) {
      SECLOG(-7, _T("%s: player %u: demoted in guild %d by player %u"), method, player_->PlayerID(), guild, msg.SenderID());
      // update database
      int used = 0;
	  GMsg_ChangeStat changemsg;
      int new_rank = player_->Demote(guild, used, changemsg);
	  if (changemsg.NumChanges() > 0) {
		   main_->OutputDispatch()->SendMessage(&changemsg, player_->Connection());
	  }

      // make entry in initiator's xp journal if they were booted from the guild entirely
      lyra_id_t initiator = player_->DB().Initiator(guild);
      if ((initiator != Lyra::ID_UNKNOWN) && (new_rank < Guild::INITIATE)) {
	int xp_loss = 10000; // lose 10K xp
	int rc = main_->PlayerDBC()->ModifyXPJournal(initiator, guild, -xp_loss);
	int sc = main_->PlayerDBC()->LastSQLCode();
	// int lt = main_->PlayerDBC()->LastCallTime();
	// main_->Log()->Debug(_T("%s: LmPlayerDBC::ModifyXPJournal took %d ms"), method, lt);
	if (rc < 0) {
	  TLOG_Warning(_T("%s: could not modify xp journal for player %u"), method, initiator);
	  GsUtil::HandlePlayerError(main_, method, rc, sc, false);
	}
	else {
	  SECLOG(-3, _T("%s: player %u: docked %d xp (in journal) due to underling %u leaving guild %d"), method,
		 initiator, xp_loss, player_->PlayerID(), guild);
	}
      }
      // create, send ack to source
      RMsg_PlayerMsg msg_ack;
      msg_ack.Init(player_->PlayerID(), msg.SenderID(), RMsg_PlayerMsg::DEMOTE_ACK, guild, used);
      send_SMsg_Proxy(player_->LevelConnection(), msg_ack);
    }
    else {
      SECLOG(7, _T("%s: player %u: could not be demoted in guild %d by player %u"), method,
	     player_->PlayerID(), guild, msg.SenderID());
      int needed = player_->TokensToDemote(guild);
      // create, send fail to source
      RMsg_PlayerMsg msg_fail;
      msg_fail.Init(player_->PlayerID(), msg.SenderID(), RMsg_PlayerMsg::DEMOTE_FAIL, guild, needed);
      send_SMsg_Proxy(player_->LevelConnection(), msg_fail);
      // don't send DEMOTE to player
      send_to_player = false;
    }
  }
  break;

  // player's attempt to demote was successful
  case RMsg_PlayerMsg::DEMOTE_ACK: {        // guild, number tokens used
    // nothing to do, just pass to client
  }
  break;

  // player's attempt to demote was not successful
  case RMsg_PlayerMsg::DEMOTE_FAIL: {       // guild, number tokens needed
    // nothing to do, just pass to client
  }
  break;

  // player being granted XP by a gamemaster
  case RMsg_PlayerMsg::GRANT_XP: {          // units of 1000, units of 100
    int num_thousand = msg.State1();
    int num_hundred = msg.State2();
    int xp_adj = (num_thousand * 1000) + (num_hundred * 100);
    adjust_xp(xp_adj, _T("grant from player"), msg.SenderID(), true);
  };
  break;

  // player being granted RP XP by a gamemaster
  case RMsg_PlayerMsg::GRANT_RP_XP: {          // units of 1000, units of 100
    int num_thousand = msg.State1();
    int num_hundred = msg.State2();
    int xp_adj = (num_thousand * 1000) + (num_hundred * 100);
    if (xp_adj > 5000) { // max rp xp is 5k; if a unsigned value yields
					// a negative value < 5k, assume negative grant
		xp_adj = ((signed char)(msg.State1()))*1000 + ((signed char)(msg.State2()))*100;
		if (xp_adj < -5000) // ignore
			break;
    }

    adjust_offline_xp(xp_adj, _T("anon rp grant"), msg.SenderID(), true);
	SECLOG(-3, _T("%s: player %u: change of %d in offline xp due to RP XP grant"), method, 
	 player_->DB().PlayerID(), xp_adj);

    send_to_player = false; // don't send to client
  };
  break;

  // player losing XP from a gamemaster
  case RMsg_PlayerMsg::GRANT_XP_NEGATIVE: {  // units of 1000, units of 100
    int num_thousand = msg.State1();
    int num_hundred = msg.State2();
    int xp_adj = - ((num_thousand * 1000) + (num_hundred * 100));
    adjust_xp(xp_adj, _T("grant from player"), msg.SenderID(), true);
  };
  break;

  // player being untrained by a GM
  case RMsg_PlayerMsg::UNTRAIN: { // art, not used
    int art = msg.State1();
	// this cast is necessary because VC++ is confused by overloaded const/non-const selectors
    ((class LmArts&)(player_->DB().Arts())).SetSkill(art, 0);
    int rc = main_->PlayerDBC()->DeleteArt(player_->PlayerID(), art);
    int sc = main_->PlayerDBC()->LastSQLCode();
    if (rc < 0) {
      GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    }
    else {
      SECLOG(-4, _T("%s: player %u: untrained in art %d by player %u"), method, player_->PlayerID(), art, msg.SenderID());
    }
  }
  break;

  case RMsg_PlayerMsg::TEHTHUS_OBLIVION: {       // not used, not used
      SECLOG(-8, _T("%s: Tehthu was killed by player %u"), method, msg.SenderID());
      // update database with account type, and killer id
      int rc = main_->PlayerDBC()->SetKiller(player_->PlayerID(), msg.SenderID());
      int sc = main_->PlayerDBC()->LastSQLCode();
  }
  break;


  // player is being dreamstruck
  case RMsg_PlayerMsg::DREAMSTRIKE: {       // success, not used
    int success = msg.State1();
    if (success) {
      SECLOG(-8, _T("%s: player %u: was dreamstruck by player %u"), method, player_->PlayerID(), msg.SenderID());
      // update database with account type, and killer id
      int rc = main_->PlayerDBC()->SetKiller(player_->PlayerID(), msg.SenderID());
      int sc = main_->PlayerDBC()->LastSQLCode();
      // int lt = main_->PlayerDBC()->LastCallTime();
      // main_->Log()->Debug(_T("%s: LmPlayerDBC::SetKiller took %d ms"), method, lt);
      if (rc < 0) {
	TLOG_Warning(_T("%s: could not set killer of player %u; rc=%d, sqlcode=%d"), method, player_->PlayerID(), rc, sc);
	GsUtil::HandlePlayerError(main_, method, rc, sc, false);
      }
      // disconnect
      // GsUtil::FakeLogout(main_, player_);
    }
    else {
      SECLOG(-8, _T("%s: player %u: avoided being dreamstruck by player %u"), method, player_->PlayerID(), msg.SenderID());
    }
  }
  break;

  // player dreamstruck someone else, this is the ack from level server
  case RMsg_PlayerMsg::DREAMSTRIKE_ACK: {   // success, not used
    int success = msg.State1();
    if (success) {
      SECLOG(-8, _T("%s: player %u: dreamstrike of player %u succeeded"), method, player_->PlayerID(), msg.SenderID());
    }
    else {
      SECLOG(-8, _T("%s: player %u: dreamstrike of player %u failed"), method, player_->PlayerID(), msg.SenderID());
    }
    // send ack to player, in either case
    send_RMsg_PlayerMsg_DreamStrikeAck(success, msg.ReceiverID());
    send_to_player = false; // don't send to client, call above handles it
  }
  break;

  // player being asked to be initiated into guild
  case RMsg_PlayerMsg::INITIATE: {          // guild, success
    int guild = msg.State1();
    int success = msg.State2();
    if (success) { // if initiator was successful
      if (!player_->CanBeInitiated(guild)) { // if player can't be initiated
	// send_to_player = false; // don't send to client
	msg.SetState2(false);
      }
      else { // could be initiated, set rank to "initiate_pending"; client will send ack
	player_->ChangeGuildRank(guild, Guild::INITIATE_PENDING);
      }
    }
  }
  break;

  // player initiated someome else into guild
  case RMsg_PlayerMsg::INITIATE_ACK: {      // guild, accept
    int guild = msg.State1();
    int accept = msg.State2();
    if (accept) { // if they accepted
      // add appropriate amount of xp
      int xp_adj = 5000;
      // TLOG_Debug(_T("%s: player %u: initiated player %u guild %d"), method, player_->PlayerID(), msg.SenderID(), guild);
      adjust_xp(xp_adj, _T("initiation of player"), msg.SenderID(), true);
      // propagate up the chain of initiators
      lyra_id_t initiator = player_->DB().Initiator(guild);
      if (initiator != Lyra::ID_UNKNOWN) {
	int xp_gain = 2500; // divide by 2
	int rc = main_->PlayerDBC()->ModifyXPJournal(initiator, guild, xp_gain);
	int sc = main_->PlayerDBC()->LastSQLCode();
	// int lt = main_->PlayerDBC()->LastCallTime();
	// main_->Log()->Debug(_T("%s: LmPlayerDBC::ModifyXPJournal took %d ms"), method, lt);
	if (rc < 0) {
	  TLOG_Warning(_T("%s: could not modify player %u xp journal; rc=%d, sqlcode=%d"), method, initiator, rc, sc);
	  GsUtil::HandlePlayerError(main_, method, rc, sc, false);
	}
	else {
	  SECLOG(-3, _T("%s: player %u: granted %d xp (in journal) due to underling %u initiating player %u in guild %d"),
		 method, initiator, xp_gain, player_->PlayerID(), msg.SenderID(), guild);
	}
      }
    }
  }
  break;

  // player being knighted
  case RMsg_PlayerMsg::KNIGHT: {            // guild, success
    int guild = msg.State1();
    int success = msg.State2();
    if (success) { // if knightor was successful
      if (!player_->CanBeKnighted(guild)) { // if player can't be made a knight
	msg.SetState2(false);
	// send_to_player = false; // don't send to client
      }
      else { // can be knighted; you're knighted
	SECLOG(-7, _T("%s: player %u: knighted in guild %d by player %u"), method,
	       player_->PlayerID(), guild, msg.SenderID());
	player_->ChangeGuildRank(guild, Guild::KNIGHT);
	// add XP for being knighted
	int xp_adj = 25000;
	adjust_xp(xp_adj, _T("being knighted by player"), msg.SenderID(), true);
	// add to XP pool
	player_->SetPoolXP(guild, Lyra::KNIGHT_XP_POOL);
    player_->SetQuestPoolXP(Lyra::QUEST_XP_POOL);
      }
    }
  }
  break;
  // player was in a party, party member killed something
  case RMsg_PlayerMsg::PARTYKILL: {          // victim's orbit/nightmare index, # of party members
    int orbit = msg.State1();
    int party_size = msg.State2();
    // determine total # of shares, and number of shares we get
    int all_shares = party_size + 1;
    int my_shares = 1;
    if (party_size > 100) { // we were the killer, we get another share
      all_shares -= 100;
      my_shares = 2;
    }
    // determine how much XP to gain
    int xp_adj = 0;
    bool agent_killed = true;
    if (orbit < 100) { // player, state1 = orbit
      xp_adj = (int) ((double) LmStats::OrbitXPBase(orbit) * 0.01); // gain up to 1%
      agent_killed = false;
    }
    else if (orbit < 150) { // monster, state1 = 100 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 100);
	}
    else if (orbit < 200) { // player nightmare, state1 = 150 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 150)*4;
    }
    else { // dark or posessed mare = 200 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 200)*8;
    }

    // TLOG_Debug(_T("%s: party kill: xp=%d shares=%d/%d"), method, xp_adj, my_shares, all_shares);
    xp_adj = my_shares * (xp_adj / all_shares); // divide up among party members
    if (agent_killed) {
      adjust_xp(xp_adj, _T("partykill of agent"), msg.SenderID(), true);
    }
    else {
      adjust_xp(xp_adj, _T("partykill of player"), msg.SenderID(), true);
    }
  }
  break;

  case RMsg_PlayerMsg::EMPATHY: {

        int xp = (msg.State1() * 1000) + (msg.State2() * 100);
	msg.SetState1(1); // currently no further server checks - allow success
	adjust_xp(xp, _T("received empathy"), msg.SenderID(), true);
	break;
  }


  // player killed someone
  case RMsg_PlayerMsg::YOUGOTME: {           // victim's orbit/nightmare index, DS at dissolution

    int orbit = msg.State1();
    int ds = msg.State2(); 
    // nothing happens when admins get kills
    if (player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN)
      break;

    player_->AddKill(orbit);
    if (player_->DB().AccountType() == LmPlayerDB::ACCT_MONSTER) {
      break;
    }

    // determine how much XP to gain
    int xp_adj = 0;
    if (orbit < 100) { // player, state1 = orbit
      int sphere_diff = player_->DB().Stats().Sphere() - (orbit / 10);
      // if player killed someone 2+ spheres below them, 
      // and that dreamer is below 3rd sphere, they get nothing,
      // EVERYONE gets XP for kills on 3rd sphere + dreamers;
      // killing someone whose orbit is greater is just fine
      if ((orbit > 29) || (sphere_diff < 2)) {
	xp_adj = (int) ((double) LmStats::OrbitXPBase(orbit) * 0.01); // gain up to 1%
      }
      adjust_xp(xp_adj, _T("dissolving player"), msg.SenderID(), true);
    } else if (orbit < 150) { // monster, state1 = 100 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 100);
      // player mares get only 25% XP for agent kills
      if (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE) {
	xp_adj = (int)(xp_adj/4);
      }
      adjust_xp(xp_adj, _T("dissolving agent"), msg.SenderID(), true);
    } else if (orbit < 200) { // player mare = 150 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 150)*4;
      // player mares get only 25% XP for pmare kills
      if (player_->DB().AccountType() == LmPlayerDB::ACCT_PMARE) {
    	xp_adj = (int)(xp_adj/4); 
      }
      adjust_xp(xp_adj, _T("dissolving player mare"), msg.SenderID(), true);
      //}
    } else { // darkmare, posess, or player mare = 200 + nightmare index
      xp_adj = GsUtil::NightmareXP(orbit - 200)*8;
      adjust_xp(xp_adj, _T("dissolving dark mare"), msg.SenderID(), true);
      //}
    }
  }
  break;

  // player is being dreamstruck
  case RMsg_PlayerMsg::SUMMON_PRIME: {       // success, not used
    int success = msg.State2();
    if (success) {
      SECLOG(-8, _T("%s: player %u: summoned House Prime for House %u"), method, player_->PlayerID(), msg.State1());
      // nuke 5% of their XP
	  int xp_adj = - (int) ((double) player_->DB().Stats().XP() * 0.05); // lose up to 5%
      adjust_xp(xp_adj, _T("summoned house prime"), msg.SenderID(), true);
    }
    else {
      SECLOG(-8, _T("%s: player %u: failed at summoning House Prime for House %u"), method, player_->PlayerID(), msg.State1());
    }
  }
  break;

  // Player is attempting to Rally someone
  case RMsg_PlayerMsg::RALLY: {				// x-coord, y-coord
	  GsPlayer* rallying_player = main_->PlayerSet()->GetPlayer(msg.SenderID());
	  if (rallying_player) {
		  SECLOG(-8, _T("%s: player %u attempting to Rally player %u to %i; %i; %u"), method, msg.SenderID(), msg.ReceiverID(), msg.State1(), msg.State2(), rallying_player->LevelID());
		  player_->SaveSummonInfo(rallying_player->RoomID(), rallying_player->LevelID());
	  }
  }
							  break;

  //
  // player should never receive
  //

  case RMsg_PlayerMsg::ASCEND:
  case RMsg_PlayerMsg::RETURN:
    TLOG_Warning(_T("%s: player %u received illegal playermsg %d from player %u"), method,
		 msg.ReceiverID(), msg.MsgType(), msg.SenderID());
    send_to_player = false;
    break;

  default: { // unknown
    TLOG_Warning(_T("%s: player %u received unknown playermsg"), method, msg.ReceiverID());
    send_to_player = false;
  }
  break;

  } // end switch

  // send to player
  if (send_to_player) {
    // message may have been modified, so re-read into buffer
    msgbuf->ReadMessage(msg);
    main_->OutputDispatch()->SendMessage(msgbuf, player_->Connection());
  }
}
