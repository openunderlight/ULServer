// LsRoomThread3.cpp  -*- C++ -*-
// $Id: LsRoomThread3.cpp,v 1.34 1998-05-01 17:09:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// proxy handle_* methods

#include <stdio.h>
#include <list>

#include "LsRoomThread.h"
#include "LyraDefs.h"
#include "LmParty.h"
#include "LsMain.h"
#include "LmRoomDB.h"
#include "LmLevelDBC.h"
#include "LsLevelState.h"
#include "LmGlobalDB.h"
#include "RMsg.h"
#include "SMsg.h"
#include "RMsg_All.h"
#include "SMsg_All.h"
#include "GMsg_All.h"
#include "LsPlayer.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LsPlayerSet.h"
#include "LmSrvMesgBuf.h"
#include "LsRoomState.h"
#include "LsUtil.h"
#include "LmTimer.h"
#include "LsComputeGroups.h"
#include "LsMacros.h"
#include "LsPlayerList.h"
#include "LmLogFile.h"
#include "LmPlayerDB.h"
#include "LmItemDBC.h"
#include "LmRand.h"

DECLARE_TheFileName;

////
// handle_RMsg_Ping
////

void LsRoomThread::handle_RMsg_Ping(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Ping);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_Ping, true); // send error
  // send it right back
  LsUtil::Send_SMsg_Proxy(main_, source, msg);
}

////
// handle_RMsg_GetAvatarDescription
////

void LsRoomThread::handle_RMsg_GetAvatarDescription(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_GetAvatarDescription);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_GetAvatarDescription, true); // send error
  // find target player
  LsPlayer* tplayer = main_->PlayerSet()->GetPlayer(msg.PlayerID());
  if (!tplayer) {
    return; // don't send anything
  }
  // send back description
  send_RMsg_RcvAvatarDescription(source, msg.PlayerID(), tplayer->Description());
}

////
// handle_RMsg_ChangeAvatar
////

void LsRoomThread::handle_RMsg_ChangeAvatar(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_ChangeAvatar);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_ChangeAvatar, true); // send error
  // process
  //  TLOG_Debug(_T("%s: player %u changed avatar"), method, source->PlayerID());
  // change player's avatar
  source->SetAvatar(msg.Avatar());
  // get player's room
  LsRoomState* room = main_->LevelState()->RoomState(source->RoomID());
  if (!room) {
    TLOG_Error(_T("%s: could not get room %u, player %u"), method, source->RoomID(), source->PlayerID());
    // TODO: send error?
    return;
  }
  // get all players in room except source player
  LsPlayerList target_list;
  compute_PlayerList(room, target_list, source->PlayerID());
  // find players in source player's party
  compute_PartyList(source->Party(), target_list, source->PlayerID());
  // send message to them
  send_RMsg_ChangeAvatar(source, target_list);
}

////
// handle_RMsg_GetRoomDescription
////

void LsRoomThread::handle_RMsg_GetRoomDescription(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
	DEFMETHOD(LsRoomThread, handle_RMsg_GetRoomDescription);
	DECLARE_TheLineNum;
	PROXY_HANDLER_ENTRY(false);
	// accept message
	ACCEPT_PLAYERMSG(RMsg_GetRoomDescription, true); // send error
													 // look up description
	TCHAR rmDesc[Lyra::ROOMDESC_MAX];
	main_->LevelDBC()->RoomDescription(msg.LevelID(), msg.RoomID(), rmDesc);

	if (rmDesc[0] == _T('\0')) {
		return; // no description, don't send anything
	}
	// send back description
	send_RMsg_RoomDescription(source, msg.LevelID(), msg.RoomID(), rmDesc);
}

////
// handle_RMsg_GotoRoom
////

