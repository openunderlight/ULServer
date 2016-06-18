// LsRoomThread1.cpp  -*- C++ -*-
// $Id: LsRoomThread1.cpp,v 1.42 1998-05-01 17:09:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// send_* functions

#include <stdio.h>
#include <list>

#include "LsRoomThread.h"
#include "LmConnection.h"
#include "LmConnectionSet.h"
#include "LmGlobalDB.h"
#include "LmLevelDBC.h"
#include "LmParty.h"
#include "LmRoomDB.h"
#include "LmSockAddrInet.h"
#include "LmSocket.h"
#include "LsLevelState.h"
#include "LsMain.h"
#include "LsOutputDispatch.h"
#include "LsPlayer.h"
#include "LsPlayerList.h"
#include "LsPlayerSet.h"
#include "LmRoomItem.h"
#include "LmRoomItemList.h"
#include "LsRoomState.h"
#include "LsUtil.h"
#include "RMsg.h"
#include "SMsg.h"
#include "GMsg_All.h"
#include "RMsg_All.h"
#include "SMsg_All.h"
#include "LmPlayerDB.h"

DECLARE_TheFileName;

////
// broadcast_room - send message from given player to all other players in room,
//   optionally sending to source player; if player is null, then all players
//   in room are targeted
////

void LsRoomThread::broadcast_room(LsRoomState* room, LsPlayer* source, LmMesg& msg, bool sendself)
{
  DECLARE_TheLineNum;
  // if source is null, there is no source player
  lyra_id_t source_id = Lyra::ID_UNKNOWN;
  if (source) {
    source_id = source->PlayerID();
  }
  // initialize list of targets: people in room except for player sending message
  LsPlayerList player_list;
  compute_PlayerList(room, player_list, source_id);
  if (sendself && source) {
    player_list.push_back(source);
  }
  // send message
  LsUtil::Send_SMsg_Proxy(main_, player_list, msg);
}

////
// broadcast_party - send message from given player to all other players in party
//   optionally sending to source player
////

void LsRoomThread::broadcast_party(const LmParty& party, LsPlayer* source, LmMesg& msg, bool sendself)
{
  DECLARE_TheLineNum;
  lyra_id_t source_id = source->PlayerID();
  // initialize list of targets: people in room except for player sending message
  LsPlayerList player_list;
  compute_PartyList(party, player_list, source_id);
  if (sendself) {
    player_list.push_back(source);
  }
  // send message
  LsUtil::Send_SMsg_Proxy(main_, player_list, msg);
}

////
// send_GMsg_LevelPlayers
////

void LsRoomThread::send_GMsg_LevelPlayers(LmConnection* gsconn, lyra_id_t playerid)
{
  DEFMETHOD(LsRoomThread, send_GMsg_LevelPlayers);
  DECLARE_TheLineNum;

  int num_rooms = main_->LevelDBC()->NumRooms();
  // init message
  GMsg_LevelPlayers msg;
  msg.Init(main_->LevelDBC()->LevelID(), num_rooms);
  // for each room, get # of players
  for (int i = 0; i < num_rooms; ++i) {
    LsRoomState* room = main_->LevelState()->Room(i);
    if (!room) {
      TLOG_Error(_T("%s: room number %d not found"), method, i);
    }
    else {
      msg.SetRoomID(i, room->DB()->RoomID());
      // msg.SetNumPlayers(i, room->NumPlayers());
      msg.SetNumPlayers(i, room->NumPlayers(LmPlayerDB::ACCT_PLAYER));
      msg.SetNumAgents(i, room->NumPlayers(LmPlayerDB::ACCT_MONSTER));
	  //TLOG_Debug("Room %d has %d agents!", room->DB()->RoomID(), room->NumPlayers(LmPlayerDB::ACCT_MONSTER));
    }
  }
  // send it out
  LsUtil::Send_SMsg_Proxy(main_, gsconn, playerid, msg);
}

////
// send_RMsg_Party_Reject
////

void LsRoomThread::send_RMsg_Party_Reject(LsPlayer* target, int responsecode, lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  // init message
  RMsg_Party msg_party;
  msg_party.InitReject(playerid, responsecode);
  // send it
  LsUtil::Send_SMsg_Proxy(main_, target, msg_party);
}

