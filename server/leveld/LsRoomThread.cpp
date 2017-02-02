// LsRoomThread.cpp  -*- C++ -*-
// $Id: LsRoomThread.cpp,v 1.69 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsRoomThread.h"
#endif

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
#include "GMsg.h"
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
#include "LmRoomItem.h"
#include "LsUtil.h"
#include "LmTimer.h"
#include "LsComputeGroups.h"
#include "LsMacros.h"
#include "LyraMessage.h"
#include "LmItemDBC.h"
#include "LsOutputDispatch.h"
#include "LmPlayerDB.h"

#ifndef USE_INLINE
#include "LsRoomThread.i"
#endif

////
// Constructor
////

LsRoomThread::LsRoomThread(LsMain* lsmain)
  : LmThread(lsmain->BufferPool(), lsmain->Log() /* &logf_ */ ),
    main_(lsmain)
{
  open_log();
  register_handlers();
}

////
// Destructor
////

LsRoomThread::~LsRoomThread()
{
  close_log();
}

////
// Run
////

void LsRoomThread::Run()
{
  LmUtil::BlockAllSignals();
  LmThread::Run();
}

////
// Dump
////

void LsRoomThread::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsRoomThread[%p,%d]: main=[%p]>\n"), this, sizeof(LsRoomThread), main_);
  LmThread::Dump(f, indent + 1);
}

////
// open_log
////

void LsRoomThread::open_log()
{
  // logf_.Init("ls", "room", main_->LevelDBC()->LevelID());
  // logf_.Open(main_->GlobalDB()->LogDir());
}

////
// close_log
////

void LsRoomThread::close_log()
{
  // logf_.Close();
}

////
// register_handlers
////


void LsRoomThread::register_handlers()
{
  // default message handler
  SetDefaultHandler((MsgHandler) &LsRoomThread::handle_Default);

  // register SMsg message handlers
  RegisterHandler(SMsg::GETITEM, (MsgHandler)&LsRoomThread::handle_SMsg_GetItem);
  RegisterHandler(SMsg::PUTITEM, (MsgHandler)&LsRoomThread::handle_SMsg_PutItem);
  RegisterHandler(SMsg::PROXY, (MsgHandler)&LsRoomThread::handle_SMsg_Proxy);
  RegisterHandler(SMsg::DESTROYROOMITEM, (MsgHandler)&LsRoomThread::handle_SMsg_DestroyRoomItem);
  RegisterHandler(SMsg::GETLEVELPLAYERS, (MsgHandler)&LsRoomThread::handle_SMsg_GetLevelPlayers);
  RegisterHandler(SMsg::GIVEITEM, (MsgHandler)&LsRoomThread::handle_SMsg_GiveItem);
  RegisterHandler(SMsg::TAKEITEMACK, (MsgHandler)&LsRoomThread::handle_SMsg_TakeItemAck);
  RegisterHandler(SMsg::LOCATEAVATAR, (MsgHandler)&LsRoomThread::handle_SMsg_LocateAvatar);
  RegisterHandler(SMsg::SHOWITEM, (MsgHandler)&LsRoomThread::handle_SMsg_ShowItem);
  RegisterHandler(SMsg::SETAVATARDESCRIPTION, (MsgHandler)&LsRoomThread::handle_SMsg_SetAvatarDescription);

  // register SMsg_LS message handlers
  RegisterHandler(SMsg::LS_LOGIN, (MsgHandler)&LsRoomThread::handle_SMsg_LS_Login);
  RegisterHandler(SMsg::LS_ACTION, (MsgHandler)&LsRoomThread::handle_SMsg_LS_Action);
}


////
// handle_Default
////

void LsRoomThread::handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_Default);
  HANDLER_ENTRY(false);
  // print error message to log
  TLOG_Error(_T("%s: unknown message type %d, size %d received"), method, msg_type, msg_size);
}

////
// handle_SMsg_GetItem
////

