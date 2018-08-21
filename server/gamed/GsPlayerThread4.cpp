// GsPlayerThread4.cpp  -*- C++ -*-
// $Id: GsPlayerThread4.cpp,v 1.15 1998/05/02 00:09:10 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// send_* methods

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

////
// send_GMsg_RcvPlayerName
////

void GsPlayerThread::send_GMsg_RcvPlayerName(LmConnection* conn, lyra_id_t playerid, const TCHAR* name, int requestid)
{
  GMsg_RcvPlayerName msg;
  msg.Init(playerid, requestid, name);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_PPointAck
////

void GsPlayerThread::send_GMsg_PPointAck(LmConnection* conn, int type, int result) //, int var1, int var2)
{
  GMsg_PPointAck msg;
  msg.Init(type, result); //, var1, var2);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}


////
// send_GMsg_LocateAvatarAck - send locateavatar ack message to player
////

void GsPlayerThread::send_GMsg_LocateAvatarAck(LmConnection* conn, const TCHAR* name, int status,
					       lyra_id_t levelid, lyra_id_t roomid)
{
  GMsg_LocateAvatarAck msg;
  msg.Init(1); // single player
  msg.SetPlayerName(0, name);
  msg.SetStatus(0, status);
  msg.SetLocation(0, levelid, roomid);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_ChangeStat - send changestat message to player
////

void GsPlayerThread::send_GMsg_ChangeStat(LmConnection* conn, int req, int stat, int val)
{
  GMsg_ChangeStat msg;
  msg.Init(1);
  msg.InitChange(0, req, stat, val);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_ItemDrop - send itemdrop message to player
////

void GsPlayerThread::send_GMsg_ItemDrop(LmConnection* conn, const LmItemHdr& itemhdr, int status)
{
  GMsg_ItemDrop msg;
  msg.Init(status, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_ItemPickup - send itempickup message to player
////

void GsPlayerThread::send_GMsg_ItemPickup(LmConnection* conn, const LmItem& item, int status)
{
  GMsg_ItemPickup msg;
  msg.Init(status, item);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_Goal - send goal ack/error message to player
////

void GsPlayerThread::send_GMsg_Goal(LmConnection* conn, int req, lyra_id_t id)
{
  GMsg_Goal msg;
  msg.Init(id, req);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_RcvGoalHdr - send goal header to player
////

void GsPlayerThread::send_GMsg_RcvGoalHdr(LmConnection* conn, int sessionid, lyra_id_t goalid, const TCHAR* summary,
					  int status, int playeroption)
{
  GMsg_RcvGoalHdr msg;
  msg.Init(goalid, sessionid, status, playeroption, summary);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_RcvReportHdr - send report header to player
////

void GsPlayerThread::send_GMsg_RcvReportHdr(LmConnection* conn, int sessionid, lyra_id_t reportid, lyra_id_t goalid,  const TCHAR* summary, unsigned short flags)
{
  GMsg_RcvReportHdr msg;
  msg.Init(reportid, goalid, sessionid, summary, flags);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_RcvReportGoals - send report goals to player
////
void GsPlayerThread::send_GMsg_RcvReportGoals(LmConnection* conn, lyra_id_t goalid)
{
  GMsg_RcvReportGoals msg;
  msg.Init(goalid);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}


////
// send_GMsg_TakeItem - send take item request to player
////

void GsPlayerThread::send_GMsg_TakeItem(LmConnection* conn, lyra_id_t source_id, const LmItem& item)
{
  GMsg_TakeItem msg;
  msg.Init(source_id, item);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_TakeItemAck - send take item ack to player
////

void GsPlayerThread::send_GMsg_TakeItemAck(LmConnection* conn, const LmItemHdr& itemhdr, int status)
{
  GMsg_TakeItemAck msg;
  msg.Init(status, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_GiveItemAck - send give item ack to player
////

void GsPlayerThread::send_GMsg_GiveItemAck(LmConnection* conn, const LmItemHdr& itemhdr, int status)
{
  GMsg_GiveItemAck msg;
  msg.Init(status, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_ViewItem
////

void GsPlayerThread::send_GMsg_ViewItem(LmConnection* conn, lyra_id_t sourceid, const LmItemHdr& itemhdr,
					const TCHAR* itemname)
{
  GMsg_ViewItem msg;
  msg.Init(sourceid, itemhdr, itemname);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_GMsg_ItemDescription
////

void GsPlayerThread::send_GMsg_ItemDescription(LmConnection* conn, int serial, const TCHAR* creator, const TCHAR* desc, const TCHAR* target)
{
  GMsg_ItemDescription msg;
  msg.Init(serial, creator, desc, target);
  main_->OutputDispatch()->SendMessage(&msg, conn);
}

////
// send_RMsg_Logout - send logout message to level server (proxied)
////

void GsPlayerThread::send_RMsg_Logout(LmConnection* lsconn, int status)
{
  RMsg_Logout msg;
  msg.Init(status);
  send_SMsg_Proxy(lsconn, msg);
}

////
// send_RMsg_ChangeAvatar
////

void GsPlayerThread::send_RMsg_ChangeAvatar(LmConnection* lsconn, const LmAvatar& avatar)
{
  RMsg_ChangeAvatar msg;
  msg.Init(player_->PlayerID(), avatar);
  send_SMsg_Proxy(lsconn, msg);
}

////
// send_RMsg_PlayerMsg_DreamStrikeAck - send to player
////

void GsPlayerThread::send_RMsg_PlayerMsg_DreamStrikeAck(bool strike_success, lyra_id_t target)
{
  RMsg_PlayerMsg msg;
  // init and send ack
  msg.Init(target, player_->PlayerID(), RMsg_PlayerMsg::DREAMSTRIKE_ACK);
  msg.SetState1(strike_success);
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
  // send changestat for max ds
  send_GMsg_ChangeStat(player_->Connection(), GMsg_ChangeStat::SET_STAT_MAX, Stats::DREAMSOUL,
		       player_->DB().Stats().MaxStat(Stats::DREAMSOUL));
}

////
// send_RMsg_PlayerMsg_Ascend - send to player
////

void GsPlayerThread::send_RMsg_PlayerMsg_Ascend(int guild, bool success)
{
  RMsg_PlayerMsg msg;
  // init and send ack
  msg.Init(player_->PlayerID(), player_->PlayerID(), RMsg_PlayerMsg::ASCEND);
  msg.SetState1(guild);
  msg.SetState2(success);
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// send_RMsg_PlayerMsg_SelfTrain - send to player
////

void GsPlayerThread::send_RMsg_PlayerMsg_SelfTrain(int art, bool success)
{
  RMsg_PlayerMsg msg;
  // init and send ack
  msg.Init(player_->PlayerID(), player_->PlayerID(), RMsg_PlayerMsg::TRAIN_SELF);
  msg.SetState1(art);
  msg.SetState2(success);
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// send_RMsg_PlayerMsg_GratitudeAck - send to player
////

void GsPlayerThread::send_RMsg_PlayerMsg_GratitudeAck(bool success)
{
  RMsg_PlayerMsg msg;
  // init and send ack
  msg.Init(player_->PlayerID(), player_->PlayerID(), RMsg_PlayerMsg::REDEEM_GRATITUDE);
  msg.SetState1(success);
  main_->OutputDispatch()->SendMessage(&msg, player_->Connection());
}

////
// send_RMsg_PlayerMsg_GrantPPoint - send to player
////

void GsPlayerThread::send_RMsg_PlayerMsg_GrantPPoint(lyra_id_t target)
{
  RMsg_PlayerMsg msg;
  // init and send ack
  msg.Init(player_->PlayerID(), target, RMsg_PlayerMsg::GRANT_PPOINT);
  send_SMsg_Proxy(player_->LevelConnection(), msg);

  //main_->OutputDispatch()->SendMessage(&msg, player_->Connection());

}


////
// send_SMsg_LocateAvatar - send locateavatar message to level server
////

void GsPlayerThread::send_SMsg_LocateAvatar(LmConnection* lsconn, lyra_id_t playerid)
{
  SMsg_LocateAvatar msg;
  msg.Init(player_->PlayerID(), playerid, 0);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_LevelLogin - send login message to level server from player
////

void GsPlayerThread::send_SMsg_LevelLogin(LmConnection* lsconn, lyra_id_t roomid, const LmPeerUpdate& update)
{
  SMsg_LevelLogin msg;
  // player's server port is actually the game server's port (for redirection)
  // msg.Init(player_->PlayerID(), player_->DB().PlayerName(), player_->DB().AccountType(),
  // player_->ClientAddress().IPAddress(), main_->ServerPort(), player_->Avatar(), roomid, update);
  // player's server port is as given by the client, for direct ls->client updates
  
  // if we are a teacher or a guardian or above, we get the alerts
  // whenever a newly awakened enters our level

  if (player_->Firewall()) // update address is us (the gamed) so we can forward it
  {
	  TLOG_Debug( "Player has firewall set - tunnel thru gamed!" );
	  msg.Init(player_->PlayerID(), player_->DB().PlayerName(), player_->DB().AccountType(),
		main_->HostIPAddress(), main_->ServerPort(), player_->Avatar(), roomid, update,
		player_->IsHidden(), player_->DB().AvatarDescrip(), player_->DB().NewlyAlert(),
		player_->DB().NewlyAwakened(), 0); 
  }
  else
	  msg.Init(player_->PlayerID(), player_->DB().PlayerName(), player_->DB().AccountType(),
		player_->UpdateAddress().IPAddress(), player_->UpdateAddress().Port(), player_->Avatar(), roomid, update,
		player_->IsHidden(), player_->DB().AvatarDescrip(), player_->DB().NewlyAlert(),
		player_->DB().NewlyAwakened(), 0);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_PutItem - send putitem message to level server
////

void GsPlayerThread::send_SMsg_PutItem(LmConnection* lsconn, lyra_id_t playerid, lyra_id_t roomid,
				       const LmItem& item, const LmPosition& pos, int ttl)
{
  SMsg_PutItem msg;
  msg.Init(playerid, roomid, item, pos, ttl);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_GetItem - send getitem message to level server
////

void GsPlayerThread::send_SMsg_GetItem(LmConnection* lsconn, lyra_id_t roomid, const LmItemHdr& itemhdr)
{
  SMsg_GetItem msg;
  msg.Init(player_->PlayerID(), roomid, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_Proxy - send proxied message to level server from player
////

void GsPlayerThread::send_SMsg_Proxy(LmConnection* lsconn, LmSrvMesgBuf* msgbuf)
{
  SMsg_Proxy msg;
  msg.Init(player_->PlayerID(), SMsg_Proxy::PROXY_PROCESS, msgbuf);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

void GsPlayerThread::send_SMsg_Proxy(LmConnection* lsconn, LmMesg& pmsg)
{
  SMsg_Proxy msg;
  msg.Init(player_->PlayerID(), SMsg_Proxy::PROXY_PROCESS, pmsg);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}


void GsPlayerThread::send_SMsg_UniverseBroadcast(LmSrvMesgBuf* msgbuf)
{
  SMsg_UniverseBroadcast msg;
  msg.Init(msgbuf);
  main_->OutputDispatch()->SendMessage(&msg, connectToBcastLevelD());
}

void GsPlayerThread::send_SMsg_UniverseBroadcast(LmMesg& pmsg)
{
  SMsg_UniverseBroadcast msg;
  msg.Init(pmsg);
  main_->OutputDispatch()->SendMessage(&msg, connectToBcastLevelD());
}



////
// send_SMsg_DestroyRoomItem - send destroyroomitem message to level server
////

void GsPlayerThread::send_SMsg_DestroyRoomItem(LmConnection* lsconn, lyra_id_t roomid, const LmItemHdr& itemhdr)
{
  SMsg_DestroyRoomItem msg;
  msg.Init(player_->PlayerID(), roomid, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_GetLevelPlayers
////

void GsPlayerThread::send_SMsg_GetLevelPlayers(LmConnection* lsconn)
{
  SMsg_GetLevelPlayers msg;
  msg.Init(player_->PlayerID());
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_GiveItem
////

void GsPlayerThread::send_SMsg_GiveItem(LmConnection* lsconn, lyra_id_t targetid, const LmItem& item)
{
  SMsg_GiveItem msg;
  msg.Init(player_->PlayerID(), targetid, item);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_TakeItemAck
////

void GsPlayerThread::send_SMsg_TakeItemAck(LmConnection* lsconn, lyra_id_t targetid, const LmItemHdr& itemhdr, int status)
{
  SMsg_TakeItemAck msg;
  msg.Init(status, player_->PlayerID(), targetid, itemhdr);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_ShowItem
////

void GsPlayerThread::send_SMsg_ShowItem(LmConnection* lsconn, lyra_id_t sourceid, lyra_id_t targetid, const LmItem& item)
{
  SMsg_ShowItem msg;
  msg.Init(sourceid, targetid, item.Header(), item.Name());
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}

////
// send_SMsg_SetAvatarDescription
////

void GsPlayerThread::send_SMsg_SetAvatarDescription(LmConnection* lsconn, lyra_id_t playerid, const TCHAR* description)
{
  SMsg_SetAvatarDescription msg;
  msg.Init(playerid, description);
  main_->OutputDispatch()->SendMessage(&msg, lsconn);
}
