// GsPlayerThread2.cpp  -*- C++ -*-
// $Id: GsPlayerThread2.cpp,v 1.34 1998/05/11 18:11:35 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// handle_RMsg_* methods

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "GsPlayerThread.h"
#include "LyraDefs.h"
#include "LmParty.h"
#include "GsMain.h"
#include "LmLevelDBC.h"
#include "LmBillingDBC.h"
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
#include "LmRand.h"
#include "GMsg_SenseDreamersAck.h"
#include "GMsg_LocateNewliesAck.h"
#include "GMsg_LocateMaresAck.h"
#include "LmPlayerDBC.h"
#include "LmItemDBC.h"

////
// handle_RMsg_Party
////

void GsPlayerThread::handle_RMsg_Party(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_Party);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  CHECK_PLAYER_INLEVEL();
  // create proxy message, copy message bytes into it, send to player's level server
  send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
}

////
// handle_RMsg_GetAvatarDescription
////

void GsPlayerThread::handle_RMsg_GetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_GetAvatarDescription);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  CHECK_PLAYER_INLEVEL();
  // create proxy message, copy message bytes into it, send to player's level server
  send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
}

////
// handle_RMsg_GetRoomDescription
////

void GsPlayerThread::handle_RMsg_GetRoomDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
	DEFMETHOD(GsPlayerThread, handle_RMsg_GetRoomDescription);
	HANDLER_ENTRY(false);
	// pre-conditions
	CHECK_CONN_NONNULL();
	CHECK_CONN_ISCLIENT();
	CHECK_PLAYER_NONNULL();
	CHECK_CONN_ID();
	CHECK_PLAYER_INLEVEL();
	// create proxy message, copy message bytes into it, send to player's level server
	send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
}

////
// handle_RMsg_GotoRoom
////

void GsPlayerThread::handle_RMsg_GotoRoom(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_GotoRoom);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message, send error
  ACCEPT_MSG(RMsg_GotoRoom, true);
  // process
  lyra_id_t roomid = msg.RoomID();
  lyra_id_t levelid = player_->LevelID();
  // check that player is in a level, and is connected to level server
  if (!player_->InLevel() || !player_->LevelDBC() || !player_->LevelConnection()) {
    TLOG_Warning(_T("%s: player %u not in level"), method, player_->PlayerID());
    GsUtil::Send_RMsg_RoomLoginAck(main_, conn, RMsg_RoomLoginAck::LOGIN_ERROR);
    return;
  }
  // check that room is in level
  if (!player_->LevelDBC()->ContainsRoom(roomid)) {
    TLOG_Warning(_T("%s: room %u not in level %u"), method, roomid, levelid);
    GsUtil::Send_RMsg_RoomLoginAck(main_, conn, RMsg_RoomLoginAck::LOGIN_ROOMNOTFOUND);
    return;
  }

  // check that player is allowed to "get there from here"
  if (!player_->CanGotoRoom(roomid)) {
    SECLOG(1, _T("%s: player %u: illegal gotoroom, no portal from room %u to room %u in level %u"), method,
	   player_->PlayerID(), player_->RoomID(), roomid, levelid);
    // TODO: don't allow this eventually
    GsUtil::Send_RMsg_RoomLoginAck(main_, conn, RMsg_RoomLoginAck::LOGIN_ROOMNOTFOUND);
    // return;
  }
  SECLOG(-1, _T("%s: player %u: moving to room %u in level %u"), method, player_->PlayerID(), roomid, levelid);
  // save roomid, update info
  player_->GotoRoom(roomid);
  player_->ReceivedUpdate(msg.PeerUpdate());
  // create proxy message, copy message bytes into it, send to player's level server
  send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
  
  // Added by DiscoWay from server/gamed/GsPlayerThread1.cpp in GotoLevel.  Goal is for more accurate room updates and to fix sense dakote
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
// handle_RMsg_Logout
////

void GsPlayerThread::handle_RMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_Logout);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message, send error
  ACCEPT_MSG(RMsg_Logout, true);
  // process
  //  SECLOG(-1, _T("%s: player %u: logging out of level %u, status %c"), method,	 player_->PlayerID(), player_->LevelID(), msg.Status());
  if (msg.Status() == RMsg_Logout::GOALBOOK) {
    // save goal posting return location
    player_->SaveGoalReturnInfo();
  }

  // if any items in logout message, store their positions in the inventory
  if (msg.NumItems() > 0) {
    for (int i = 0; i < msg.NumItems(); ++i) {
	  //short itemx = msg.ItemX(i) | (msg.ItemFlags(i) << 8);
      player_->SetItemX(msg.ItemSerial(i), msg.ItemX(i));
    }
  }

  // create proxy message, copy message bytes into it, send to player's level server
  if (player_->LevelConnection()) {
    send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
  }
  player_->SetInLevel(false);
}

////
// handle_RMsg_Speech
////