void LsRoomThread::handle_SMsg_GetItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_GetItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_GetItem, true); // send error
  // process
  lyra_id_t playerid = msg.PlayerID();
  lyra_id_t roomid = msg.RoomID();
  // the item in question
  LmItem item;
  item.Init(msg.ItemHeader());
  // get player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: player %u not in level"), method, playerid);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::GETITEM, _T("player not in level"));
    return;
  }
  // check player's connection
  if (player->Connection() != conn) {
    TLOG_Error(_T("%s: player %u conn [%p] not incoming conn [%p]"), method, playerid, player->Connection(), conn);
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_ERROR); // send this anyway
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::GETITEM, _T("connection mismatch"));
    return;
  }
  // get room
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: room %u not in level"), method, playerid, roomid);
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_ERROR); // send this anyway
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::GETITEM, _T("room not found in level"));
    return;
  }
  // check if room contains item
  if (!room->HasItem(msg.ItemHeader())) {
    //    TLOG_Debug(_T("%s: player %u could not pick up item - not in room"), method, playerid);
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_ERROR);
    return;
  }
  // TODO: check that player is close enough to item?
  //  TLOG_Debug(_T("%s: player %u picking up item"), method, playerid);
  // get actual item into tmpitem
  item = room->GetItem(msg.ItemHeader());
  int serial = item.Header().Serial();
  // update item's full state in database before giving to player
  int rc = main_->ItemDBC()->UpdateItemFullState(item);
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::UpdateItemFullState took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: could not update fullstate of item %d; rc=%d, sql=%d"), method, serial, rc, sqlcode);
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_ERROR);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return;
  }
  // update item's ownership in database, transfer to player
  rc = main_->ItemDBC()->UpdateItemOwnership(serial, LmItemDBC::OWNER_PLAYER, playerid, 0);
  sqlcode = main_->ItemDBC()->LastSQLCode();
  // lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::UpdateItemOwnership took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: player %u could not take item %d; rc=%d, sql=%d"), method, playerid, serial, rc, sqlcode);
    send_SMsg_ItemPickup(player, item, SMsg_ItemPickup::PICKUP_ERROR);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return;
  }
  // remove item from room, send out messages to players except for one getting item
  perform_ItemPickup(player, room, item);
}

////
// handle_SMsg_PutItem
////

void LsRoomThread::handle_SMsg_PutItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_PutItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_PutItem, true); // send error
  // process
  lyra_id_t playerid = msg.PlayerID();
  lyra_id_t roomid = msg.RoomID();
  LmItem item = msg.Item();
  // get player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) { // players not in the room can still drop items
    TLOG_Warning(_T("%s: player %u not in level, dropping item in room %u"), method, playerid, roomid);
    //LsUtil::Send_SMsg_Error(main_, conn, SMsg::PUTITEM, _T("player not in level"));
    //return;
  }
  // get room
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: player %u dropping item in room %u not in level"), method, playerid, roomid);
    send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::PUTITEM, _T("room not found in level"));
    return;
  }
  // if room has "noreapitems" flag set, then check that item is not a ward
  if (room->DB()->NoReapItems()) {
    // wards are single-function items
    if ((item.NumFields() == 1) && (LyraItem::StateFunction(item.StateField(0)) == LyraItem::WARD_FUNCTION)) {
      TLOG_Warning(_T("%s: player %u tried to drop a ward in a noreap room"), method, playerid);
      send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
      return;
    }
  }
  // if playerid is 0, then item is an "auto-dropped" item, and MUST be placed in the room
  if (playerid != Lyra::ID_UNKNOWN) { // playerid != 0, could reject drop
    // check player's connection
    if (player && (player->Connection() != conn)) {
      TLOG_Error(_T("%s: player %u conn [%p] not incoming conn [%p]"), method, playerid, player->Connection(), conn);
      send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
      LsUtil::Send_SMsg_Error(main_, conn, SMsg::PUTITEM, _T("connection mismatch"));
      return;
    }
    // check if room contains item
    if (room->HasItem(item.Header())) {
      TLOG_Debug(_T("%s: player %u not able to drop item - already in room"), method, playerid);
      send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
      return;
    }
    // check if room cannot contain any more items
    if (!room->CanAddItem()) {
      TLOG_Debug(_T("%s: player %u not able to drop item - room full"), method, playerid);
      send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
      return;
    }
    TLOG_Debug(_T("%s: player %u dropping item"), method, playerid);
  }
  else { // playerid is 0, must drop
    LmRoomItem reaped;
    room->MakeSpaceForItem(item.Header(), reaped);
    if (reaped.Item().Serial() != 0) {
      // destroy room item
      perform_ItemDestroy(room, reaped.Item());
    }
  }
  // update ownership, transfer to room
  int serial = item.Header().Serial();
  int rc = main_->ItemDBC()->UpdateItemOwnership(serial, LmItemDBC::OWNER_ROOM, main_->LevelDBC()->LevelID(), room->DB()->RoomID());
  int sqlcode = main_->ItemDBC()->LastSQLCode();
  // int lt = main_->ItemDBC()->LastCallTime();
  // main_->Log()->Debug(_T("%s: LmItemDBC::UpdateItemOwnership took %d ms"), method, lt);
  if (rc < 0) {
    TLOG_Warning(_T("%s: player %u could not drop item %d; rc=%d, sql=%d"), method, playerid, serial, rc, sqlcode);
    send_SMsg_ItemDrop(player, item, SMsg_ItemDrop::DROP_ERROR);
    LsUtil::HandleItemError(main_, method, rc, sqlcode);
    return;
  }
  // create new room item
  LmRoomItem roomitem;
  roomitem.Init(msg.Item(), msg.Position(), msg.Lifetime());
  // put item in room, send out messages to players except for one dropping item
  perform_ItemDrop(player, room, roomitem);
}

