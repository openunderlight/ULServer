// GsPlayerThread.h  -*- C++ -*-
// $Id: GsPlayerThread.h,v 1.46 1998-05-01 17:09:10-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player thread class

#ifndef INCLUDED_GsPlayerThread
#define INCLUDED_GsPlayerThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmLogFile.h"
#include "LmThread.h"

// forward declarations

class GsMain;
class GsPlayer;
class LmItem;
class LmItemHdr;
class LmPosition;
class LmMesg;
class LmPeerUpdate;
class LmLevelDBC;
class LmAvatar;
class GMsg_Goal;
class LmGoalInfo;
class GMsg_Logout;
class GMsg_LocateAvatar;

// the class

class GsPlayerThread : public LmThread {

public:

  GsPlayerThread(GsMain* gsmain);
  ~GsPlayerThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Login(GsPlayer* player);
  void Logout();

  void Run();

  enum {
    SAVE_DISK = 5,  // save to disk interval, in minutes
    SAVE_DB = 15    // save to database interval, in minutes
  };

private:

  // not implemented
  GsPlayerThread(const GsPlayerThread&);
  //operator=(const GsPlayerThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // GMsg_* message handlers
  void handle_GMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_PutItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_CreateItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_DestroyItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GotoLevel(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_UpdateItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_UpdateStats(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_ChangeStat(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_DestroyRoomItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetLevelPlayers(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_ChangeAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetGoalHdrs(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetReportHdrs(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_PostGoal(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_PostReport(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetItemDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_SetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_GetPlayerName(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void handle_GMsg_Goal(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_Goal_DeleteReport(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_AcceptGoal(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_RemoveGoal(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_Vote(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_ExpireGoal(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_CompleteGoal(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_CompleteQuest(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_DoesHaveCodex(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_GetGoalText(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_GetGoalDetails(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_GetReportText(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_GetGoalbookHeaders(const GMsg_Goal& gmsg);
  void handle_GMsg_Goal_GetGuardianFlag(const GMsg_Goal& gmsg);

  void handle_GMsg_GrantPPoint(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_GMsg_UsePPoint(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // RMsg_* message handlers (C->LS)
  void handle_RMsg_Party(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_Logout(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_Speech(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_GotoRoom(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_GetAvatarDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_GetRoomDescription(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_* message handlers
  void handle_SMsg_ItemDrop(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_ItemPickup(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Proxy(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GiveItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GiveItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_TakeItemAck(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LocateAvatar(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_PartyLeader(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_ShowItem(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_DBQueryAckPT(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_SMsg_Proxy_* message handlers
  void handle_SMsg_Proxy_RMsg_PlayerMsg(LmSrvMesgBuf* msgbuf);

  // SMsg_GS_* message handlers
  void handle_SMsg_GS_Login(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // message sending methods
  void send_GMsg_ItemDrop(LmConnection* conn, const LmItemHdr& itemhdr, int status);
  void send_GMsg_ItemPickup(LmConnection* conn, const LmItem& item, int status);
  void send_GMsg_TakeItem(LmConnection* conn, lyra_id_t source_id, const LmItem& item);
  void send_GMsg_TakeItemAck(LmConnection* conn, const LmItemHdr& itemhdr, int status);
  void send_GMsg_GiveItemAck(LmConnection* conn, const LmItemHdr& itemhdr, int status);
  void send_GMsg_ChangeStat(LmConnection* conn, int req, int stat, int val);
  //void send_GMsg_ChangeAvatar(LmConnection* conn, LmAvatar avtr, int type);
  void send_GMsg_Goal(LmConnection* conn, int req, lyra_id_t id);
  void send_GMsg_RcvGoalHdr(LmConnection* conn, int sessionid,
			    lyra_id_t goalid, const TCHAR* summary, int status, int playeroption);
  void send_GMsg_RcvReportHdr(LmConnection* conn, int sessionid, lyra_id_t repid, lyra_id_t goalid, const TCHAR* summary, unsigned short flags);
  void send_GMsg_RcvReportGoals(LmConnection* conn, lyra_id_t goalid);
  void send_GMsg_LocateAvatarAck(LmConnection* conn, const TCHAR* name, int status, lyra_id_t levelid, lyra_id_t roomid);
  void send_GMsg_ViewItem(LmConnection* conn, lyra_id_t sourceid, const LmItemHdr& itemhdr, const TCHAR* itemname);
  void send_GMsg_ItemDescription(LmConnection* conn, int serial, const TCHAR* creator, const TCHAR* description, const TCHAR* target);
  void send_GMsg_RcvPlayerName(LmConnection* conn, lyra_id_t playerid, const TCHAR* name, int requestid);
  void send_GMsg_PPointAck(LmConnection* conn, int type, int result);
  void send_SMsg_LevelLogin(LmConnection* lsconn, lyra_id_t roomid, const LmPeerUpdate& update);
  void send_SMsg_PutItem(LmConnection* lsconn, lyra_id_t playerid, lyra_id_t roomid, const LmItem& item, 
			 const LmPosition& pos, int ttl);
  void send_SMsg_GiveItem(LmConnection* lsconn, lyra_id_t targetid, const LmItem& item);
  void send_SMsg_GetItem(LmConnection* lsconn, lyra_id_t roomid, const LmItemHdr& itemhdr);
  void send_SMsg_Proxy(LmConnection* lsconn, LmSrvMesgBuf* msgbuf);
  void send_SMsg_Proxy(LmConnection* lsconn, LmMesg& msg);
  void send_SMsg_UniverseBroadcast(LmSrvMesgBuf* msgbuf);
  void send_SMsg_UniverseBroadcast(LmMesg& msg);

  void send_SMsg_DestroyRoomItem(LmConnection* lsconn, lyra_id_t roomid, const LmItemHdr& itemhdr);
  void send_SMsg_GetLevelPlayers(LmConnection* lsconn);
  void send_SMsg_TakeItemAck(LmConnection* lsconn, lyra_id_t targetid, const LmItemHdr& itemhdr, int status);
  void send_SMsg_LocateAvatar(LmConnection* lsconn, lyra_id_t playerid);
  void send_SMsg_ShowItem(LmConnection* lsconn, lyra_id_t sourceid, lyra_id_t targetid, const LmItem& item);
  void send_SMsg_SetAvatarDescription(LmConnection* lsconn, lyra_id_t playerid, const TCHAR* description);

  void send_RMsg_ChangeAvatar(LmConnection* lsconn, const LmAvatar& avatar);
  void send_RMsg_Logout(LmConnection* lsconn, int status);
  void send_RMsg_PlayerMsg_DreamStrikeAck(bool strike_success, lyra_id_t target);
  void send_RMsg_PlayerMsg_Ascend(int guild, bool success);
  void send_RMsg_PlayerMsg_SelfTrain(int art, bool success);
  void send_RMsg_PlayerMsg_GratitudeAck(bool success);
  void send_RMsg_PlayerMsg_GrantPPoint(lyra_id_t target);

  // message handler auxiliary methods
  void perform_locateavatar(lyra_id_t playerid, const TCHAR* playername);
  void perform_locateavatar_group(const GMsg_LocateAvatar& msg);
  void guild_error(int rc, int sqlcode, int err_status, lyra_id_t id);
  int get_goalinfo(LmGoalInfo& goalinfo, lyra_id_t goalid, int err_status);
  void perform_logout(const GMsg_Logout& msg);
  void adjust_xp(int xp_adj, const TCHAR* why, lyra_id_t why_id, bool send_msg);
  void adjust_offline_xp(int xp_adj, const TCHAR* why, lyra_id_t why_id, bool send_msg);
  void hide_player(int hide);
  void send_motd();
  LmConnection* connectToBcastLevelD();
  void open_log();
  void close_log();
  void register_handlers();
  
  GsMain* main_;      // pointer to game server main object
  GsPlayer* player_;  // pointer to player object for this thread
  int num_uses_;      // number of uses this thread has had
  int num_save_sigs_; // number of times thread has been told to save player state

  LmLogFile logf_;     // player thread log

};

#ifdef USE_INLINE
#include "GsPlayerThread.i"
#endif

#endif /* INCLUDED_GsPlayerThread */