////
// send_RMsg_Party_Join
////

void LsRoomThread::send_RMsg_Party_Join(LsPlayer* target, lyra_id_t new_playerid, int responsecode)
{
  DECLARE_TheLineNum;
  // init message
  RMsg_Party msg_party;
  msg_party.InitJoin(new_playerid, responsecode);
  // send it
  LsUtil::Send_SMsg_Proxy(main_, target, msg_party);
}

////
// send_RMsg_Party_Leave
////

void LsRoomThread::send_RMsg_Party_Leave(LsPlayer* player, int how)
{
  DECLARE_TheLineNum;
  lyra_id_t playerid = player->PlayerID();
  // initialize message
  RMsg_Party msg_party;
  msg_party.InitLeave(playerid, how);
  // send message to party members
  broadcast_party(player->Party(), player, msg_party);
}

////
// send_RMsg_Party_JoinedParty
////

void LsRoomThread::send_RMsg_Party_JoinedParty(LsPlayer* newplayer)
{
  DECLARE_TheLineNum;
  // initialize remote player info
  RmRemotePlayer rp;
  newplayer->CopyInfo(rp);
  // initialize message
  RMsg_JoinedParty msg_joinedparty;
  msg_joinedparty.Init(rp);
  // send message to party members
  broadcast_party(newplayer->Party(), newplayer, msg_joinedparty);
}

////
// send_RMsg_PartyInfo
////

void LsRoomThread::send_RMsg_PartyInfo(LsPlayer* target)
{
  DECLARE_TheLineNum;
  LmParty party = target->Party();
  // initialize message
  RMsg_PartyInfo msg_partyinfo;
  msg_partyinfo.Init(party.LeaderID(), party.PartySize());
  for (int i = 0; i < party.PartySize(); ++i) {
    lyra_id_t memberid = party.PlayerID(i);
    LsPlayer* member = main_->PlayerSet()->GetPlayer(memberid);
    if (!member) {
      TLOG_Error(_T("send_RMsg_PartyInfo: could not get party member %u!"), memberid);
      continue;
    }
    RmRemotePlayer rp;
    member->CopyInfo(rp);
    msg_partyinfo.SetPartyMember(i, rp);
  }
  // send to new player
  LsUtil::Send_SMsg_Proxy(main_, target, msg_partyinfo);
}

////
// send_RMsg_LeaveRoom
////

void LsRoomThread::send_RMsg_LeaveRoom(LsPlayer* player, LsRoomState* room, int how, int lastx, int lasty)
{
  DECLARE_TheLineNum;
  lyra_id_t playerid = player->PlayerID();
  // initialize message
  RMsg_LeaveRoom msg_leaveroom;
  msg_leaveroom.Init(playerid, how, lastx, lasty);
  // send message to players in room
  broadcast_room(room, player, msg_leaveroom);
}

////
// send_RMsg_EnterRoom
////

void LsRoomThread::send_RMsg_EnterRoom(LsPlayer* player, LsRoomState* room)
{
  DECLARE_TheLineNum;
  //static const TCHAR* method = "send_RMsg_EnterRoom";
  lyra_id_t playerid = player->PlayerID();
  // initialize remote player info
  RmRemotePlayer rp;
  player->CopyInfo(rp);

  // initialize message; new player
  RMsg_EnterRoom msg_enterroom;
  msg_enterroom.Init(1);
  msg_enterroom.SetPlayer(0, rp);
  // send message to players in room
  broadcast_room(room, player, msg_enterroom);

  // initialize message; current room members
  LsPlayerList player_list;
  compute_PlayerList(room, player_list, playerid);
  msg_enterroom.Init(player_list.size());
  // initialize remote player list
  int rpnum = 0;
  for (LsPlayerList::iterator i = player_list.begin(); !(bool)(i == player_list.end()); ++i) {
    RmRemotePlayer rp2;
    (*i)->CopyInfo(rp2);
    msg_enterroom.SetPlayer(rpnum, rp2);
    ++rpnum;
  }
  // send list of players to new player, if there are any
  if (rpnum > 0) {
    LsUtil::Send_SMsg_Proxy(main_, player, msg_enterroom);
  }
}