void LsRoomThread::handle_RMsg_GotoRoom(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_GotoRoom);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_GotoRoom, true); // send error
  // check target roomid
  if (!main_->LevelDBC()->ContainsRoom(msg.RoomID())) {
        TLOG_Warning(_T("%s: player %u attempted goto room %u pos=(%d,%d), not in level"), method,
		 source_id, msg.RoomID(), source->Position().X(), source->Position().Y());
    LsUtil::Send_RMsg_RoomLoginAck(main_, source, RMsg_LoginAck::LOGIN_ROOMNOTFOUND, 0);
    return;
  }
  // check that player is not going into same room
  if (source->RoomID() == msg.RoomID()) {
    // treat as a no-op
    //    TLOG_Warning(_T("%s: player %u going to same room %u pos=(%d,%d)"), method,
    //		 source_id, msg.RoomID(), source->Position().X(), source->Position().Y());
    return;
  }
  // get player's current room
  LsRoomState* room = main_->LevelState()->RoomState(source->RoomID());
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u, room not in level!"), method, source_id, source->RoomID());
    LsUtil::Send_RMsg_RoomLoginAck(main_, source, RMsg_RoomLoginAck::LOGIN_ERROR, 0);
    return;
  }
  // check if player is in room
  if (!room->HasPlayer(source_id)) {
    TLOG_Error(_T("%s: player %u not in room %u?"), method, source_id, source->RoomID());
    LsUtil::Send_RMsg_RoomLoginAck(main_, source, RMsg_RoomLoginAck::LOGIN_ERROR, 0);
    return;
  }
  // get target room state
  LsRoomState* troom = main_->LevelState()->RoomState(msg.RoomID());
  if (!troom) {
    TLOG_Error(_T("%s: room %u state not found!"), method, msg.RoomID());
    LsUtil::Send_RMsg_RoomLoginAck(main_, source, RMsg_RoomLoginAck::LOGIN_ERROR, 0);
    return;
  }
  // check if target room is full
  if (troom->IsFull()) {
    LsUtil::Send_RMsg_RoomLoginAck(main_, source, RMsg_RoomLoginAck::LOGIN_ROOMFULL, 0);
    return;
  }
  //  TLOG_Debug(_T("%s: player %u going from room %u to room %u"), method,
  //	     source_id, room->DB()->RoomID(), troom->DB()->RoomID());
  // leave current room
  perform_LeaveRoom(source, room, RMsg_LeaveRoom::NORMAL, msg.LastX(), msg.LastY());
  // update player room, position, other info
  source->GotoRoom(msg.RoomID(), msg.PeerUpdate());
  // enter target room
  perform_EnterRoom(source, troom, source->RoomID());
}

////
// handle_RMsg_Logout
////

void LsRoomThread::handle_RMsg_Logout(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Logout);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_Logout, true); // send error
  // process
  //TLOG_Debug(_T("%s: player %u logging out, status %c"), method, source_id, msg.Status());
  if ((source->AccountType() == LmPlayerDB::ACCT_PLAYER) ||
	  (source->AccountType() == LmPlayerDB::ACCT_ADMIN) ||
	  (source->AccountType() == LmPlayerDB::ACCT_PMARE))
	main_->ItemDBC()->ChangeNumDreamers(main_->LevelNum(), -1);

  perform_Logout(source, msg.Status());
  // if a normal player, add

}

////
// handle_RMsg_Party
////

void LsRoomThread::handle_RMsg_Party(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Party);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_Party, true); // send error
  // process
  //TLOG_Debug(_T("%s: received party message, type %c, playerid %u, source %u"), method,     msg.RequestType(), msg.PlayerID(), source->PlayerID());
  // switch on party request type
  switch (msg.RequestType()) {
  case RMsg_Party::JOIN:
    handle_RMsg_Party_Join(source, msg);
    break;
  case RMsg_Party::ACCEPT:
    handle_RMsg_Party_Accept(source, msg);
    break;
  case RMsg_Party::REJECT:
    handle_RMsg_Party_Reject(source, msg);
    break;
  case RMsg_Party::LEAVE:
    handle_RMsg_Party_Leave(source, msg);
    break;
  default:
    TLOG_Error(_T("%s: illegal party request %d"), method, msg.RequestType());
    LsUtil::Send_RMsg_Error(main_, source, RMsg::PARTY, _T("illegal party request"));
    break;
  }
}

////
// handle_RMsg_Party_Join
////