////
// handle_SMsg_DestroyRoomItem
////

void LsRoomThread::handle_SMsg_DestroyRoomItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_DestroyRoomItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_DestroyRoomItem, true); // send error
  // process
  lyra_id_t playerid = msg.PlayerID();
  lyra_id_t roomid = msg.RoomID();
  // get player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: player %u not in level"), method, playerid);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::DESTROYROOMITEM, _T("player not in level"));
    return;
  }
  // check player's connection
  if (player->Connection() != conn) {
    TLOG_Error(_T("%s: player %u conn [%p] not incoming conn [%p]"), method, playerid, player->Connection(), conn);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::DESTROYROOMITEM, _T("connection mismatch"));
    return;
  }
  // get room
  LsRoomState* room = main_->LevelState()->RoomState(roomid);
  if (!room) {
    TLOG_Error(_T("%s: room %u not in level"), method, playerid, roomid);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::DESTROYROOMITEM, _T("room not found in level"));
    return;
  }
  // check if room contains item (may have been destroy by someone else earlier)
  if (!room->HasItem(msg.ItemHeader())) {
    TLOG_Debug(_T("%s: player %u could not destroy item - not in room"), method, playerid);
    return;
  }
  // TLOG_Debug(_T("%s: player %u destroying item"), method, playerid);
  // get item state
  LmItem item = room->GetItem(msg.ItemHeader());
  // remove item from room, send out messages to all players in room
  perform_ItemDestroy(room, item);
}

////
// handle_SMsg_SetAvatarDescription
////

void LsRoomThread::handle_SMsg_SetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_SetAvatarDescription);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_SetAvatarDescription, true); // send error
  // process
  lyra_id_t playerid = msg.PlayerID();
  // get player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: player %u not in level"), method, playerid);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::SETAVATARDESCRIPTION, _T("player not in level"));
    return;
  }
  // check player's connection
  if (player->Connection() != conn) {
    TLOG_Error(_T("%s: player %u conn [%p] not incoming conn [%p]"), method, playerid, player->Connection(), conn);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::SETAVATARDESCRIPTION, _T("connection mismatch"));
    return;
  }
  // update player
  player->SetDescription(msg.Description());
}

////
// handle_SMsg_Proxy
////