////
// send_RMsg_Speech
////

void LsRoomThread::send_RMsg_Speech(LsPlayer* player, LsPlayerList& t_list, RMsg_Speech& omsg)
{
  DECLARE_TheLineNum;
  // create message to be sent out
  RMsg_Speech msg_speech;
  //  TLOG_Debug(_T("XXXXX: babble = %u"), omsg.Babble());

  msg_speech.Init(omsg.SpeechType(), player->PlayerID(), omsg.Babble(), omsg.SpeechText());
  // send out to targets
  LsUtil::Send_SMsg_Proxy(main_, t_list, msg_speech);
}

////
// send_RMsg_ItemPickup
////

void LsRoomThread::send_RMsg_ItemPickup(LsPlayer* player, LsRoomState* room, const LmItem& item)
{
  DECLARE_TheLineNum;
  // picking up a single item
  LmRoomItemList items;
  LmRoomItem roomitem;
  roomitem.Init(item, LmPosition::DEFAULT_INSTANCE, 0);
  items.push_back(roomitem);
  send_RMsg_ItemPickup(player, room, items);
}

void LsRoomThread::send_RMsg_ItemPickup(LsPlayer* player, LsRoomState* room, const LmRoomItemList& items)
{
  DECLARE_TheLineNum;
  // check list length
  if (items.size() == 0) {
    return;
  }
  // initialize message
  RMsg_ItemPickup msg_itempickup;
  msg_itempickup.Init(items.size());
  int itemnum = 0;
  for (LmRoomItemList::const_iterator i = items.begin(); !(bool)(i == items.end()); ++i) {
    msg_itempickup.SetItemHeader(itemnum, (*i).Item().Header());
    ++itemnum;
  }
  // send message to players in room except for one who picked items up
  broadcast_room(room, player, msg_itempickup);
}

////
// send_RMsg_ItemDrop
////

void LsRoomThread::send_RMsg_ItemDrop(LsPlayer* player, LsRoomState* room,
				      const LmRoomItemList& items, bool bcast)
{
  DECLARE_TheLineNum;
  // check list length
  // even if there are no items, we send a full item message
  if (items.size() == 0) {
    return;
  }
  // if player is null, then consider item to be generated; if bcast is false,
  // items are existing room items; if player is non-null, and bcast is true,
  // item(s) were dropped by the given player
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  if (player && bcast) {
    playerid = player->PlayerID();
  }
  // initialize "header only" itemdrop message
  RMsg_ItemHdrDrop msg_itemhdrdrop;
  msg_itemhdrdrop.Init(0);
  int hdr_item = 0;
  // initialize "full" itemdrop message
  RMsg_ItemDrop msg_itemdrop;
  msg_itemdrop.Init(0);
  int full_item = 0;
  // iterate through items, adding to one message or the other
  for (LmRoomItemList::const_iterator i = items.begin(); !(bool)(i == items.end()); ++i) {
    LmRoomItem item = *i;
    if (!item.Item().Header().FlagSet(LyraItem::FLAG_SENDSTATE)) {
      msg_itemhdrdrop.SetItemHeader(hdr_item, item.Item().Header(), item.Position());
      ++hdr_item;
    }
    else {
      msg_itemdrop.SetItem(full_item, item.Item(), item.Position());
      ++full_item;
    }
  }
  // update the message sizes
  msg_itemdrop.SetNumItems(full_item);
  msg_itemhdrdrop.SetNumItems(hdr_item);
  // if broadcast, send to players in room except for one who dropped item
  if (bcast) {
    if (full_item > 0) {
      broadcast_room(room, player, msg_itemdrop);
    }
    if (hdr_item > 0) {
      broadcast_room(room, player, msg_itemhdrdrop);
    }
  }
  // else, only send to player
  else {
    // if (full_item > 0) {
    // always send this, so that client knows what items are in room (wards, in particular)
    LsUtil::Send_SMsg_Proxy(main_, player, msg_itemdrop);
    // }
    if (hdr_item > 0) {
      LsUtil::Send_SMsg_Proxy(main_, player, msg_itemhdrdrop);
    }
  }
}

