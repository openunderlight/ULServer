// LsRoomThread2.cpp  -*- C++ -*-
// $Id: LsRoomThread2.cpp,v 1.39 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// auxiliary methods (perform_*, compute_*)

#include <stdio.h>
#include <list>
#include <cstdlib>

#include "LsRoomThread.h"
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
#include "LsRoomState.h"
#include "LmRoomItem.h"
#include "LmRoomItemList.h"
#include "LsUtil.h"
#include "LsSignalThread.h"
#include "LmThreadPool.h"
#include "LsPlayerList.h"
#include "LmItemDBC.h"
#include "LmItemGen.h"
#include "LmRand.h"

DECLARE_TheFileName;

////
// perform_Logout
////

void LsRoomThread::perform_Logout(LsPlayer* player, int how)
{
  DEFMETHOD(LsRoomThread, perform_Logout);
  DECLARE_TheLineNum;

  lyra_id_t playerid = player->PlayerID();
  lyra_id_t roomid = player->RoomID();

  // notify player's party that player left party by logging out
  if (!player->Party().IsEmpty()) {
	  if (how == RMsg_Logout::FINAL)
		perform_Party_Leave(player, RMsg_Party::LEAVE_LOGOUT, false);
	  else
		perform_Party_Leave(player, RMsg_Party::LEAVE_LOGOUT, true);
  }

  // notify players in room that player left, leave room
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u, in room %u not found in level!"), method, playerid, roomid);
    LsUtil::Send_RMsg_Error(main_, player, RMsg::LOGOUT, _T("room %u not in level"), roomid);
    // NOTE: don't return, continue
  }
  else {
    LmPosition lastpos = player->Position();
    perform_LeaveRoom(player, room, how, lastpos.X(), lastpos.Y());
  }

  // send reject messages to any players in the "join request" list
  const LmIdSet& joiners = player->Joiners();
  for (int i = 0; i < joiners.Size(); ++i) {
    lyra_id_t joinid = joiners.Member(i);
    LsPlayer* joiner = main_->PlayerSet()->GetPlayer(joinid);
    if (!joiner) {
      TLOG_Warning(_T("%s: could not get joiner %u"), method, joinid);
      continue;
    }
    //    TLOG_Debug(_T("%s: sending reject to player %u from player %u"), method, playerid, joinid);
    send_RMsg_Party_Reject(joiner, RMsg_Party::REJECT_NOTINROOM, playerid);
  }

  // if player was giving items, fake GiveItemAcks back to the player's game server and remove
  // any items from the takers' taking lists
  LsGiveTakeItemList give = player->GivingList();
  if (give.size() > 0) {
    for (LsGiveTakeItemList::iterator it = give.begin(); !(bool)(it == give.end()); ++it) {
      lyra_id_t takerid = (*it).playerid;
      LmItem& item = (*it).item;
      LsPlayer* taker = main_->PlayerSet()->GetPlayer(takerid);
      if (!taker) {
	TLOG_Warning(_T("%s: could not get taker %u"), method, takerid);
	continue;
      }
      taker->TookItem(item.Header());
      send_SMsg_GiveItemAck(player->Connection(), takerid, playerid, item.Header(), GMsg_GiveItemAck::GIVE_NO);
    }
  }

  // if player was taking any items, fake GiveItemAcks to giver, and remove items from givers' giving lists
  LsGiveTakeItemList take = player->TakingList();
  if (take.size() > 0) {
    for (LsGiveTakeItemList::iterator it = take.begin(); !(bool)(it == take.end()); ++it) {
      lyra_id_t giverid = (*it).playerid;
      LmItem& item = (*it).item;
      LsPlayer* giver = main_->PlayerSet()->GetPlayer(giverid);
      if (!giver) {
	TLOG_Warning(_T("%s: could not get giver %u"), method, giverid);
	continue;
      }
      giver->GaveItem(item.Header());
      send_SMsg_GiveItemAck(giver->Connection(), playerid, giverid, item.Header(),
			    GMsg_GiveItemAck::GIVE_NO);
    }
  }

  // remove player from active player set
  main_->PlayerSet()->RemovePlayer(player);

  // clear player object
  // remove from realtime ID array
  main_->LevelState()->FreeRealtimeID(player->RealtimeID());
  player->Logout();
}