void GsPlayerThread::handle_RMsg_Speech(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_Speech);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  // accept message, send error
  ACCEPT_MSG(RMsg_Speech, true);
  // process certain kinds of speech in game server
  bool send_out = true;
  switch (msg.SpeechType()) {
  case RMsg_Speech::REPORT_BUG:
    msg.RemoveNewlines();
    SECLOG(-2, _T("%s: player %u: BUG (level=%u, room=%u, pos=(%d,%d)): %s"), method,
	   player_->PlayerID(), player_->LevelID(), player_->RoomID(),
	   player_->PlayerUpdate().X(), player_->PlayerUpdate().Y(), msg.SpeechText());
    send_out = false;
    break;
  //case RMsg_Speech::REPORT_QUEST: // eliminated unnecessary double sending of text
//    msg.RemoveNewlines();
    //SECLOG(-2, _T("%s: player %u: QUEST: %s"), method,
	   //player_->PlayerID(), 
	   //msg.SpeechText());
    //send_out = false;
    //break;

  case RMsg_Speech::RP:
    msg.RemoveNewlines();
    SECLOG(-2, _T("%s: player %u: ROLE PLAY REPORT: %s"), method,
	   player_->PlayerID(), msg.SpeechText());
    send_out = false;
    break;
  case RMsg_Speech::REPORT_DEBUG:
    msg.RemoveNewlines();
    SECLOG(-2, _T("%s: player %u: DEBUG (level=%u, room=%u, pos=(%d,%d)): %s"), method,
	   player_->PlayerID(), player_->LevelID(), player_->RoomID(),
	   player_->PlayerUpdate().X(), player_->PlayerUpdate().Y(), msg.SpeechText());
    send_out = false;
    break;
  case RMsg_Speech::REPORT_CHEAT:
    if (player_->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) { // don't log cheat reports from agents
      msg.RemoveNewlines();
      SECLOG(2, _T("%s: player %u: CHEAT (level=%u, room=%u): %s"), method, player_->PlayerID(),  player_->LevelID(), player_->RoomID(), msg.SpeechText());
    }
    send_out = false;
    break;
  case RMsg_Speech::AUTO_CHEAT:
    if (player_->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) { // don't log cheat reports from agents
      msg.RemoveNewlines();
      SECLOG(2, _T("%s: player %u: AUTOCHEAT  (level=%u, room=%u): %s"), method, player_->PlayerID(),  player_->LevelID(), player_->RoomID(),msg.SpeechText());
    }
    send_out = false;
    break;
  case RMsg_Speech::SERVER_TEXT:
    // shouldn't ever be sent from client, just ignore it
    SECLOG(2, _T("%s: player %u: illegal SERVER_TEXT speech"), method, player_->PlayerID());
    send_out = false;
    break;
  case RMsg_Speech::RAW_EMOTE: // GM-only
  case RMsg_Speech::GLOBALSHOUT:
    if (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN) {
      SECLOG(2, _T("%s: player %u: illegal GM-only speech (%c)"), method, player_->PlayerID(), msg.SpeechType());
      send_out = false;
    }
    break;
  case RMsg_Speech::EMOTE:
  case RMsg_Speech::SHOUT:
  case RMsg_Speech::MONSTER_SPEECH:
  case RMsg_Speech::PARTY:
  case RMsg_Speech::SPEECH:
  case RMsg_Speech::WHISPER:
  case RMsg_Speech::WHISPER_EMOTE:
  case RMsg_Speech::SYSTEM_SPEECH:
  case RMsg_Speech::SYSTEM_WHISPER:
  case RMsg_Speech::TELL_IP:
    // ok
    break;
  default: // unknown
    SECLOG(2, _T("%s: player %u: illegal unknown speech (%c)"), method, player_->PlayerID(), msg.SpeechType());
    send_out = false;
    break;
  }
  // send it?
  if (send_out) {
    CHECK_PLAYER_INLEVEL();
    // send to level server
    send_SMsg_Proxy(player_->LevelConnection(), msgbuf);
  }
}

////
// handle_RMsg_PlayerMsg
////