void LsRoomThread::send_RMsg_ItemDrop(LsPlayer* player, LsRoomState* room, const LmRoomItem& item)
{
  DECLARE_TheLineNum;
  // player dropping a single item
  LmRoomItemList items;
  items.push_back(item);
  send_RMsg_ItemDrop(player, room, items, true);
}

////
// send_RMsg_ChangeAvatar
////

void LsRoomThread::send_RMsg_ChangeAvatar(LsPlayer* source, LsPlayerList& target_list)
{
  DECLARE_TheLineNum;
  RMsg_ChangeAvatar msg;
  msg.Init(source->PlayerID(), source->Avatar());
  LsUtil::Send_SMsg_Proxy(main_, target_list, msg);
}

////
// send_RcvAvatarDescription
////

void LsRoomThread::send_RMsg_RcvAvatarDescription(LsPlayer* player, lyra_id_t targetid, const TCHAR* targetdesc)
{
  RMsg_RcvAvatarDescription msg;
  msg.Init(targetid, targetdesc);
  LsUtil::Send_SMsg_Proxy(main_, player, msg);
}

////
// send_RMsg_RoomDescription
////

void LsRoomThread::send_RMsg_RoomDescription(LsPlayer* player, short levelid, short roomid, const TCHAR * roomdesc)
{
		RMsg_RoomDescription msg;
		msg.Init(levelid, roomid, roomdesc);
		LsUtil::Send_SMsg_Proxy(main_, player, msg);
}

////
// send_RMsg_PlayerMsg_DreamStrikeAck
////

void LsRoomThread::send_RMsg_PlayerMsg_DreamStrikeAck(LsPlayer* player, lyra_id_t target, bool success)
{
  RMsg_PlayerMsg msg;
  // target is the target of the DS, player is the person who cast it; this ack comes from
  // the person who was the target
  msg.Init(target, player->PlayerID(), RMsg_PlayerMsg::DREAMSTRIKE_ACK);
  msg.SetState1(success);
  LsUtil::Send_SMsg_Proxy(main_, player, msg);
}

////
// send_SMsg_ItemPickup
////

void LsRoomThread::send_SMsg_ItemPickup(LsPlayer* player, const LmItem& item, int status)
{
  DECLARE_TheLineNum;
  if (!player) {
    return;
  }
  SMsg_ItemPickup msg_itempickup;
  msg_itempickup.Init(player->PlayerID(), item, status);
  main_->OutputDispatch()->SendMessage(&msg_itempickup, player->Connection());
}

////
// send_SMsg_ItemDrop
////

void LsRoomThread::send_SMsg_ItemDrop(LsPlayer* player, const LmItem& item, int status)
{
  DECLARE_TheLineNum;
  if (!player) {
    return;
  }
  SMsg_ItemDrop msg_itemdrop;
  msg_itemdrop.Init(player->PlayerID(), item.Header(), status);
  main_->OutputDispatch()->SendMessage(&msg_itemdrop, player->Connection());
}

////
// send_SMsg_PartyLeader
////

void LsRoomThread::send_SMsg_PartyLeader(LmConnection* gsconn, lyra_id_t playerid, int leader_time)
{
  SMsg_PartyLeader msg;
  msg.Init(playerid, leader_time);
  main_->OutputDispatch()->SendMessage(&msg, gsconn);
}

////
// send_SMsg_GiveItemAck
////

void LsRoomThread::send_SMsg_GiveItemAck(LmConnection* conn, lyra_id_t sourceid, lyra_id_t targetid,
					 const LmItemHdr& hdr, int status)
{
  SMsg_GiveItemAck msg;
  msg.Init(status, sourceid, targetid, hdr);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_SMsg_TakeItemAck
////

void LsRoomThread::send_SMsg_TakeItemAck(LmConnection* conn, lyra_id_t sourceid, lyra_id_t targetid,
					 const LmItemHdr& hdr, int status)
{
  SMsg_TakeItemAck msg;
  msg.Init(status, sourceid, targetid, hdr);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}