////
// perform_LeaveRoom
////

void LsRoomThread::perform_LeaveRoom(LsPlayer* player, LsRoomState* room, int how, int lastx, int lasty)
{
  DECLARE_TheLineNum;
  // notify players in room that player left
  send_RMsg_LeaveRoom(player, room, how, lastx, lasty);
  // remove player from level
  main_->LevelState()->RemovePlayer(player->PlayerID(), player->RealtimeID());
}

////
// perform_EnterRoom
////

void LsRoomThread::perform_EnterRoom(LsPlayer* player, LsRoomState* room, int last_room)
{
  DECLARE_TheLineNum;

  // ack room entry (if we do this after room entry, we are counted as a neighbor)
  LsUtil::Send_RMsg_RoomLoginAck(main_, player, RMsg_RoomLoginAck::LOGIN_OK, room->NumPlayers());

  // add player to room
  room->AddPlayer(player);

  SMsg_LS_Action msg;

#if 0 // BMP: DISABLED 10/8/04 DUE TO LAG ISSUES
  // recalculate groups for last room
  if ((last_room > 0) && (last_room != player->RoomID())) {
	msg.Init(SMsg_LS_Action::ACTION_COMPUTE_GROUPS, last_room);
	if (LsUtil::SendInternalMessage(main_, msg, LsMain::THREAD_ROOMSERVER) < 0) {
		TLOG_Error(_T("On enter room, could not send compute_groups message to room thread") );
	}
  }

  msg.Init(SMsg_LS_Action::ACTION_COMPUTE_GROUPS, player->RoomID());
  if (LsUtil::SendInternalMessage(main_, msg, LsMain::THREAD_ROOMSERVER) < 0) {
	TLOG_Error(_T("On enter room, could not send compute_groups message to room thread") );
  }
#endif

  //TLOG_Warning(_T("added player %d, rtid %d"), player->PlayerID(), player->RealtimeID());

  // send player the list of items in room
  send_RMsg_ItemDrop(player, room, room->ItemList(), false);
  // notify players in room that player entered; send player the room occupants
  send_RMsg_EnterRoom(player, room);
}

////
// perform_Party_Leave
////

void LsRoomThread::perform_Party_Leave(LsPlayer* player, int how, bool grant_xp)
{
  DEFMETHOD(LsRoomThread, perform_Party_Leave);
  DECLARE_TheLineNum;

  lyra_id_t playerid = player->PlayerID();
  LmParty party = player->Party();
  // double-check
  if (party.IsEmpty()) {
    TLOG_Warning(_T("%s: called with empty party"), method);
    return;
  }
  lyra_id_t leaderid = party.LeaderID();
  lyra_id_t creatorid = party.CreatorID();
  bool leader_left = false;
  // send "left party" message to members of party
  send_RMsg_Party_Leave(player, how);
  // if not the party leader, deal with giving xp to party's original leader (creator)
  if (leaderid != playerid) {
    LsPlayer* leader = main_->PlayerSet()->GetPlayer(leaderid);
    if (leader && (leaderid == creatorid)) { // leader is creator
      // get time player was in party
      int member_time = LmUtil::TimeSince(player->PartyJoinTime());
      // TLOG_Debug(_T("%s: party membership time is %d"), method, member_time);
      // send it to the game server
      if (member_time > 0) {
	send_SMsg_PartyLeader(leader->Connection(), leaderid, member_time);
      }
    }
  }
  else { // is the party leader
    leader_left = true;
    if (playerid == creatorid) { // player created and led party
      int leader_time = 0;
      // sum up all member times
      for (int i = 0; i < party.PartySize(); ++i) {
	lyra_id_t memberid = party.PlayerID(i);
	if (memberid == playerid) {
	  continue;
	}
	LsPlayer* member = main_->PlayerSet()->GetPlayer(memberid);
	if (!member) {
	  TLOG_Error(_T("%s: party member %u not in level!"), method, memberid);
	  continue;
	}
	leader_time += LmUtil::TimeSince(member->PartyJoinTime());
      }
      // send it to the game server
      if ((leader_time > 0) && (grant_xp)) {
		send_SMsg_PartyLeader(player->Connection(), leaderid, leader_time);
      }
    }
  }
  // remove player from party members parties
  for (int i = 0; i < party.PartySize(); ++i) {
    lyra_id_t memberid = party.PlayerID(i);
    if (memberid == playerid) {
      continue;
    }

    LsPlayer* member = main_->PlayerSet()->GetPlayer(memberid);
    if (!member) {
      TLOG_Error(_T("%s: party member %u not in level!"), method, memberid);
      continue;
    }
    
    if(player->ChannelTarget() == memberid)
    {
        RMsg_PlayerMsg chmsg;
        chmsg.Init(playerid, memberid, RMsg_PlayerMsg::CHANNEL, 0, 0);
        LsUtil::Send_SMsg_Proxy(main_, member, chmsg);
    }
    // remove; if leader_left, then clear the party instead
    if (leader_left) {
      member->EmptyParty();
    }
    else {
      member->RemovePartyMember(playerid);
    }
  }
  // clear player's party
  player->EmptyParty();
}