void LsRoomThread::handle_SMsg_Proxy(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_Proxy);
  HANDLER_ENTRY(false);
  // pre-conditions
  // if conn is null, message was internally generated (ie. faked)
  bool send_error = false;
  if (conn) {
    CHECK_CONN_ISGAME();
    send_error = true;
  }
  // accept message
  ACCEPT_MSG(SMsg_Proxy, send_error);
  // check proxy type (should be PROXY_PROCESS)
  if (msg.ProxyType() != SMsg_Proxy::PROXY_PROCESS) {
    TLOG_Error(_T("%s: incorrect proxy type '%c'"), method, msg.ProxyType());
    if (send_error) {
      LsUtil::Send_SMsg_Error(main_, conn, SMsg::PROXY, _T("incorrect proxy type"));
    }
    return;
  }
  // re-use msgbuf (no need to up the usage count, since the handle_RMsg methods don't decrement it)
  msg.CopyToBuffer(*msgbuf);
  // get source player
  LsPlayer* player = main_->PlayerSet()->GetPlayer(msg.PlayerID());
  if (!player) {
    TLOG_Error(_T("%s: source player %u not in level"), method, msg.PlayerID());
    if (send_error) {
      LsUtil::Send_SMsg_Error(main_, conn, SMsg::PROXY, _T("source player not in level"));
    }
    return;
  }
  // check that player's connection is same as one message came over, if the conn is non-null
  if (conn && (player->Connection() != conn)) {
    TLOG_Warning(_T("%s: message from player %u received from wrong connection %p"), method, msg.PlayerID(), conn);
    LsUtil::Send_SMsg_Error(main_, conn, SMsg::PROXY, _T("player/connection mismatch"));
    return;
  }
#if 0
  TLOG_Debug(_T("%s: dispatching proxied message, type %s (%d), length %d"), method,
	     LyraMessage::MessageName(msg.EnclosedMessageType()),
	     msg.EnclosedMessageType(), msg.EnclosedMessageSize());
#endif
  // dispatch
  switch (msg.EnclosedMessageType()) {
  case RMsg::GOTOROOM:
    handle_RMsg_GotoRoom(msgbuf, player);
    break;
  case RMsg::LOGOUT:
    handle_RMsg_Logout(msgbuf, player);
    break;
  case RMsg::PARTY:
    handle_RMsg_Party(msgbuf, player);
    break;
  case RMsg::SPEECH:
    handle_RMsg_Speech(msgbuf, player);
    break;
  case RMsg::PLAYERMSG:
    handle_RMsg_PlayerMsg(msgbuf, player);
    break;
  case RMsg::PING:
    handle_RMsg_Ping(msgbuf, player);
    break;
  case RMsg::CHANGEAVATAR:
    handle_RMsg_ChangeAvatar(msgbuf, player);
    break;
  case RMsg::GETAVATARDESCRIPTION:
    handle_RMsg_GetAvatarDescription(msgbuf, player);
    break;
  case RMsg::GETROOMDESCRIPTION:
	  handle_RMsg_GetRoomDescription(msgbuf, player);
	  break;
  default:
    TLOG_Error(_T("%s: unknown proxied message type %d"), method, msg.EnclosedMessageType());
    break;
  }
}

////
// handle_SMsg_GetLevelPlayers
////

void LsRoomThread::handle_SMsg_GetLevelPlayers(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_GetLevelPlayers);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_GetLevelPlayers, true); // send error
  // process
  lyra_id_t playerid = msg.PlayerID();
  // TLOG_Debug(_T("%s: player %u getting level players"), method, playerid);
  send_GMsg_LevelPlayers(conn, playerid);
}

////
// handle_SMsg_LocateAvatar
////

void LsRoomThread::handle_SMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LocateAvatar);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_LocateAvatar, true); // send error
  // process
  lyra_id_t locatorid = msg.LocatorID();
  lyra_id_t playerid = msg.PlayerID();
  TLOG_Debug("%s: player %u locating player %u", method, locatorid, playerid);
  // find target player
  lyra_id_t roomid = Lyra::ID_UNKNOWN; // default: not found here
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (player && (player->AccountType() != LmPlayerDB::ACCT_MONSTER)) { // monsters are not locatable
    roomid = player->RoomID();
    // hidden?
    if(player->Avatar().Hidden()) {
	TLOG_Debug("%s: player %u is hidden - returning Unknown!", method, playerid);
	roomid += Lyra::HIDDEN_DELTA * 2;
    }
    else if (player->IsHidden()) {
	TLOG_Debug("%s: player %u is MINDBLANKED - returning HIDDEN!", method, playerid);
      roomid += Lyra::HIDDEN_DELTA;
    }
  }
  msg.SetRoomID(roomid);
  // send message back
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// handle_SMsg_GiveItem
////