void GsPlayerThread::handle_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(GsPlayerThread, handle_RMsg_PlayerMsg);
 HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISCLIENT();
  CHECK_PLAYER_NONNULL();
  CHECK_CONN_ID();
  CHECK_PLAYER_INLEVEL();
  // accept message, send error
  ACCEPT_MSG(RMsg_PlayerMsg, true);

  // check senderid (it should be the player's id)
  if (msg.SenderID() != player_->PlayerID()) {
    TLOG_Warning(_T("%s: player %u != sender %u"), method, player_->PlayerID(), msg.SenderID());
    // TODO: return?  set sender?
  }
  // debug log
  TLOG_Debug(_T("%s: player %u sending playermsg %d [%d,%d] to %u"), method,
	     msg.SenderID(), msg.MsgType(), msg.State1(), msg.State1(), msg.ReceiverID());
  // process, depending on type of player message
  // NOTE: this is the client->gs->rs message; the rs->gs->message is in SMsg_Proxy_RMsg_PlayerMsg
  bool send_to_level = true; // default: forward message to level server (and to target player)

  // Debug code
  int message_type = msg.MsgType();
  int in_art_id = msg.State1();
  int reflect_art = RMsg_PlayerMsg::REFLECT_ART;
  int from = msg.SenderID();

  bool bypass = false;
  //_tprintf("Player #%d sent message #%d.\nServer says reflect_art is #%d, message says #%d. Art reflected is #%d",from,message_type,reflect_art,message_type,in_art_id);

	// ensure player has the art
  //int art = RMsg_PlayerMsg::ArtType(msg.MsgType());

  //if (!player_->CanUseArt(art, 1)) {
    //  SECLOG(4, _T("%s: player %u: attempt to use art %d with zero"), method,
	  //   player_->PlayerID(), art);
      //send_to_level = false;
  //} else 
  
  switch (msg.MsgType()) {

  //
  // Simple Skill-Check Messages
  //
  case RMsg_PlayerMsg::RESIST_FEAR:         // skill, not used
  case RMsg_PlayerMsg::RESIST_CURSE:        // skill, not used
  case RMsg_PlayerMsg::RESIST_PARALYSIS:    // skill, not used
  case RMsg_PlayerMsg::JUDGEMENT:           // skill, not used
  case RMsg_PlayerMsg::IDENTIFY_CURSE:      // skill, not used
  case RMsg_PlayerMsg::VISION:              // skill, not used
  case RMsg_PlayerMsg::BLAST:               // skill, not used
  case RMsg_PlayerMsg::RESTORE:             // skill, not used     
  case RMsg_PlayerMsg::PURIFY:              // skill, not used
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
  case RMsg_PlayerMsg::FIRESTORM:           // skill, not used
  case RMsg_PlayerMsg::RAZORWIND:           // skill, not used
  case RMsg_PlayerMsg::TRAP_NIGHTMARE:      // skill, not used
  case RMsg_PlayerMsg::MIND_BLANK_OTHER:    // skill, not used
  case RMsg_PlayerMsg::EARTHQUAKE:          // skill, not used
  case RMsg_PlayerMsg::HYPNOTIC_WEAVE:      // skill, not used
  case RMsg_PlayerMsg::TERROR:              // skill, not used
  case RMsg_PlayerMsg::HEALING_AURA:         // skill, not used
  case RMsg_PlayerMsg::ABJURE:              // skill, not used
  case RMsg_PlayerMsg::SOUL_SHIELD:         // skill, not used     
  case RMsg_PlayerMsg::REFLECT_ART:         // skill, art_id
  case RMsg_PlayerMsg::EXPEL:				// skill, guild_id
  case RMsg_PlayerMsg::CHAOS_PURGE:			// skill, not used 
  case RMsg_PlayerMsg::CUP_SUMMONS:			// skill, not used 
  case RMsg_PlayerMsg::SUMMON_PRIME:		// skill, success
  case RMsg_PlayerMsg::SCAN:				// skill, not used 
  case RMsg_PlayerMsg::HEAL:				// skill, not used 
  case RMsg_PlayerMsg::SANCTIFY:			// skill, not used 
  case RMsg_PlayerMsg::REMOVE_CURSE:		// skill, not used 
  case RMsg_PlayerMsg::HOLD_AVATAR:			// skill, not used 
  case RMsg_PlayerMsg::TEMPEST:         // skill, angle
  case RMsg_PlayerMsg::KINESIS:         // skill, angle
  case RMsg_PlayerMsg::MISDIRECTION:    // skill, unused
  case RMsg_PlayerMsg::CHAOTIC_VORTEX:  // skill
    {
    // check that player can use given art, at the given skill level
    int art = RMsg_PlayerMsg::ArtType(msg.MsgType());
    int skill = msg.State1();
    if ((player_->DB().AccountType() != LmPlayerDB::ACCT_MONSTER) && !(player_->CanUseArt(art, skill))) {
		if (player_->PPEvoking() == art) { // we spent pp's to evoke this art once
			player_->SetPPEvoking(Arts::NONE);
			player_->SetPPSkill(0);
			break;
		}

      int p_skill = player_->DB().Arts().Skill(art);
      SECLOG(4, _T("%s: player %u: attempt to use art %d at skill level %d, own skill is %d"), method,
	     player_->PlayerID(), art, skill, p_skill);
      send_to_level = false;
    }
  }
  break;


  // should never get this
  case RMsg_PlayerMsg::GRANT_PPOINT: {         // not used, not used
      SECLOG(5, _T("%s: player %u: sent illegal attempt at ppoint grant message"), method, player_->PlayerID());
      send_to_level = false;
	  break;
	}

  //
  // GM-Only Messages
  //

  case RMsg_PlayerMsg::FINGER_OF_DEATH:     // not used, not used
  case RMsg_PlayerMsg::GRANT_XP:            // units of 1000, units of 100
  case RMsg_PlayerMsg::GRANT_XP_NEGATIVE:   // units of 1000, units of 100
 case RMsg_PlayerMsg::UNTRAIN:             // art id, not used
  case RMsg_PlayerMsg::BOOT:                // not used, not used
  case RMsg_PlayerMsg::SUMMON:                // not used, not used
  case RMsg_PlayerMsg::SUSPEND:                // not used, not used
  case RMsg_PlayerMsg::TERMINATE: {         // not used, not used
    if (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN) {
      SECLOG(5, _T("%s: player %u: sent GM-only playermsg %d"), method, player_->PlayerID(), msg.MsgType());
      send_to_level = false;
    }
  }
  break;

  case RMsg_PlayerMsg::GRANT_RP_XP: {       // units of 1000, units of 100
    if (player_->DB().AccountType() != LmPlayerDB::ACCT_ADMIN) {
      SECLOG(5, _T("%s: player %u: sent GM-only playermsg %d"), method, player_->PlayerID(), msg.MsgType());
      send_to_level = false;
    }
    int xp = (msg.State1() * 1000) + (msg.State2() * 100);
    if ((xp > 100000) || (xp < -100000)) { // max rp xp is 100k;
		SECLOG(5, _T("%s: player %u: attempted to grant %d rp xp"), method, player_->PlayerID(), xp);
		send_to_level = false;
    }
  }
  break;

  // 
  // Special Processing (XP changes, skill/stat changes, etc)
  // 

  case RMsg_PlayerMsg::SENSE_DREAMERS:	  {

	  // look up current dreamer locations
    GMsg_SenseDreamersAck sense_msg;
	lyra_id_t level_ids[PLANES_SENSED_COUNT];
	//main_->ItemDBC()->SetNumDreamers(29, 4);
	//main_->ItemDBC()->ChangeNumDreamers(29, -2);
	main_->ItemDBC()->GetDreamerLocations((lyra_id_t*)level_ids, player_->DB().AccountType());
    sense_msg.Init(); // one change
	int i;
	for (i=0; i<PLANES_SENSED_COUNT; i++)
		sense_msg.SetLevelID((unsigned char)level_ids[i], i);

    main_->OutputDispatch()->SendMessage(&sense_msg, player_->Connection());
	}
	break;

  case RMsg_PlayerMsg::HOUSE_MEMBERS:	{		// guild_id, not used 
	GMsg_LocateAvatarAck locate_msg;
	bool gm = (player_->DB().AccountType() == LmPlayerDB::ACCT_ADMIN);

	main_->PlayerDBC()->FindHouseMembers(locate_msg, (unsigned int)msg.State1(), gm, player_->DB().PlayerID());
    main_->OutputDispatch()->SendMessage(&locate_msg, player_->Connection());
	}
	break;



  case RMsg_PlayerMsg::LOCATE_NEWLIES:	  {
	  // look up newly awakened dreamer locations
    GMsg_LocateNewliesAck newly_msg;
	//main_->ItemDBC()->SetNumDreamers(29, 4);
	//main_->ItemDBC()->ChangeNumDreamers(29, -2);
	main_->PlayerDBC()->LocateNewlyAwakened(&newly_msg);

    main_->OutputDispatch()->SendMessage(&newly_msg, player_->Connection());
	}
	break;

  case RMsg_PlayerMsg::LOCATE_MARES:	  {
	  // look up newly awakened dreamer locations
    GMsg_LocateMaresAck mares_msg;
	//main_->ItemDBC()->SetNumDreamers(29, 4);
	//main_->ItemDBC()->ChangeNumDreamers(29, -2);
	main_->PlayerDBC()->LocateMares(&mares_msg);

    main_->OutputDispatch()->SendMessage(&mares_msg, player_->Connection());
	}
	break;


  case RMsg_PlayerMsg::REDEEM_GRATITUDE: {
    int i = 0;
    unsigned short creator_low_bits = 0;
    const void* state;
    unsigned char function_type;

	creator_low_bits = (msg.State1() << 8) | msg.State2();
	// find the gratitude token in inventory that matches these low bits
    for (i = 0; i< player_->DB().Inventory().NumItems(); ++i) {
		LmItem item = player_->DB().Inventory().ItemByIndex(i);
		state = item.StateField(0);
		function_type = (*((unsigned char*)state));

		if (function_type == LyraItem::GRATITUDE_FUNCTION)
		{
			lyra_item_gratitude_t gratitude;
			memcpy(&gratitude, state, sizeof(gratitude));
			if (creator_low_bits == gratitude.creator_lo) {
					// we found the match!
				//bool success = main_->BillingDBC()->CanRedeemToken(gratitude.creatorid(), gratitude.maturity_date);
				bool success = true; // maturity period no longer enforced
				// and double check the target ID is correct
				unsigned short target_lo = (player_->PlayerID() & 0x0000ffff);
				if (target_lo != gratitude.target)
					success = false;
				send_RMsg_PlayerMsg_GratitudeAck(success);
				// grant 1% XP bonus, or 5000, whichever is more
				if (success) 
				{
					int xp_bonus = player_->DB().Stats().XP()/20;
					if (xp_bonus < 5000)
						xp_bonus = 5000;
					adjust_xp(xp_bonus, _T("Redemption of Gratitude Token"), gratitude.creatorid(), true);
				}
			}
		}
	}									 
	}
	break;


  case RMsg_PlayerMsg::DRAIN_SELF:          // stat, amount
    // for now, don't do a thing
    break;

 case RMsg_PlayerMsg::ROGER_WILCO:           // not used, not used
	 TLOG_Debug(_T("%s: player %u sending ROGERWILCO msg %d [%d,%d] to %u"), method,
	     msg.SenderID(),  msg.State1(), msg.State2(), msg.ReceiverID());
 
   break;

  // player attempting to train someone in an art
  case RMsg_PlayerMsg::TRAIN: {             // art_id, teacher_skill (+ 100 if GM train)
    int art = msg.State1();
    int skill = msg.State2();
    if (!player_->CanTrain(art, skill)) {
      SECLOG(4, _T("%s: player %u: attempted illegal train of player %u, art %d, skill %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), art, skill);
      send_to_level = false;
    }
    else {
      SECLOG(-4, _T("%s: player %u: training player %u in art %d, skill %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), art, skill);
    }
  }
  break;

  case RMsg_PlayerMsg::TRAIN_ACK:
	 break; // do nothing

  // siphon off XP
  case RMsg_PlayerMsg::EMPATHY: {
    
        int xp = (msg.State1() * 1000) + (msg.State2() * 100);
		if ((xp > 50000) || (xp < 100)) {
			SECLOG(3, _T("%s: player %u: attempted to Bequeath %i xp to %u"), method, player_->PlayerID(), xp, msg.ReceiverID());
			send_to_level = false;
			break;
		}
	adjust_xp(-2*xp, _T("evoking empathy"), player_->PlayerID(), true);
	break;
  }

  // check guild membership on House arts
  case RMsg_PlayerMsg::RADIANT_BLAZE:
	if (player_->DB().Stats().GuildRank(Guild::RADIANCE) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Radiant Blaze (non-POR)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

	break;

  case RMsg_PlayerMsg::POISON_CLOUD:
	if (player_->DB().Stats().GuildRank(Guild::CALENTURE) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Poison Cloud (non-HC)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::BREAK_COVENANT:
	if (player_->DB().Stats().GuildRank(Guild::COVENANT) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Break Covenant (non-UOC)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::PEACE_AURA:
	if (player_->DB().Stats().GuildRank(Guild::ECLIPSE) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Peace Aura (non-AOE)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::SABLE_SHIELD:
	if (player_->DB().Stats().GuildRank(Guild::MOON) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Sable Shield (non-OSM)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::ENTRANCEMENT:
	if (player_->DB().Stats().GuildRank(Guild::ENTRANCED) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Entrancement (non-GOE)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::SHADOW_STEP:
	if (player_->DB().Stats().GuildRank(Guild::SHADOW) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Shadow Step (non-KOES)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;

  case RMsg_PlayerMsg::DAZZLE:
	if (player_->DB().Stats().GuildRank(Guild::LIGHT) < Guild::INITIATE) {
		  SECLOG(3, _T("%s: player %u: attempted illegal use of Dazzle (non-DOL)"), method,
			 player_->PlayerID());
		send_to_level = false;
	}

    break;




  // player attempting to spheretrain someone
  case RMsg_PlayerMsg::TRAIN_SPHERE: {       // success, max sphere trainable
    int max_skill_sphere = player_->DB().Stats().Sphere() - 1;
    if (player_->DB().Arts().Skill(Arts::LEVELTRAIN) < 1) {
      SECLOG(3, _T("%s: player %u: attempted illegal leveltrain of player %u"), method,
	     player_->PlayerID(), msg.ReceiverID());
      send_to_level = false;
    }  else {

      int max_tokens_sphere = player_->SphereTokens(msg.ReceiverID()); 
      int max_sphere = MIN(max_tokens_sphere, max_skill_sphere);
      //SECLOG(-3, _T("%s: player %u: max skill = %d, tokens = %d, max = %d"), method, max_tokens_sphere, max_skill_sphere, max_sphere);
      msg.SetState1(1);
      msg.SetState2(max_sphere);
      SECLOG(-3, _T("%s: player %u: leveltraining player %u to at most sphere %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), max_sphere);
    }
  }
  break;

  // player attempting to demote someone
  case RMsg_PlayerMsg::DEMOTE: {            // guild, number tokens available
    int guild = msg.State1();
    int num_tokens = msg.State2();
    if ((!player_->CanDemote(guild, num_tokens, msg.ReceiverID())) && 
		(player_->PlayerID() != msg.ReceiverID())) {
      SECLOG(7, _T("%s: player %u: attempted illegal demotion of player %u, guild %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), guild);
      send_to_level = false; // can't demote
    }
    else {
      SECLOG(-7, _T("%s: player %u: attempting to demote player %u in guild %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), guild);
    }
    // tokens checked on receiver side, ack/fail sent by receiver
  }
  break;

  // player using soulevoke art
  case RMsg_PlayerMsg::SOULEVOKE: { // not used, not used
    send_to_level = false; // never send
    int delta = 1; // how much to lose
    // has soulevoke art?
    if (player_->DB().Arts().Skill(Arts::SOULEVOKE) == 0) {
      SECLOG(7, _T("%s: player %u: attempted illegal soulevoke"), method);
    }
    // decrement their dreamsoul anyway
    int max_ds = player_->DB().Stats().MaxStat(Stats::DREAMSOUL);
    max_ds -= delta;
    if (max_ds <= 0) {
      max_ds = 1; // min DS
    }
    player_->ChangeMaxStat(Stats::DREAMSOUL, max_ds);
    GMsg_ChangeStat out_msg;
    out_msg.Init(1); // one change
    out_msg.InitChange(0, GMsg_ChangeStat::SET_STAT_MAX, Stats::DREAMSOUL, max_ds);
    main_->OutputDispatch()->SendMessage(&out_msg, player_->Connection());
  }
  break;

  // player attempting to dreamstrike someone
  case RMsg_PlayerMsg::DREAMSTRIKE: {       // not used, not used
    bool strike_success = false;
    // check that player has the skill and necessary dreamsoul
    if (!player_->CanDreamStrike(msg.ReceiverID())) { // naughty, naughty
      SECLOG(8, _T("%s: player %u: attempting illegal dreamstrike of player %u"), method,
	     player_->PlayerID(), msg.ReceiverID());
      send_to_level = false;
    }
    else { // you asked for it...
      // do skill trial: pct success is current DS / 2
      int curr_ds = player_->DB().Stats().CurrentStat(Stats::DREAMSOUL);
      int max_ds = player_->DB().Stats().MaxStat(Stats::DREAMSOUL);
	  //OLD: % success = half current ds
      //if (LmRand::Generate(1, 100) < (curr_ds / 2)) {
	  //NEW: 75% chance of success
	  if (LmRand::Generate(1,100) <= 75) {
		strike_success = true;
		max_ds -= 5;
      } else {
		max_ds -= 1;
	  }
      // OLD: dock stats (max DS down by d4) (even if not successful)
      //max_ds -= LmRand::Generate(1, 4);
	  /// NEW: lose 5 ds on success, 1 on failure
      player_->ChangeMaxStat(Stats::DREAMSOUL, max_ds);
      // log it if unsuccessful (successful is logged in ack reception)
      if (!strike_success) {
		send_to_level = false; // don't send to target
		SECLOG(-8, _T("%s: player %u: failed in attempt to dreamstrike player %u"), method,
	       player_->PlayerID(), msg.ReceiverID());
      }
    }
    // if not sending to level server, send ack, changestat to player (otherwise,
    // level server will send ack and be handled in reception of playermsg)
    if (!send_to_level) {
      send_RMsg_PlayerMsg_DreamStrikeAck(strike_success, msg.ReceiverID());
    }
  }
  break;

  // player is attempting to initiate someone into a guild
  case RMsg_PlayerMsg::INITIATE: {          // guild, success
    int guild = msg.State1();
    int success = msg.State2();
    // check that they can initiate
    if (!player_->CanInitiate(guild)) {
      SECLOG(7, _T("%s: player %u: illegal attempt to initiate into guild %d"), method, player_->PlayerID(), guild);
      msg.SetState2(0); // set success to false
    }
    else {
      SECLOG(-7, _T("%s: player %u: attempting to initiate player %u into guild %d (success %d)"), method,
	     player_->PlayerID(), msg.ReceiverID(), guild, success);
    }
  }
  break;
							 
  // player is acknowledging initiation
  case RMsg_PlayerMsg::INITIATE_ACK: {      // guild, accept
    int guild = msg.State1();
    int accept = msg.State2();
    // check that rank is initiate_pending
    if (player_->DB().Stats().GuildRank(guild) != Guild::INITIATE_PENDING) {
      SECLOG(7, _T("%s: player %u: illegal attempt to ack initiate in guild %d"), method, player_->PlayerID(), guild);
      send_to_level = false;
    }
    else if (accept) { // accepted, update guild rank
      SECLOG(-7, _T("%s: player %u: accepted initiation into guild %d from player %u"), method,
	     player_->PlayerID(), guild, msg.ReceiverID());
      player_->ChangeGuildRank(guild, Guild::INITIATE);
      // update initiator in player db (both local and db copies)
      player_->Initiate(guild, msg.ReceiverID());
      int rc = main_->PlayerDBC()->SetInitiator(player_->PlayerID(), msg.ReceiverID(), guild);
      int sc = main_->PlayerDBC()->LastSQLCode();
      // int lt = main_->PlayerDBC()->LastCallTime();
      // main_->Log()->Debug(_T("%s: LmPlayerDBC::SetInitiator took %d ms"), method, lt);
      if (rc < 0) {
	TLOG_Warning(_T("%s: could not set initiator in guild %d for player %u; rc=%d, sqlcode=%d"), method,
		     guild, player_->PlayerID(), rc, sc);
	GsUtil::HandlePlayerError(main_, method, rc, sc, false);
      }
    }
    else { // didn't accept, reset guild rank to none (from pending)
      SECLOG(-7, _T("%s: player %u: rejected initiation into guild %d from player %u"), method,
	     player_->PlayerID(), guild, msg.ReceiverID());
      player_->ChangeGuildRank(guild, Guild::NO_RANK);
    }
  }
  break;

  // player attempting to knight someone in a guild
  case RMsg_PlayerMsg::KNIGHT: {            // guild, success
    int guild = msg.State1();
    // check that they can knight someone
    if (!player_->CanKnight(guild)) {
      SECLOG(7, _T("%s: player %u: illegal attempt to knight in guild %d"), method, player_->PlayerID(), guild);
      msg.SetState2(0); // set success to false
    }
    else {
      SECLOG(-7, _T("%s: player %u: knighting player %u in guild %d"), method,
	     player_->PlayerID(), msg.ReceiverID(), guild);
    }
  }
  break;

 
  // ascend to ruler
  case RMsg_PlayerMsg::ASCEND: {            // guild, not used (CS) / success (SC)
    int guild = msg.State1();
    bool success = false;
    if (player_->CanAscend(guild)) {
      player_->ChangeGuildRank(guild, Guild::RULER);
      success = true;
      SECLOG(-7, _T("%s: player %u: ascending to rulership in guild %d"), method, player_->PlayerID(), guild);
      // set xp pool value
      player_->SetPoolXP(guild, Lyra::RULER_XP_POOL);
      player_->SetQuestPoolXP(Lyra::QUEST_XP_POOL);
    }
    else {
      SECLOG(7, _T("%s: player %u: unsuccessful attempt to ascend in guild %d"), method, player_->PlayerID(), guild);
    }
    // send ack to client either way
    send_RMsg_PlayerMsg_Ascend(guild, success);
    send_to_level = false; // don't ever send to level server
  }
  break;

  // train self
  case RMsg_PlayerMsg::TRAIN_SELF: {            // art, not used (CS) / success (SC)
    int art = msg.State1();
    bool success = false;
    TCHAR names_buffer[512];
    if (player_->CanSelfTrain(art, (TCHAR*)names_buffer)) {
      success = player_->ChangeSkill(art, (player_->DB().Arts().Skill(art)) + 1, true);

      // now make up a list of all those who contributed to self training
      
      SECLOG(-7, _T("%s: player %u: self training in art %d to level"), method, player_->PlayerID(), art, player_->DB().Arts().Skill(art));
    }
    else {
      SECLOG(7, _T("%s: player %u: unsuccessful attempt to self train in art %d"), method, player_->PlayerID(), art);
    }
    // send ack to client either way
    send_RMsg_PlayerMsg_SelfTrain(art, success);
    send_to_level = false; // don't ever send to level server
  }
  break;



  // shield from being located
  case RMsg_PlayerMsg::MIND_BLANK: { // on/off, not used
    bool hide = msg.State1();
    // we can't do a skill check, because the client may send this message
	// when someone else has evoked mind blank on them
    hide_player(hide);
  }
  break;

  // save return location
  case RMsg_PlayerMsg::RETURN: {            // not used, not used
    // player have return art?
    if (player_->DB().Arts().Skill(Arts::RETURN) > 0) {
      // save player's room/level in return info
      player_->SaveReturnInfo();
    }
    else {
      SECLOG(4, _T("%s: player %u: illegal use of RETURN"), method, player_->PlayerID());
    }
    send_to_level = false; // don't ever send to level server
  }
  break;

  // save recall location
  case RMsg_PlayerMsg::RECALL: {            // not used, not used
    // player have return art?
    if (player_->DB().Arts().Skill(Arts::RECALL) > 0) {
      // save player's room/level in return info
      player_->SaveRecallInfo();
    }
    else {
      SECLOG(4, _T("%s: player %u: illegal use of RECALL"), method, player_->PlayerID());
    }
    send_to_level = false; // don't ever send to level server
  }
  break;

  // collapsed to soulsphere
  case RMsg_PlayerMsg::YOUGOTME: {           // victim's orbit/100 + nightmare index, DS at dissolution
    // determine how much XP to lose
    int xp_adj = - (int) ((double) player_->DB().Stats().XP() * 0.01); // lose up to 1%
    adjust_xp(xp_adj, _T("being dissolved by player"), msg.ReceiverID(), true);
    // fill in orbit (state1), DS field with "1", if player (monster/admin client will fill in fields)
    if (player_->DB().AccountType() == LmPlayerDB::ACCT_PLAYER)
      {
	msg.SetState1(player_->DB().Stats().Orbit());
	msg.SetState2(1);
      }
    player_->AddDeath();
  }
  break;

  case RMsg_PlayerMsg::VAMPIRIC_DRAW: {
  // player have art?
    if (player_->DB().Arts().Skill(Arts::VAMPIRIC_DRAW) > 0) {
      // player have head of target?
    }
    else {
      SECLOG(4, _T("%s: player %u: illegal use of VAMPIRIC_DRAW"), method, player_->PlayerID());
    }
    send_to_level = true;
  }
  break;


  // player attempting to obliterate Tehthu
  case RMsg_PlayerMsg::TEHTHUS_OBLIVION: {       // not used, not used
      int max_ds = player_->DB().Stats().MaxStat(Stats::DREAMSOUL);
	  max_ds -= 3;
      player_->ChangeMaxStat(Stats::DREAMSOUL, max_ds);
	  GMsg_ChangeStat changemsg;
	  changemsg.Init(1);
	  changemsg.InitChange(0, GMsg_ChangeStat::SET_STAT_MAX, Stats::DREAMSOUL, max_ds);
	  main_->OutputDispatch()->SendMessage(&changemsg, player_->Connection());
	  SECLOG(-8, _T("%s: player %u: evoking Tehthu's Oblivion"), method,
	       player_->PlayerID());
	  send_to_level = true;
  }
  break;

  // player attemptign to Rally someone
  case RMsg_PlayerMsg::RALLY: {
	  if (msg.SenderID() == msg.ReceiverID()) { // This is a RALLY ACK, so just clear rally info
		  player_->SaveSummonInfo(0, 0);
		  send_to_level = false;
		  break;
	  }

	  // check that player can use Rally, skill level doesn't matter
	  if (!(player_->CanUseArt(Arts::RALLY, 1))) {
		  if (player_->PPEvoking() == Arts::RALLY) { // we spent pp's to evoke Rally
			  player_->SetPPEvoking(Arts::NONE);
			  player_->SetPPSkill(0);
			  break;
		  }

		  int p_skill = player_->DB().Arts().Skill(Arts::RALLY);
		  SECLOG(4, _T("%s: player %u: attempt to use Rally art, own skill is %d"), method,
			      player_->PlayerID(), p_skill);
		      send_to_level = false;
	  }

	  // Rally not allowed on certain levels
	  for (int i = 0; i < num_no_rally_levels; i++) {
		  if (no_rally_levels[i] == player_->LevelID()) {
			  SECLOG(4, _T("%s: player %u: attempt to use Rally in illegal level %d"), method,
			      player_->PlayerID(), player_->LevelID());
			  send_to_level = false;
		  }
	  }
  }
  break;


  // No-Processing Messages (just forward to level server)
  //


  case RMsg_PlayerMsg::BLAST_ACK:           // not used, not used
  case RMsg_PlayerMsg::RANDOM:              // random number, random number
  case RMsg_PlayerMsg::NEWBIE_ENTERED:      // not used, not used
  case RMsg_PlayerMsg::VAMPIRIC_DRAW_ACK:   // amount, not used
  case RMsg_PlayerMsg::SPHERE_REPLY:   // amount, not used
  case RMsg_PlayerMsg::TEHTHUS_OBLIVION_ACK:		    // unused, unused 
    // do nothing
    break;

  //
  // Messages that shouldn't be sent from client
  //

  case RMsg_PlayerMsg::DREAMSTRIKE_ACK:     // success, not used
  case RMsg_PlayerMsg::DEMOTE_ACK:          // guild, number tokens used
  case RMsg_PlayerMsg::DEMOTE_FAIL:         // guild, number tokens needed
  case RMsg_PlayerMsg::PARTYKILL: {         // victim's orbit, nightmare index
    TLOG_Warning(_T("%s: player %u: sending illegal playermsg %d"), method, msg.SenderID(), msg.MsgType());
    send_to_level = false;
  }
  break;

  default: { // unknown
    TLOG_Warning(_T("%s: player %u: sending unknown playermsg %d"), method, msg.SenderID(), msg.MsgType());
    send_to_level = false;
  }
  break;

  } // end switch
  // create proxy message, copy message into it, send to player's level server
  if (send_to_level) {
    send_SMsg_Proxy(player_->LevelConnection(), msg);
  }
}