////
// perform_ItemPickup
////

void LsRoomThread::perform_ItemPickup(LsPlayer* player, LsRoomState* room, const LmItem& item)
{
  DECLARE_TheLineNum;
  // remove item from room
  room->RemoveItem(item.Header());
  // get playerid of player taking item (null conn means item is being reaped)
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  if (player) {
    playerid = player->PlayerID();
    // notify game server if it's a player taking item
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_OK);
  }
  // notify room members, except for player taking item
  send_RMsg_ItemPickup(player, room, item);
}

////
// perform_ItemDestroy
////

void LsRoomThread::perform_ItemDestroy(LsRoomState* room, const LmItem& item)
{
  DECLARE_TheLineNum;
  // remove item from room
  room->RemoveItem(item.Header());
  // notify all room members
  send_RMsg_ItemPickup(0, room, item);
  // put serial number back in free pool
  main_->LevelState()->Serials().FreeSerial(item.Serial());
}

////
// perform_ItemDrop - called when a player drops a single item into a room, or when
//   items are generated
////

void LsRoomThread::perform_ItemDrop(LsPlayer* player, LsRoomState* room, const LmRoomItem& ritem)
{
  DECLARE_TheLineNum;
  // add item to room
  if (!room->CanAddItem()) { // make space for essence
    LmRoomItem reaped;
    room->MakeSpaceForItem(ritem.Item().Header(), reaped);
    if (reaped.Item().Serial() != 0) {
      // destroy room item
     TLOG_Warning(_T("perform_ItemDrop: item %u reaped to make room for essence"), reaped.Item().Serial());
      perform_ItemDestroy(room, reaped.Item());
    }
  }

  room->AddItem(ritem);
  // get playerid of player dropping item
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  if (player) {
    playerid = player->PlayerID();
    // notify game server
    send_SMsg_ItemDrop(player, ritem.Item(), SMsg_ItemDrop::DROP_OK);
  }
  // notify room members, except for player dropping item
  send_RMsg_ItemDrop(player, room, ritem);
}

////
// perform_ItemReap
////