void LsRoomThread::handle_SMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_GiveItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_GiveItem, true); // send error
  // process
  lyra_id_t sourceid = msg.SourceID();
  lyra_id_t targetid = msg.TargetID();
  // check that source (giver) is in level
  LsPlayer* source = main_->PlayerSet()->GetPlayer(sourceid);
  if (!source) {
    TLOG_Warning(_T("%s: source player %u not in level"), method, sourceid);
    // nothing to send back
    return;
  }
  // check that target (taker) is in level
  LsPlayer* target = main_->PlayerSet()->GetPlayer(targetid);
  if (!target) {
    TLOG_Warning(_T("%s: target player %u not in level"), method, target);
    // send nack back to source
    send_SMsg_GiveItemAck(conn, targetid, sourceid, msg.Item().Header(), GMsg_GiveItemAck::GIVE_NO);
    return;
  }
  // TLOG_Debug(_T("%s: player %u giving item %d to player %u"), method, sourceid, msg.Item().Serial(), targetid);
  // add item/source to target's "taking item" list
  target->TakingItem(sourceid, msg.Item());
  // add item/target to source's "giving item" list
  source->GivingItem(targetid, msg.Item());
  // send SMsg_GiveItem message along to target (taker)
  main_->OutputDispatch()->SendMessage(&msg, target->Connection());
}

////
// handle_SMsg_TakeItemAck
////