void LsRoomThread::handle_RMsg_Party_Join(LsPlayer* source, RMsg_Party& msg)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Party_Join);
  DECLARE_TheLineNum;

  lyra_id_t source_id = source->PlayerID();
  //TLOG_Debug(_T("%s: source %u (%p)"), method, source_id, source);
  // check that player is not already in a party
  if (!source->Party().IsEmpty()) {
    TLOG_Warning(_T("%s: player %u already in party"), method, source_id);
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_MUSTLEAVEPARTY);
    return;
  }
  // get target player (leader)
  lyra_id_t leaderid = msg.PlayerID();
  LsPlayer* leader = main_->PlayerSet()->GetPlayer(leaderid);
  // check that target player is in level
  if (!leader) {
    TLOG_Warning(_T("%s: leader %u not in level"), method, leaderid);
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_NOTINROOM, leaderid);
    return;
  }
  // check that target player is not an agent
  if ((leader->AccountType() == LmPlayerDB::ACCT_MONSTER) ||
      (leader->AccountType() == LmPlayerDB::ACCT_PMARE)) {
    TLOG_Warning(_T("%s: player %u attempting to join agent %u's party"), method, source_id, leaderid);
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_PARTYFULL);
    return;
  }
  // check that target player is leader of an existing party, or that
  // target player's party is empty
  if (!leader->Party().IsEmpty() && (leader->Party().LeaderID() != leaderid)) {
    //TLOG_Debug(_T("%s: target player %u not leader"), method, leaderid);
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_NOTLEADER, leader->Party().LeaderID());
    return;
  }
  // check that leader's party is not full
  if (leader->Party().IsFull()) {
    //TLOG_Debug(_T("%s: leader %u's party is full"), method, leaderid);
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_PARTYFULL);
    return;
  }
  // send request to leader (pass along response code, client uses it)
  send_RMsg_Party_Join(leader, source_id, msg.ResponseCode());
  // add source player to leader's join_request list
  leader->AddJoiner(source_id);
}

////
// handle_RMsg_Party_Accept
////

void LsRoomThread::handle_RMsg_Party_Accept(LsPlayer* source, RMsg_Party& msg)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Party_Accept);
  DECLARE_TheLineNum;

  lyra_id_t source_id = source->PlayerID(); // source is leader
  // get target player
  lyra_id_t tplayerid = msg.PlayerID();
  LsPlayer* tplayer = main_->PlayerSet()->GetPlayer(tplayerid);
  if (!tplayer) {
    TLOG_Warning(_T("%s: target player %u not in level"), method, tplayerid);
    // notify leader that player left level
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_LEFT, tplayerid);
    return;
  }
  // check that target player's party is empty
  if (!tplayer->Party().IsEmpty()) {
    send_RMsg_Party_Reject(source, RMsg_Party::REJECT_LEFT, tplayerid);
    send_RMsg_Party_Reject(tplayer, RMsg_Party::REJECT_MUSTLEAVEPARTY);
    return;
  }
  // check that source player is in party and is the leader of it, or that
  // source player's party is empty
  if (!source->Party().IsEmpty() && (source->Party().LeaderID() != source_id)) {
    TLOG_Error(_T("%s: source player %u is not leader of a party"), method, source_id);
    LsUtil::Send_RMsg_Error(main_, source, RMsg::PARTY, _T("party accept: player not party leader"));
    // send target player a reject message
    send_RMsg_Party_Reject(tplayer, RMsg_Party::REJECT_NOTINROOM);
    return;
  }
  // check that leader's party is not full
  if (source->Party().IsFull()) {
    TLOG_Error(_T("%s: leader %u's party is full, but accepted player %u"), method, source_id, tplayerid);
    LsUtil::Send_RMsg_Error(main_, source, RMsg::PARTY, _T("party accept: leader's party full"));
    // send target player a reject message
    send_RMsg_Party_Reject(tplayer, RMsg_Party::REJECT_PARTYFULL);
    return;
  }
  // At this point, we know that the leader is valid, and there is room in
  // the party.  In addition, the acceptee's party is empty.
  LmParty party;
  // check if leader's party is empty, and if so create one
  if (source->Party().IsEmpty()) {
    party.Create(source_id);
    source->SetPartyJoinTime(time(NULL));
  }
  else {
    party = source->Party();
  }
  // add new member to party members' parties, update new member's party
  party.AddPlayer(tplayerid);
  for (int i = 0; i < party.PartySize(); ++i) {
    lyra_id_t memberid = party.PlayerID(i);
    LsPlayer* member = main_->PlayerSet()->GetPlayer(memberid);
    if (!member) {
      TLOG_Error(_T("%s: stale party member %u in leader %u's party"), method, memberid, source_id);
    }
    else {
      member->SetParty(party);
    }
  }
  // send out "joined party" message to party members
  send_RMsg_Party_JoinedParty(tplayer);
  // send "party info" message to new member
  send_RMsg_PartyInfo(tplayer);
  // set new member's join time
  tplayer->SetPartyJoinTime(time(NULL));
  // remove player from leader's "join request" list
  source->RemoveJoiner(tplayerid);
}