void LsRoomThread::perform_ItemReap(LsRoomState* room)
{
  DEFMETHOD(LsRoomThread, perform_ItemReap);
  DECLARE_TheLineNum;
  // TEMP: don't reap anything
  // return;
  // get list of items to be reaped
  LmRoomItemList reaped;
  room->ReapItems(reaped);
  // if any items were reaped, send out itempickup message
  if (reaped.size() > 0) {
    LmRoomItemList::iterator i;
    // remove from room
    for (i = reaped.begin(); !(bool)(i == reaped.end()); ++i) {
      const LmItem& item = (*i).Item();
      room->RemoveItem(item.Header());
      // if item has a description, destroy it
      if (item.FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
	int rc = main_->ItemDBC()->DeleteItem(item.Serial());
	int sc = main_->ItemDBC()->LastSQLCode();
	if (rc < 0) {
	  LsUtil::HandleItemError(main_, method, rc, sc);
	  // continue
	}
      }
      else {
	// reclaim serial number for re-use
	main_->LevelState()->Serials().FreeSerial(item.Serial());
      }
    }
    // send messages to room members
    send_RMsg_ItemPickup(0, room, reaped);
  }
}

////
// perform_ItemGenerate
////

void LsRoomThread::perform_ItemGenerate(LsRoomState* room)
{
  DECLARE_TheLineNum;
  DEFMETHOD(LsRoomThread, perform_ItemGenerate);
  // get list of items to be generated
  LmRoomItemList gen;

  if (!room->CanAddItem()) {
      TLOG_Debug(_T("%s: not able to generate items - room full of items"), method);
      return;
  }

  room->GenerateItems(gen);
  // if any items were generated, send out itemdrop message
  if (gen.size() > 0) {
    // add items to room
    for (LmRoomItemList::iterator i = gen.begin(); !(bool)(i == gen.end()); ++i) {
      room->AddItem(*i);
    }
    // notify room members of new items
    send_RMsg_ItemDrop(0, room, gen, true);
  }
}

////
// perform_create_essence
////

void LsRoomThread::perform_create_essence(LsPlayer* player, LsRoomState* room, lyra_id_t slaverid,
					  int state1, int state2)
{
  DEFMETHOD(LsRoomThread, perform_create_essence);
  DECLARE_TheLineNum;
  int item_ttl = 0;
  TCHAR itemname[40]; // allow overflow

  if (!room->CanAddItem()) {
      TLOG_Debug(_T("%s: not able to create essence for player %u - room full of items"), method, player->PlayerID());
      return;
  }

  // item position
  LmPosition pos = player->Position();
  // create item header
  int serial = main_->LevelState()->Serials().GetNextSerial();
  LmItemHdr ihdr;
  ihdr.Init(0, serial);
  // TODO: set colors based on avatar color?
  ihdr.SetColor1(0);
  ihdr.SetColor2(0);
  ihdr.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION)));
  // create essence object, fill in values
  lyra_item_essence_t essence = { LyraItem::ESSENCE_FUNCTION, 0, 0, 0, 0 };
  essence.strength = state2;
  essence.weapon_type = 0;
  essence.slaver_id = slaverid;
#ifdef Ul3D
  ihdr.SetGraphic(player->Avatar().EssenceModel());
#else
  ihdr.SetGraphic(player->Avatar().EssenceBitmap());
#endif
  // for players, player mares, and dark mares
  if ((state1 < 100) || (state1 > 150)) { // player: state1 = orbit, state2 = DS
    item_ttl = 60; 
    ihdr.SetFlags(LyraItem::FLAG_IMMUTABLE);
   _stprintf(itemname, _T("%s"), player->PlayerName());
    
    essence.mare_type = player->Avatar().AvatarType();

  }
  else { // nightmare: state1 - 100 = nightmare index, state2 = DS
    item_ttl = 60; 
    ihdr.SetFlags(0);
    essence.mare_type = state1 - 100;
// *** STRING LITERAL ***  
   _stprintf(itemname, _T("%s Essence"), player->PlayerName());
  }
  // create item state, copy info
  LmItem item;
  item.Init(ihdr, itemname);
  item.SetStateField(0, &essence, sizeof(essence));
  item.SetCharges(1);
  // copy info into a room item
  LmRoomItem roomitem;
  roomitem.Init(item, pos, item_ttl);
  // drop item in room, notify clients
  // TLOG_Debug(_T("%s: essence being dropped:"), method);
  // roomitem.Dump(logf_.Stream(), 2); logf_.FlushLog();
  perform_ItemDrop(0, room, roomitem);

  // if it's a mare of any kind, also spawn an item
  if (state1 > 100) { // player: state1 = orbit, state2 = DS
	  int num_items = 1;
	  int item_type = player->Avatar().AvatarType()-1;
	  if (player->Avatar().AvatarType() == Avatars::HORRON) {
		  num_items = 2; 
		  item_type = item_type - 1;
	  }
	  if (state1 > 200) { // dark mares drop more items
          switch (player->Avatar().AvatarType()){
              case Avatars::EMPHANT:
                  num_items = 2;
                  break;
              case Avatars::BOGROM:
                  num_items = 4;
                  break;
              case Avatars::AGOKNIGHT:
                  num_items = 6;
                  break;
              case Avatars::SHAMBLIX:
                  num_items = 8;
                  break;
              case Avatars::HORRON:
                  num_items = 10;
                  break;
              default:
                  num_items = 2;
                  break;
          }

					
      }

      for (int i = 0; i<num_items; i++) {
		  	perform_spawn_mare_item(player, room, item_type, state1 > 200 ? get_random_neighbor_position(room, player) : player->Position());
	  }
  }
}

