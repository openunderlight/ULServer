// LsRoomThread.h  -*- C++ -*-
// $Id: LsRoomThread.h,v 1.45 1998-05-01 17:09:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// room thread class

#ifndef INCLUDED_LsRoomThread
#define INCLUDED_LsRoomThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmLogFile.h"
#include "LmThread.h"
#include "RMsg_All.h"
#include "SMsg_All.h"

// forward declarations

class LmLevelDBC;
class LmParty;
class LsMain;
class LsLevelState;
class LsRoomState;
class LsPlayer;
class LmRoomItem;
class SMsg_Proxy;
class LmRoomItemList;
class LsPlayerList;

// the class

class LsRoomThread : public LmThread {

public:

  LsRoomThread(LsMain* lsmain);
  ~LsRoomThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();

private:

  // not implemented
  LsRoomThread(const LsRoomThread&);
  //operator=(const LsRoomThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // RMsg_* message handlers (proxied)
  void handle_RMsg_GotoRoom(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_Logout(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_Party(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_Speech(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_Ping(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_ChangeAvatar(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_GetAvatarDescription(LmSrvMesgBuf* msgbuf, LsPlayer* source);
  void handle_RMsg_GetRoomDescription(LmSrvMesgBuf* msgbuf, LsPlayer* source);

  // SMsg_* message handlers
  void handle_SMsg_GetItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_PutItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Proxy(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_DestroyRoomItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GetLevelPlayers(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_SetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_LS_* message handlers
  void handle_SMsg_LS_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // message sending methods (sent to player via proxy)
  void send_GMsg_LevelPlayers(LmConnection* conn, lyra_id_t playerid);
  void send_RMsg_Party_Reject(LsPlayer* target, int responsecode, lyra_id_t playerid = 0);
  void send_RMsg_Party_Join(LsPlayer* target, lyra_id_t new_playerid, int responsecode);
  void send_RMsg_Party_JoinedParty(LsPlayer* newplayer);
  void send_RMsg_Party_Leave(LsPlayer* leavingplayer, int how);
  void send_RMsg_PartyInfo(LsPlayer* target);
  void send_RMsg_LeaveRoom(LsPlayer* player, LsRoomState* room, int how, int lastx, int lasty);
  void send_RMsg_EnterRoom(LsPlayer* player, LsRoomState* room);
  void send_RMsg_Speech(LsPlayer* source, LsPlayerList& target_list, RMsg_Speech& msg);
  void send_RMsg_ItemPickup(LsPlayer* player, LsRoomState* room, const LmItem& item);
  void send_RMsg_ItemPickup(LsPlayer* player, LsRoomState* room, const LmRoomItemList& items);
  void send_RMsg_ItemDrop(LsPlayer* player, LsRoomState* room, const LmRoomItem& roomitem);
  void send_RMsg_ItemDrop(LsPlayer* player, LsRoomState* room, const LmRoomItemList& items, bool broadcast);
  void send_RMsg_ChangeAvatar(LsPlayer* source, LsPlayerList& target_list);
  void send_RMsg_PlayerMsg_DreamStrikeAck(LsPlayer* player, lyra_id_t target, bool success);
  void send_RMsg_RcvAvatarDescription(LsPlayer* player, lyra_id_t targetid, const TCHAR* targetdesc);
  void send_RMsg_RoomDescription(LsPlayer* player, short levelid, short roomid, const TCHAR* roomdesc);

  void send_SMsg_ItemPickup(LsPlayer* player, const LmItem& item, int status);
  void send_SMsg_ItemDrop(LsPlayer* player, const LmItem& item, int status);
  void send_SMsg_Proxy(LmConnection* conn, LmMesg& msg);
  void send_SMsg_PartyLeader(LmConnection* gsconn, lyra_id_t playerid, int leader_time);
  void send_SMsg_GiveItemAck(LmConnection* conn, lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& hdr, int stat);
  void send_SMsg_TakeItemAck(LmConnection* conn, lyra_id_t sourceid, lyra_id_t targetid, const LmItemHdr& mdr, int stat);

  void broadcast_room(LsRoomState* room, LsPlayer* source, LmMesg& msg, bool sendself = false);
  void broadcast_party(const LmParty& party, LsPlayer* source, LmMesg& msg, bool sendself = false);

  // message handler auxiliary methods
  void perform_Logout(LsPlayer* player, int how);
  void perform_LeaveRoom(LsPlayer* player, LsRoomState* room, int how, int lastx, int lasty);
  void perform_EnterRoom(LsPlayer* player, LsRoomState* room, int last_room);
  void perform_Party_Leave(LsPlayer* player, int how, bool grant_xp = true);
  void perform_ItemPickup(LsPlayer* player, LsRoomState* room, const LmItem& item);
  void perform_ItemDestroy(LsRoomState* room, const LmItem& item);
  void perform_ItemDrop(LsPlayer* player, LsRoomState* room, const LmRoomItem& roomitem);
  void perform_ItemDrop_List(LsRoomState* room, const LmRoomItemList& items);

  void perform_ItemReap(LsRoomState* room);
  void perform_ItemGenerate(LsRoomState* room);

  void perform_GiveItem(LsPlayer* giver, LsPlayer* taker, const LmItem& item);
  void perform_TakeItem(LsPlayer* taker, LsPlayer* giver, const LmItemHdr& itemhdr, bool taken);

  void perform_create_essence(LsPlayer* player, LsRoomState* room, lyra_id_t slaverid, int s1, int s2);
  void perform_create_soulessence(LsPlayer* player, LsRoomState* room, lyra_id_t killer);
  void perform_spawn_mare_item(LsPlayer* player, LsRoomState* room, int item_type, const LmPosition& spawnPoint);
	const LmPosition& get_random_neighbor_position(LsRoomState* room, LsPlayer* player);  

  void handle_RMsg_Party_Join(LsPlayer* player, RMsg_Party& msg);
  void handle_RMsg_Party_Accept(LsPlayer* player, RMsg_Party& msg);
  void handle_RMsg_Party_Reject(LsPlayer* player, RMsg_Party& msg);
  void handle_RMsg_Party_Leave(LsPlayer* player, RMsg_Party& msg);

  void handle_SMsg_LS_Action_ComputeGroups(short room_id);
  void handle_SMsg_LS_Action_GenerateItems();
  void handle_SMsg_LS_Action_ReapItems();

  void compute_RMsg_Speech_Speech(LsPlayer* source, LsPlayerList& t_list, RMsg_Speech& m);
  void compute_RMsg_Speech_Shout(LsPlayer* source, LsPlayerList& t_list, RMsg_Speech& m);
  void compute_RMsg_Speech_Whisper(LsPlayer* source, LsPlayerList& t_list, RMsg_Speech& m);
  void compute_RMsg_Speech_WhisperEmote(LsPlayer* source, LsPlayerList& t_list, RMsg_Speech& m);
  void compute_RMsg_Speech_GlobalShout(LsPlayer* source, LsPlayerList& t_list, RMsg_Speech& m);

  void compute_PlayerList(LsRoomState* room, LsPlayerList& player_list, lyra_id_t playerid);
  void compute_PartyList(const LmParty& party, LsPlayerList& player_list, lyra_id_t playerid);

  void open_log();
  void close_log();
  void register_handlers();

  LsMain* main_;    // pointer to level server main object

  // LmLogFile logf_;  // room thread log

};

#ifdef USE_INLINE
#include "LsRoomThread.i"
#endif

#endif /* INCLUDED_LsRoomThread */