////
// handle_RMsg_Party_Reject
////

void LsRoomThread::handle_RMsg_Party_Reject(LsPlayer* source, RMsg_Party& msg)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Party_Reject);
  DECLARE_TheLineNum;

  lyra_id_t source_id = source->PlayerID(); // source is party leader
  // check that target player is in level
  lyra_id_t tplayerid = msg.PlayerID();
  LsPlayer* tplayer = main_->PlayerSet()->GetPlayer(tplayerid);
  if (!tplayer) {
    TLOG_Warning(_T("%s: target player %u not in level"), method, tplayerid);
    return;
  }
  // check if target player had requested party membership
  if (!source->Joiners().HasMember(tplayerid)) {
    TLOG_Warning(_T("%s: target player %u not in leader's joiners"), method, tplayerid);
    // NOTE: continue
  }
  // send reject message to player
  send_RMsg_Party_Reject(tplayer, msg.ResponseCode(), source_id);
  // remove player from leader's "join request" list
  source->RemoveJoiner(tplayerid);

  // NOTE: the rest of this is debugging the client, to make sure it's
  // not doing anything it shouldn't be.

  // check that source player is in party and is the leader of it, or that
  // source player's party is empty
  if (!source->Party().IsEmpty() && (source->Party().LeaderID() != source_id)) {
    TLOG_Error(_T("%s: source player %u is not leader of party"), method, source_id);
  }
}

////
// handle_RMsg_Party_Leave
////

void LsRoomThread::handle_RMsg_Party_Leave(LsPlayer* source, RMsg_Party& /* msg */)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Party_Leave);
  DECLARE_TheLineNum;

  lyra_id_t source_id = source->PlayerID();
  // check that player is in a party
  if (source->Party().IsEmpty()) {
    TLOG_Error(_T("%s: player %u is not in a party"), method, source_id);
    //LsUtil::Send_RMsg_Error(main_, source, RMsg::PARTY, _T("leave party: player not in a party"));
    return;
  }
  // leave the party, notify members, etc
  perform_Party_Leave(source, RMsg_Party::LEAVE_NORMAL);
}

////
// handle_RMsg_Speech
////

void LsRoomThread::handle_RMsg_Speech(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_Speech);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_Speech, true); // send error
  // switch on speech type, compute target player list
  bool send_out = true;
  bool log_speech = true;
  bool show_whisper_emote = false;
  if (source->AccountType() == LmPlayerDB::ACCT_MONSTER) {
    log_speech = false; // don't log monster comments
  }
  LsPlayerList player_list;
  LsPlayerList emote_list;
  switch(msg.SpeechType()) {
  case RMsg_Speech::SYSTEM_SPEECH:
    log_speech = false;
    // fall through
  case RMsg_Speech::MONSTER_SPEECH:
  case RMsg_Speech::SPEECH:
  case RMsg_Speech::EMOTE:
    compute_RMsg_Speech_Speech(source, player_list, msg);
    break;
  case RMsg_Speech::RAW_EMOTE:
  case RMsg_Speech::SHOUT:
    compute_RMsg_Speech_Shout(source, player_list, msg);
    break;
  case RMsg_Speech::TELL_IP:
  case RMsg_Speech::SYSTEM_WHISPER:
    log_speech = false;
    // fall through
  case RMsg_Speech::WHISPER:
    compute_RMsg_Speech_Whisper(source, player_list, msg);
    break;
  case RMsg_Speech::WHISPER_EMOTE:
    compute_RMsg_Speech_WhisperEmote(source, player_list, msg);
    break;
  case RMsg_Speech::PARTY:
    compute_PartyList(source->Party(), player_list, source->PlayerID());
    break;
  case RMsg_Speech::GLOBALSHOUT:
    compute_RMsg_Speech_GlobalShout(source, player_list, msg);
    break;
  case RMsg_Speech::REPORT_BUG: // handled in game server
  case RMsg_Speech::RP:			 // handled in game server
  case RMsg_Speech::REPORT_DEBUG: // handled in game server
  case RMsg_Speech::REPORT_QUEST: // handled in game server
  case RMsg_Speech::REPORT_CHEAT: // handled in game server
  case RMsg_Speech::SERVER_TEXT: // handled in game server
    TLOG_Warning(_T("%s: illegal speech type %c"), msg.SpeechType());
    send_out = false; // no targets
    log_speech = false;
    break;
  default:
    TLOG_Error(_T("%s: unknown speech type %d"), method, msg.SpeechType());
    LsUtil::Send_RMsg_Error(main_, source, RMsg::SPEECH, _T("unknown speech type"));
    return;
    break;
  }
  // send speech to the target players
  if (send_out) {
    send_RMsg_Speech(source, player_list, msg);
  }
  // log
  if (log_speech) {
    msg.RemoveNewlines();
    main_->SpeechLog()->Speech(_T("player %u [level=%u room=%u]: %c (%u): %s"),
			       source->PlayerID(), main_->LevelDBC()->LevelID(), source->RoomID(),
			       msg.SpeechType(), msg.PlayerID(), msg.SpeechText());
  }
}