/////
// get_random_neighbor_position
/////
const LmPosition& LsRoomThread::get_random_neighbor_position(LsRoomState* room, LsPlayer* player)
{
	unsigned int distance = LmRand::Generate(0, room->PlayerList().size() - 1);		
	std::list<lyra_id_t>::const_iterator li = room->PlayerList().begin();
	std::advance(li, distance);
	LsPlayer* p = main_->PlayerSet()->GetPlayer((*li));
	if(p && !p->Avatar().Hidden())
		return p->Position();
	else
		return player->Position();
}

////
// perform_create_soulessence
////

void LsRoomThread::perform_create_soulessence(LsPlayer* player, LsRoomState* room, lyra_id_t killer)
{
  DEFMETHOD(LsRoomThread, perform_create_soulessence);
  int item_ttl = 0;
  TCHAR itemname[40]; // allow overflow

  if (!room->CanAddItem()) {
      TLOG_Debug(_T("%s: not able to create soul essence for player %u - room full"), method, player->PlayerID());
      return;
  }

  // item position
  LmPosition pos = player->Position();
  // create item header
  int serial = main_->LevelState()->Serials().GetNextSerial();
  LmItemHdr ihdr;
  ihdr.Init(0, serial);
  // TODO: set colors based on avatar color?
  ihdr.SetColor1(0);
  ihdr.SetColor2(0);
  ihdr.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::ESSENCE_FUNCTION)));
  // create essence object, fill in values
  lyra_item_essence_t essence = { LyraItem::ESSENCE_FUNCTION, 0, 0, 0, 0 };
  essence.strength = 99;
  essence.weapon_type = 0;
  essence.slaver_id = killer;
#ifdef Ul3D
  ihdr.SetGraphic(0); //LyraBitmap::SOUL_ESSENCE);
#else
  ihdr.SetGraphic(LyraBitmap::SOUL_ESSENCE);
#endif
  item_ttl = 300; 
  ihdr.SetFlags(LyraItem::FLAG_IMMUTABLE | LyraItem::FLAG_ALWAYS_DROP | LyraItem::FLAG_NOREAP);
 _stprintf(itemname, _T("%s"), player->PlayerName());
  if (player->Avatar().AvatarType() == Avatars::MALE) {
    essence.mare_type = Avatars::MALE;
  }
  else if (player->Avatar().AvatarType() == Avatars::FEMALE) {
    essence.mare_type = Avatars::FEMALE;
  }
  else {
    essence.mare_type = 0;
  }
  // create item state, copy info
  LmItem item;
  item.Init(ihdr, itemname);
  item.SetStateField(0, &essence, sizeof(essence));
  item.SetCharges(1);
  // copy info into a room item
  LmRoomItem roomitem;
  roomitem.Init(item, pos, item_ttl);
  // drop item in room, notify clients
  // TLOG_Debug(_T("%s: essence being dropped:"), method);
  // roomitem.Dump(logf_.Stream(), 2); logf_.FlushLog();
  perform_ItemDrop(0, room, roomitem);
}