void LsRoomThread::handle_SMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_TakeItemAck);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_TakeItemAck, true); // send error
  // process
  lyra_id_t sourceid = msg.SourceID(); // taker
  lyra_id_t targetid = msg.TargetID(); // giver
  LmItemHdr hdr = msg.ItemHeader();
  // get source, target players
  LsPlayer* source = main_->PlayerSet()->GetPlayer(sourceid);
  LsPlayer* target = main_->PlayerSet()->GetPlayer(targetid);
  // check that target (giver) is in level
  if (!target) {
    TLOG_Warning(_T("%s: target player %u not in level"), method, targetid);
    // send nack back to source (taker); giver left before ack could be sent, so abort transaction
    send_SMsg_TakeItemAck(conn, targetid, sourceid, hdr, GMsg_TakeItemAck::TAKE_ERROR);
    // if source player is in level, then end take
    if (source) {
      source->TookItem(hdr);
    }
    return;
  }
  // check that source is in level
  if (!source) {
    TLOG_Warning(_T("%s: source player %u not in level"), method, sourceid);
    // send nack back to target (giver)
    send_SMsg_GiveItemAck(target->Connection(), sourceid, targetid, hdr, GMsg_GiveItemAck::GIVE_NO);
    target->GaveItem(hdr);
    return;
  }
  // TLOG_Debug(_T("%s: player %u taking item %d from player %u: %c"), method, sourceid, hdr.Serial(), targetid, msg.Status());
  // all parties present and accounted for
  bool ok = true;
  // check that target was giving item to source
  if (!target->IsGivingItem(sourceid, hdr)) {
    TLOG_Warning(_T("%s: player %u not giving item to player %u"), method, targetid, sourceid);
    send_SMsg_TakeItemAck(conn, targetid, sourceid, hdr, GMsg_TakeItemAck::TAKE_ERROR);
    ok = false;
  }
  // denote that target (giver) either gave or kept item
  target->GaveItem(hdr);
  // check that source was taking item from target
  if (!source->IsTakingItem(targetid, hdr)) {
    TLOG_Warning(_T("%s: player %u not taking item from player %u"), method, sourceid, targetid);
    send_SMsg_GiveItemAck(target->Connection(), sourceid, targetid, hdr, GMsg_GiveItemAck::GIVE_NO);
    ok = false;
  }
  // denote that source (taker) either rejected or took item
  source->TookItem(hdr);
  // if all of that was ok, then continue, otherwise we're outta here
  if (!ok) {
    return;
  }
  // determine what to send in ack to target (giver)
  int source_ack, target_ack;
  switch (msg.Status()) {
  case GMsg_TakeItemAck::TAKE_YES:
    source_ack = GMsg_TakeItemAck::TAKE_OK;
    target_ack = GMsg_GiveItemAck::GIVE_YES;
    break;
  case GMsg_TakeItemAck::TAKE_NO:
    source_ack = 0; // don't send
    target_ack = GMsg_GiveItemAck::GIVE_NO;
    break;
  case GMsg_TakeItemAck::TAKE_REJECT_ALL:
    source_ack = 0; // don't send
    target_ack = GMsg_GiveItemAck::GIVE_REJECT_ALL;
    break;
  default:
    TLOG_Warning(_T("%s: unknown take status '%c'"), method, msg.Status());
    source_ack = GMsg_TakeItemAck::TAKE_ERROR;
    target_ack = GMsg_GiveItemAck::GIVE_NO;
    break;
  }
  // if taken update ownership, transfer to source (taker)
  if (msg.Status() == GMsg_TakeItemAck::TAKE_YES) {
    int serial = hdr.Serial();
    int rc = main_->ItemDBC()->UpdateItemOwnership(serial, LmItemDBC::OWNER_PLAYER, sourceid, 0);
    int sc = main_->ItemDBC()->LastSQLCode();
    // int lt = main_->ItemDBC()->LastCallTime();
    // main_->Log()->Debug(_T("%s: LmItemDBC::UpdateItemOwnership took %d ms"), method, lt);
    if (rc < 0) {
      TLOG_Warning(_T("%s: player %u could not take item %d; rc=%d, sqlcode=%d"), method, sourceid, serial, rc, sc);
      source_ack = GMsg_TakeItemAck::TAKE_ERROR;
      target_ack = GMsg_GiveItemAck::GIVE_NO;
      LsUtil::HandleItemError(main_, method, rc, sc);
    }
  }
  // send ack to target (giver)
  send_SMsg_GiveItemAck(target->Connection(), sourceid, targetid, hdr, target_ack);
  // send ack to source (taker) if non-zero
  if (source_ack != 0) {
    send_SMsg_TakeItemAck(conn, targetid, sourceid, hdr, source_ack);
  }
}

////
// handle_SMsg_ShowItem
////

void LsRoomThread::handle_SMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_ShowItem);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NONNULL();
  CHECK_CONN_ISGAME();
  // accept message
  ACCEPT_MSG(SMsg_ShowItem, true); // send error
  // process
  lyra_id_t targetid = msg.TargetID(); // giver
  // get target player
  LsPlayer* target = main_->PlayerSet()->GetPlayer(targetid);
  // check
  if (!target) {
    TLOG_Warning(_T("%s: target player %u not in level"), method, targetid);
    return;
  }
  // just pass along to target
  main_->OutputDispatch()->SendMessage(&msg, target->Connection());
}

////
// handle_SMsg_LS_Login
////

void LsRoomThread::handle_SMsg_LS_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LS_Login);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_LS_Login, false); // don't send error
  // get player
  lyra_id_t playerid = msg.PlayerID();
  LsPlayer* player = main_->PlayerSet()->GetPlayer(playerid);
  if (!player) {
    TLOG_Error(_T("%s: player %u not in player set"), method, playerid);
    return;
  }
  // get room
  LsRoomState* room = main_->LevelState()->RoomState(player->RoomID());
  if (!room) {
    TLOG_Error(_T("%s: player %u in room %u, not in level"), method, playerid, player->RoomID());
    return;
  }
  // TLOG_Debug(_T("%s: player %u logging into room %u"), method, playerid, player->RoomID());
  // enter target room
  perform_EnterRoom(player, room, 0);
}

////
// handle_SMsg_LS_Action
////