////
// handle_RMsg_PlayerMsg
////

void LsRoomThread::handle_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf, LsPlayer* source)
{
  DEFMETHOD(LsRoomThread, handle_RMsg_PlayerMsg);
  DECLARE_TheLineNum;
  PROXY_HANDLER_ENTRY(false);
  // accept message
  ACCEPT_PLAYERMSG(RMsg_PlayerMsg, true); // send error
  // get target list
  lyra_id_t targetid = msg.ReceiverID();
  LsPlayerList targets;
  LsPlayer* target = 0;
  bool send_out = true;
  LsRoomState* room = main_->LevelState()->RoomState(source->RoomID());
  if (targetid == Lyra::ID_UNKNOWN) {  // if it's 0, broadcast to all players in room
    if (!room) {
      TLOG_Error(_T("%s: no room state for player %u, room %u"), method, source_id, source->RoomID());
    }
    else {
		// if we're a party affect art, compute only the party
		if (msg.MsgType() == RMsg_PlayerMsg::HEALING_AURA) {
			compute_PartyList(source->Party(), targets, source_id);
		} else {
		// get list of players in room, except for source
			compute_PlayerList(room, targets, source_id);
		}
    }
  }
  else {  // otherwise, target is a single player in the level
    target = main_->PlayerSet()->GetPlayer(targetid);
    if (!target) {
      // TLOG_Warning(_T("%s: target player %u not in level"), method, targetid);
    }
    else {
      targets.push_back(target);
    }
  }
  // any processing to do?
  switch (msg.MsgType()) {

  // generate and send out a random number
  case RMsg_PlayerMsg::RANDOM: {
    msg.SetState1(LmRand::Generate(1, 100)); // update random number
    msg.SetState2(LmRand::Generate(1, 100)); // update random number
    targets.push_back(source); // add source player to list of targets
  }
  break;

  // handle player dissolution
  case RMsg_PlayerMsg::YOUGOTME: {
    if (room) {
      // create and drop essence talisman of the right type
  // MODIFIED: before 5/01, put the killer's ID (targetid) into slaver_id
  // now it puts the collapsed player/mare's ID there
      perform_create_essence(source, room, source_id, msg.State1(), msg.State2());
    }
    // if source=target, then it's a result of a player dropping own dreamsoul, no output PlayerMsg
    if (source_id == targetid) {
      send_out = false;
    }
    else if (target) {
      // if target (killer) is in a party, then don't send YOUGOTME, send PARTYKILL to party members
      if (target->Party().PartySize() > 0) {
        send_out = false; // don't send original message to receiver, will do here
	msg.Init(msg.SenderID(), 0, RMsg_PlayerMsg::PARTYKILL, msg.State1(), 0);
	LmParty party = target->Party();
	for (int i = 0; i < party.PartySize(); ++i) {
	  lyra_id_t memberid = party.PlayerID(i);
	  LsPlayer* member = main_->PlayerSet()->GetPlayer(memberid);
	  if (member) {
	    msg.SetReceiverID(memberid);
	    if (memberid == targetid) {
	      msg.SetState2(100 + party.PartySize()); // killer share; indicate by 100 + members in state2
	    }
	    else {
	      msg.SetState2(party.PartySize()); // member share
	    }
	    LsUtil::Send_SMsg_Proxy(main_, member, msg);
	  }
	} // end for
      } // end party
    } // end target
  }
  break;

  // handle part of dreamstrike
  case RMsg_PlayerMsg::DREAMSTRIKE: {
    bool strike_ok = true; // default, source can dreamstrike target (gs checks all else)
    // check that target player is in room
    if (!target) {
      send_out = false;
      strike_ok = false;
    }
    else {
      // check that target player is a soulsphere
      LmPeerUpdate update = target->PlayerUpdate();
      if (!update.FlagSet(LmPeerUpdate::LG_SOULSPHERE)) {
	strike_ok = false;
      }
    }
    // update success flag in message for target
    msg.SetState1(strike_ok);
    // send ack back to source
    send_RMsg_PlayerMsg_DreamStrikeAck(source, msg.ReceiverID(), strike_ok);
    // if was struck, drop soul essence
    if (strike_ok && room) {
      perform_create_soulessence(target, room, msg.SenderID());
    }
  };
  break;

  // player evading location?
  case RMsg_PlayerMsg::MIND_BLANK: { // on/off, unused
    source->SetHidden(msg.State1());
    send_out = false;
  }
  break;

  // player mindblanking the target?
  case RMsg_PlayerMsg::MIND_BLANK_OTHER: { // skill, not used
    // target still there?
    if (target) {
      //TLOG_Debug(_T("%s: player %u hiding player %u"), method, msg.SenderID(), msg.ReceiverID());
      //target->SetHidden(true);
    }
    else { // target not found, don't continue
      send_out = false;
    }
  }
  break;

  // handle part of dreamstrike
  case RMsg_PlayerMsg::SUMMON_PRIME: {
	  bool success = false;
	  int roomid = 0;
	  const LmItemHdr* primehdr = NULL;
	  LmItem prime;
	  //LmRoomItem dummy;
	  int max_rooms = main_->LevelDBC()->NumRooms();
	  for (roomid = 0; roomid < max_rooms; roomid++) {
		LsRoomState* primeroom = main_->LevelState()->Room(roomid);
		if (!primeroom) {
		  continue;
		}
		primehdr = primeroom->HasPrime(msg.State1());
		if (primehdr) {
			prime = primeroom->GetItem(*primehdr);
			//primeroom->RemoveItem(*primehdr);
			//room->MakeSpaceForItem(*primehdr, dummy);
			//dummy.Init(prime, source->Position(), 9999999);
			//room->AddItem(dummy);
			// now announce the item
			//LmRoomItemList roomitemlist;
			//roomitemlist.push_back(dummy);
		   // update item's ownership in database, transfer to player
			lyra_id_t serial = primehdr->Serial();
			// update item's full state in database before giving to player
			int rc = main_->ItemDBC()->UpdateItemFullState(prime);
			int sqlcode = main_->ItemDBC()->LastSQLCode();

			rc = main_->ItemDBC()->UpdateItemOwnership(serial, LmItemDBC::OWNER_PLAYER, source_id, 0);
			sqlcode = main_->ItemDBC()->LastSQLCode();
			  if (rc < 0) {
				TLOG_Warning(_T("%s: player %u could not summon prime item %d; rc=%d, sql=%d"), method, source_id, serial, rc, sqlcode);
				break;
			}

			perform_ItemPickup(source, primeroom, prime);

			success = true;
			break;
		}
	}
	msg.SetState2(success);
    send_out = true; 
 }
  break;


  default:
    // nothing, just pass along to target
    break;

  } // end switch
  // send result out?
  if (!send_out) {
    return;
  }
  // quick check
  if (targets.size() == 0) {
    // TLOG_Warning(_T("%s: no targets found"), method);
    return;
  }
  // send message to targets
  LsUtil::Send_SMsg_Proxy(main_, targets, msg);
}