////
// perform_spawn_mare_item
////

void LsRoomThread::perform_spawn_mare_item(LsPlayer* player, LsRoomState* room, int item_type, const LmPosition& spawnPoint)
{
  DEFMETHOD(LsRoomThread, perform_spawn_mare_item);
  DECLARE_TheLineNum;
  int item_ttl = 0;
  
  if (!room->CanAddItem()) {
	  TLOG_Debug(_T("%s: not able to create essence for player %u - room full of items"), method, player->PlayerID());
	  return;
  }
  
  // item position
  LmPosition pos = player->Position();
  // create item header
  int serial = main_->LevelState()->Serials().GetNextSerial();
  
  LmRoomItem roomitem;
  LmItem it; 
  LmItemGen::GenerateItem(item_type, LmItemGen::ITEM_ANY, it);
  it.Header().SetSerial(serial);

  TLOG_Debug(_T("%s: created item %s for gen type %d with %d charges"), "Agent Drop", it.Name(), item_type, it.Charges());

  int ttl = 300; 	
  roomitem.Init(it, spawnPoint, ttl);
  
  // drop item in room, notify clients
  // TLOG_Debug(_T("%s: essence being dropped:"), method);
  // roomitem.Dump(logf_.Stream(), 2); logf_.FlushLog();
  perform_ItemDrop(0, room, roomitem);
  
}


////
// compute_PlayerList
////

void LsRoomThread::compute_PlayerList(LsRoomState* room, LsPlayerList& player_list, lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  // add to player_list all players in given room, except for given playerid
  std::list<lyra_id_t> players = room->PlayerList();
  std::list<lyra_id_t>::iterator li;
  for (li = players.begin(); !(bool)(li == players.end()); ++li) {
    lyra_id_t targetid = (*li);
    if (playerid != targetid) {
      LsPlayer* rplayer = main_->PlayerSet()->GetPlayer(targetid);
      if (!rplayer) {
	TLOG_Warning(_T("compute_PlayerList: player %u not found in room"), targetid);
      }
      else {
	player_list.push_back(rplayer);
      }
    }
  }
}

////
// compute_PartyList
////

void LsRoomThread::compute_PartyList(const LmParty& party, LsPlayerList& player_list, lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  // add to conn_list all players in given party, except for given playerid
  for (int i = 0; i < party.PartySize(); ++i) {
    lyra_id_t targetid = party.PlayerID(i);
    if (targetid != playerid) {
      LsPlayer* rplayer = main_->PlayerSet()->GetPlayer(targetid);
      if (!rplayer) {
	TLOG_Warning(_T("compute_PartyList: player %u not found in level"), targetid);
      }
      else {
	player_list.push_back(rplayer);
      }
    }
  }
}

////
// compute_RMsg_Speech_Speech
////

void LsRoomThread::compute_RMsg_Speech_Speech(LsPlayer* player, LsPlayerList& target_list,
					      RMsg_Speech& /* msg */)
{
  DEFMETHOD(LsRoomThread, compute_RMsg_Speech_Speech);
  DECLARE_TheLineNum;

  lyra_id_t playerid = player->PlayerID();
  lyra_id_t roomid = player->RoomID();
  // get room state
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u not in level"), method, playerid, roomid);
    LsUtil::Send_RMsg_Error(main_, player, RMsg::SPEECH, _T("room %u not found in level"), roomid);
    return;
  }
  // find players in room that are close enough to hear speech
  std::list<lyra_id_t> players = room->PlayerList();
  std::list<lyra_id_t>::iterator li;
  for (li = players.begin(); !(bool)(li == players.end()); ++li) {
    lyra_id_t targetid = (*li);
    // don't send speech back to self
    if (playerid == targetid) {
      continue;
    }
    LsPlayer* rplayer = main_->PlayerSet()->GetPlayer(targetid);
    if (!rplayer) {
      TLOG_Warning(_T("%s: player %u not found in room"), method, targetid);
    }
    else {
      if (player->Position().DistanceXY2(rplayer->Position()) < LsMain::SPEECH_DIST2) {
	target_list.push_back(rplayer);
      }
    }
  }
  // find players in source player's party
  compute_PartyList(player->Party(), target_list, playerid);
}