void LsRoomThread::handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LS_Action);
  HANDLER_ENTRY(false);
  // pre-conditions
  CHECK_CONN_NULL();
  // accept message
  ACCEPT_MSG(SMsg_LS_Action, false); // don't send error
  // switch on action to perform
  // TLOG_Debug(_T("%s: action=%c"), method, msg.Action());
  switch (msg.Action()) {
  case SMsg_LS_Action::ACTION_EXIT:
    TLOG_Log(_T("%s: shutting down"), method);
    // signal end of thread's main loop
    SetDone();
    break;
  case SMsg_LS_Action::ACTION_COMPUTE_GROUPS:
    handle_SMsg_LS_Action_ComputeGroups(msg.Parameter());
    break;
  case SMsg_LS_Action::ACTION_GENERATE_ITEMS:
    handle_SMsg_LS_Action_GenerateItems();
    break;
  case SMsg_LS_Action::ACTION_REAP_ITEMS:
    handle_SMsg_LS_Action_ReapItems();
    break;
  case SMsg_LS_Action::ACTION_HEARTBEAT:
//    TLOG_Log(_T("%s: room thread running"), method);
    break;
  default:
    TLOG_Error(_T("%s: illegal action code %c (%d)"), method, msg.Action(), msg.Action());
    break;
  }
}

////
// handle_SMsg_LS_Action_ComputeGroups
////

void LsRoomThread::handle_SMsg_LS_Action_ComputeGroups(short room_id)
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LS_Action_ComputeGroups);
  LmTimer timer;
  timer.Start();
  //TLOG_Debug(_T("%s: computing groups"), method);
  // compute local groups for each room
  bool single_room = false;

  // if called with room_id 0, calculate for all rooms
  // if called with room_id > 0, calculate for just that room

  if (room_id > 0)
	  single_room = true;

  for (int i = 0; i < main_->LevelDBC()->NumRooms(); ++i) {
	  if (single_room && (i != room_id))
		  continue;
    LsRoomState* room = main_->LevelState()->Room(i);
    if (!room) {
      TLOG_Error(_T("%s: could not get room state for room number %d"), method, i);
      continue;
    }
    // only if room has somebody in it
    if (!room->IsEmpty()) {
      //TLOG_Debug(_T("%s: room %u is not empty; has %u occupants - computing groups"), method, i, room->PlayerList().size());
      ComputeGroups(main_, room);
    }
  }
  timer.Stop();
  // if there are any players in the level
  int num_players = main_->PlayerSet()->NumPlayers();
  if (num_players > 0) {
    //TLOG_Debug(_T("%s: computing groups (%d players) took %lu usec"), method, num_players, timer.MicroSeconds());
  }
}

////
// handle_SMsg_LS_Action_GenerateItems
////

void LsRoomThread::handle_SMsg_LS_Action_GenerateItems()
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LS_Action_GenerateItems);
  // TLOG_Debug(_T("%s: generating items"), method);
  // for each room in level
  int numrooms = main_->LevelDBC()->NumRooms();
  for (int i = 0; i < numrooms; ++i) {
    // generate items, send out messages
    LsRoomState* room = main_->LevelState()->Room(i);
    if (room) {
      if (room->CanAddItem())
	perform_ItemGenerate(room);
     // else TLOG_Warning("%s: could not generate item - room %d full!", method, i);
    }
    else {
      TLOG_Error(_T("%s: null room for room number %d"), method, i);
    }
  }
}

////
// handle_SMsg_LS_Action_ReapItems
////

void LsRoomThread::handle_SMsg_LS_Action_ReapItems()
{
  DEFMETHOD(LsRoomThread, handle_SMsg_LS_Action_ReapItems);
  // TLOG_Debug(_T("%s: reaping items"), method);
  // for each room in level
  int numrooms = main_->LevelDBC()->NumRooms();
  for (int i = 0; i < numrooms; ++i) {
    // reap items if there's no player in there, send out messages
    LsRoomState* room = main_->LevelState()->Room(i);
    if (room /* && (room->NumPlayers(LmPlayerDB::ACCT_PLAYER) == 0) */ ) {
      perform_ItemReap(room);
    }
    else {
      TLOG_Error(_T("%s: null room for room number %d"), method, i);
    }
  }
}