////
// compute_RMsg_Speech_Shout
////

void LsRoomThread::compute_RMsg_Speech_Shout(LsPlayer* player, LsPlayerList& target_list,
					     RMsg_Speech& /* msg */)
{
  DEFMETHOD(LsRoomThread, compute_RMsg_Speech_Shout);
  DECLARE_TheLineNum;

  lyra_id_t playerid = player->PlayerID();
  lyra_id_t roomid = player->RoomID();
  // get room state
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u not in level"), method, playerid, roomid);
    LsUtil::Send_RMsg_Error(main_, player, RMsg::SPEECH, _T("room %u not found in level"), roomid);
    return;
  }
  // get all players in room except source player
  compute_PlayerList(room, target_list, playerid);
  // find players in source player's party
  compute_PartyList(player->Party(), target_list, playerid);
}

////
// compute_RMsg_Speech_Whisper
////

void LsRoomThread::compute_RMsg_Speech_Whisper(LsPlayer* player, LsPlayerList& target_list, RMsg_Speech& msg)
{
  DEFMETHOD(LsRoomThread, compute_RMsg_Speech_Whisper);
  DECLARE_TheLineNum;

  lyra_id_t roomid = player->RoomID();
  lyra_id_t playerid = player->PlayerID();
  // get room state
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u, not found in level"), method, playerid, roomid);
    LsUtil::Send_RMsg_Error(main_, player, RMsg::SPEECH, _T("room %u not found in level"), roomid);
    return;
  }
  // find target player
  LsPlayer* rplayer = main_->PlayerSet()->GetPlayer(msg.PlayerID());
  if (!rplayer) {
    TLOG_Warning(_T("%s: target player %u not found in room"), method, msg.PlayerID());
    return;
  }
  target_list.push_back(rplayer);
}

////
// compute_RMsg_Speech_Whisper_Emote
////

void LsRoomThread::compute_RMsg_Speech_WhisperEmote(LsPlayer* player, LsPlayerList& target_list, RMsg_Speech& msg)
{
  DEFMETHOD(LsRoomThread, compute_RMsg_Speech_WhisperEmote);
  DECLARE_TheLineNum;

  lyra_id_t roomid = player->RoomID();
  lyra_id_t playerid = player->PlayerID();
  // get room state
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u, not found in level"), method, playerid, roomid);
    LsUtil::Send_RMsg_Error(main_, player, RMsg::SPEECH, _T("room %u not found in level"), roomid);
    return;
  }

  // find players in room that are close enough to hear speech
  std::list<lyra_id_t> players = room->PlayerList();
  std::list<lyra_id_t>::iterator li;
  for (li = players.begin(); !(bool)(li == players.end()); ++li) {
    lyra_id_t targetid = (*li);
    // don't send speech back to self
    if (playerid == targetid) {
      continue;
    }
    LsPlayer* rplayer = main_->PlayerSet()->GetPlayer(targetid);
    if (!rplayer) {
      TLOG_Warning(_T("%s: player %u not found in room"), method, targetid);
    }
    else {
      if ((player->Position().DistanceXY2(rplayer->Position()) < LsMain::SPEECH_DIST2) &&
		  (targetid != msg.PlayerID()))  {
		target_list.push_back(rplayer);
      }
    }
  }

}


////
// compute_RMsg_Speech_GlobalShout
////

void LsRoomThread::compute_RMsg_Speech_GlobalShout(LsPlayer* /* player */, LsPlayerList& target_list,
						   RMsg_Speech& /* msg */)
{
  DECLARE_TheLineNum;
  // get all players in level
  main_->PlayerSet()->GetPlayerList(target_list);
}
