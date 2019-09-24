// LsPlayer.cpp  -*- C++ -*-
// $Id: LsPlayer.cpp,v 1.41 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsPlayer.h"
#endif

#include <stdio.h>

#include "LsPlayer.h"

#include "LyraDefs.h"

#include "PThMutex.h"
#include "LmDatabase.h"
#include "LsMain.h"
#include "LmPlayerDB.h"
#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSockType.h"
#include "LmSockAddrInet.h"
#include "RmRemotePlayer.h"
#include "LmLogFile.h"
#include "LmLocker.h"
#include "LmUtil.h"

#ifndef USE_INLINE
#include "LsPlayer.i"
#endif

DECLARE_TheFileName;


////
// Constructor
////

LsPlayer::LsPlayer()
  : lgroup_(Lyra::MAX_GROUP),
    lgroup2_(Lyra::MAX_GROUP),
    neighbors_(Lyra::MAX_OTHERPEOPLE),
    neighbors2_(Lyra::MAX_OTHERPEOPLE),
    joiners_(Lyra::MAX_PARTYSIZE),
    neighbor_updates_(LmNEW(LsUpdateSet())),
    channeltarget(0),
    channellevel(0)
{
  DECLARE_TheLineNum;
  lock_.Init();
  u_lock_.Init();
  clear_information();
}

////
// Destructor
////

LsPlayer::~LsPlayer()
{
  DECLARE_TheLineNum;
  for (LsUpdateSet::iterator k = neighbor_updates_->begin(); k != neighbor_updates_->end(); ++k) {
    LsLastUpdate* last_pos = (*k).second;
    LmDELETE(last_pos);
  }
  LmDELETE(neighbor_updates_);
}

////
// Login
////

int LsPlayer::Login(LsMain* /* lsmain */, lyra_id_t playerid)
{
  DEFMETHOD(LsPlayer, Login);
  DECLARE_TheLineNum;
#if 0
  // get database file name
  TCHAR pfile[FILENAME_MAX];
  lsmain->GlobalDB()->GetPlayerFile(pfile, playerid);
  // open database
  LmDatabase db;
  if (db.Open(_T(""), pfile, GDBM_READER) < 0) {
    lsmain->Log()->Error(_T("%s: could not open player database '%s'"), method, pfile);
    return -1;
  }
  // read it in
  if (dbc_.Load(db, playerid) < 0) {
    lsmain->Log()->Error(_T("%s: could not load player database '%s'"), method, pfile);
    return -1;
  }
  db.Close();
#endif
  clear_information();
  playerid_ = playerid;
  login_time_ = time(NULL);
  return 0;
}

////
// Logout
////

void LsPlayer::Logout()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  clear_information();
}

////
// Init
////

void LsPlayer::Init(LmConnection* conn, const TCHAR* playername, int acct_type, lyra_id_t roomid,
		    const LmPeerUpdate& update, unsigned long clientip, int servport, const LmAvatar& avatar,
		    const TCHAR* description, bool newly_alert, bool newly_awakened)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  // if account type is monster, playername should be modified (format: "X_Y" where X is
  // the actual name (ie. "Horron"), and Y is the playerid, which is stripped away)
  if (acct_type == LmPlayerDB::ACCT_MONSTER) {
	  if (_tcscmp(_T("Revenant"), description) == 0) {
		  _tcsncpy(playername_, "Revenant", sizeof(playername_));
	  }
	  else
	  {
		int pi;
		// *** STRING LITERAL ***  
		if (_stscanf(playername, "%[^_]_%d", playername_, &pi) != 2) {
		  // couldn't parse it
		 _tcsnccpy(playername_, playername, sizeof(playername_));
		}
	  }
  }
  else {
   _tcsnccpy(playername_, playername, sizeof(playername_));
  }
 _tcsnccpy(desc_, description, sizeof(desc_));
  acct_type_ = acct_type;
  set_roomid(roomid);
  set_update(update);
  client_ipaddr_ = clientip;
  serv_port_ = servport;
  avatar_ = avatar;
  conn_ = conn;
  ipaddr_ = 0;
  // determine IP address
  if (conn->Socket().SockType() == LmSockType::Inet_Stream()) {
    const LmSockAddrInet& peeraddr = conn->Socket().PeerName();
    ipaddr_ = peeraddr.IPAddress();
    // send updates to game server
    // update_addr_.Init(ipaddr_, serv_port_);
    // send updates to client
    update_addr_.Init(client_ipaddr_, serv_port_);
  }
#if 0
  lgroup_.Empty();
  neighbors_.Empty();
  joiners_.Empty();
  t_items_.erase(t_items_.begin(), t_items_.end());
  g_items_.erase(g_items_.begin(), g_items_.end());
#endif
  in_use_ = true;
  newly_alert_ = newly_alert;
  newly_awakened_ = newly_awakened;
//  tcp_only_ = tcp_only;
}

////
// SetDescription
////

void LsPlayer::SetDescription(const TCHAR* description)
{
  LmLocker mon(lock_); // lock object for method duration
 _tcsnccpy(desc_, description, sizeof(desc_));
}

lyra_id_t LsPlayer::ChannelTarget()
{
    LmLocker mon(lock_);
    return channeltarget;
}

int LsPlayer::ChannelLevel()
{
    LmLocker mon(lock_);
    return channellevel;
}

////
// CopyInfo
////

void LsPlayer::CopyInfo(RmRemotePlayer& rp)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  pupdate_.SetRealtimeID(realtimeid_);
  rp.Init(pupdate_, avatar_, playername_, playerid_, roomid_);
}

////
// GivingItem
////

void LsPlayer::GivingItem(lyra_id_t takerid, const LmItem& item)
{
  LmLocker mon(lock_); // lock object for method duration
  LsGiveTakeItem give;
  give.item = item;
  give.playerid = takerid;
  g_items_.push_back(give);
}

////
// GaveItem
////

void LsPlayer::GaveItem(const LmItemHdr& hdr)
{
  LmLocker mon(lock_); // lock object for method duration
  for (LsGiveTakeItemList::iterator i = g_items_.begin(); !(bool)(i == g_items_.end()); ++i) {
    LmItem item = (*i).item;
    if (item.Header() == hdr) {
      g_items_.erase(i);
      return;
    }
  }
}

////
// IsGivingItem
////

bool LsPlayer::IsGivingItem(lyra_id_t takerid, const LmItemHdr& hdr) const
{
  LmLocker mon(lock_); // lock object for method duration
  for (LsGiveTakeItemList::const_iterator i = g_items_.begin(); !(bool)(i == g_items_.end()); ++i) {
    lyra_id_t playerid = (*i).playerid;
    LmItem item = (*i).item;
    if ((item.Header() == hdr) && (takerid = playerid)) {
      return true;
    }
  }
  return false;
}

////
// TakingItem
////

void LsPlayer::TakingItem(lyra_id_t giverid, const LmItem& item)
{
  LmLocker mon(lock_); // lock object for method duration
  LsGiveTakeItem take;
  take.item = item;
  take.playerid = giverid;
  t_items_.push_back(take);
}

////
// TookItem
////

void LsPlayer::TookItem(const LmItemHdr& hdr)
{
  LmLocker mon(lock_); // lock object for method duration
  for (LsGiveTakeItemList::iterator i = t_items_.begin(); !(bool)(i == t_items_.end()); ++i) {
    LmItem item = (*i).item;
    if (item.Header() == hdr) {
      t_items_.erase(i);
      return;
    }
  }
}

////
// IsTakingItem
////

bool LsPlayer::IsTakingItem(lyra_id_t giverid, const LmItemHdr& hdr) const
{
  LmLocker mon(lock_); // lock object for method duration
  for (LsGiveTakeItemList::const_iterator i = t_items_.begin(); !(bool)(i == t_items_.end()); ++i) {
    lyra_id_t playerid = (*i).playerid;
    LmItem item = (*i).item;
    if ((item.Header() == hdr) && (giverid = playerid)) {
      return true;
    }
  }
  return false;
}

////
// Selectors that lock the object
////

lyra_id_t LsPlayer::RoomID() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return roomid_;
}

int LsPlayer::Online() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return LmUtil::TimeSince(login_time_);
}

int LsPlayer::IdleTime() const
{
  LmLocker mon(u_lock_); // lock update data for method duration
  return LmUtil::TimeSince(last_pos_update_);
}

const LmPosition& LsPlayer::Position() const
{
  DECLARE_TheLineNum;
  LmLocker mon(u_lock_); // lock object for method duration
  return pos_;
}


const LmPeerUpdate& LsPlayer::LastUpdate() const
{
  DECLARE_TheLineNum;
  LmLocker mon(u_lock_); // lock object for method duration
  return last_update_;
}

const LmPeerUpdate& LsPlayer::PlayerUpdate() const
{
  DECLARE_TheLineNum;
  LmLocker mon(u_lock_); // lock object for method duration
  LsPlayer* self = (LsPlayer*) this;
  self->pupdate2_ = pupdate_;
  return pupdate2_;
}

const LmParty& LsPlayer::Party() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  LsPlayer* self = (LsPlayer*) this;
  self->party2_ = party_;
  return party2_;
}

bool LsPlayer::InUse() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  return in_use_;
}

time_t LsPlayer::PartyJoinTime() const
{
  LmLocker mon(lock_); // lock object for method duration
  return party_join_;
}

const LmIdSet& LsPlayer::LocalGroup() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  LsPlayer* self = (LsPlayer*) this;
  self->lgroup2_ = lgroup_;
  return lgroup2_;
}

const LmIdSet& LsPlayer::Neighbors() const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  LsPlayer* self = (LsPlayer*) this;
  self->neighbors2_ = neighbors_;
  return neighbors2_;
}

////
// Mutators that lock the object
////

void LsPlayer::GotoRoom(lyra_id_t roomid, const LmPeerUpdate& update)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  set_roomid(roomid);
  set_update(update);
}

void LsPlayer::SetAvatar(const LmAvatar& avatar)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  avatar_ = avatar;
}

void LsPlayer::SetUpdateAddress(unsigned long clientip, int servport) 
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  update_addr_.Init(clientip, servport);
}

void LsPlayer::SetPlayerUpdate(const LmPeerUpdate& update)
{
  DECLARE_TheLineNum;
  LmLocker mon(u_lock_); // lock object for method duration
  set_update(update); // also updates position
}

void LsPlayer::SetParty(const LmParty& party)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  set_party(party);
}

void LsPlayer::SetInUse(bool inuse)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  in_use_ = inuse;
}

void LsPlayer::SetLocalGroup(const LmIdSet& lgroup)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  lgroup_ = lgroup;
}

void LsPlayer::SetNeighbors(const LmIdSet& neighbors)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  neighbors_ = neighbors;
}

void LsPlayer::AddJoiner(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  joiners_.Add(playerid);
}

void LsPlayer::RemoveJoiner(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  joiners_.Remove(playerid);
}

////
// ClearGroupsAndCopyParty: empty local group and neighbors, copy party
//   into local group (used at start of local group computation)
////

void LsPlayer::ClearGroupsAndCopyParty()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  lgroup_.Empty();
  neighbors_.Empty();
  for (int i = 0; i < party_.PartySize(); ++i) {
    lyra_id_t memberid = party_.PlayerID(i);
    if (memberid != playerid_) {
      lgroup_.Add(memberid);
    }
  }
}

void LsPlayer::AddGroupMember(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  lgroup_.Add(playerid);
}

void LsPlayer::AddNeighbor(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  neighbors_.Add(playerid);
}

void LsPlayer::EmptyParty()
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  party_.Empty();
  party_join_ = 0;
}

void LsPlayer::AddPartyMember(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  party_.AddPlayer(playerid);
}

void LsPlayer::RemovePartyMember(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  party_.RemovePlayer(playerid);
}

void LsPlayer::SetPartyJoinTime(time_t time)
{
  LmLocker mon(lock_); // lock object for method duration
  party_join_ = time;
}

bool LsPlayer::IsHidden() const
{
  LmLocker mon(lock_); // lock object for method duration
  return hidden_;
}

//bool LsPlayer::TCPOnly() const
//{ LmLocker mon(lock_); // lock object for method duration
  //return tcp_only_;}

void LsPlayer::SetHidden(bool hidden)
{
  LmLocker mon(lock_); // lock object for method duration
  hidden_ = hidden;
}

 void LsPlayer::SetChannelLevel(int channelLevel)
 {
    LmLocker mon(lock_);
    channellevel = channelLevel;
 }
 
 void LsPlayer::SetChannelTarget(lyra_id_t channelTarget)
 {
    LmLocker mon(lock_);
    channeltarget = channelTarget;
 }

//void LsPlayer::SetTCPOnly(bool tcp_only)
//{ LmLocker mon(lock_); // lock object for method duration
  //tcp_only_ = tcp_only;}

bool LsPlayer::IsNewlyAwakened() const
{
  LmLocker mon(lock_); // lock object for method duration
  return newly_awakened_;
}

void LsPlayer::SetNewlyAwakened(bool newly_awakened)
{
  LmLocker mon(lock_); // lock object for method duration
  newly_awakened_ = newly_awakened;
}

bool LsPlayer::GetNewlyAlerts() const
{
  LmLocker mon(lock_); // lock object for method duration
  return newly_alert_;
}

void LsPlayer::SetNewlyAlert(bool newly_alert)
{
  LmLocker mon(lock_); // lock object for method duration
  newly_alert_ = newly_alert;
}

bool LsPlayer::IsChannelling()
{
    LmLocker mon(lock_);
    return channeltarget != 0 && channeltarget != playerid_ &&
        channellevel > 0;
}


////
// Dump
////

void LsPlayer::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LsPlayer[%p,%d]: inuse=%d conn=[%p] player(%u)='%s' acct=%c online=%d>\n"),
	  this, sizeof(LsPlayer),
	  in_use_, conn_, playerid_, playername_, acct_type_, LmUtil::TimeSince(login_time_));
  avatar_.Dump(f, indent + 1);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("giving=%d taking=%d\n"), g_items_.size(), t_items_.size());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("roomid=%u lastposupdate=%lu (%d) party_join=%u hidden=%d stationary=%d\n"),  roomid_, last_pos_update_, LmUtil::TimeSince(last_pos_update_), party_join_, hidden_, stationary_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("pos="));
  pos_.Dump1(f);
 _ftprintf(f, _T("\n"));
  party_.Dump(f, indent + 1);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("localgroup: "));
  lgroup_.Dump(f, 0);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("neighbors: "));
  neighbors_.Dump(f, 0);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("joiners: "));
  joiners_.Dump(f, 0);
  pupdate_.Dump(f, indent + 1);
  update_addr_.Dump(f, indent + 1);
}

////
// clear_information
////

void LsPlayer::clear_information()
{
  DECLARE_TheLineNum;
  // initialize to unused state (assumes lock is held)


  in_use_ = false;
  playerid_ = Lyra::ID_UNKNOWN;
// *** STRING LITERAL ***  
 _tcsnccpy(playername_,  _T("(unknown)"), sizeof(playername_));
 _tcsnccpy(desc_, _T("none"), sizeof(desc_));
  acct_type_ = 'L';
  roomid_ = 0;
  last_pos_update_ = 0;
  pos_.Init(0,0,0,0);
  party_.Empty();
  party2_.Empty();
  party_join_ = 0;
  serv_port_ = 0;
  //avatar_.Init(Lyra::AVATAR_UNKNOWN);
  hidden_ = false;
  stationary_ = false;
//  tcp_only_ = false;
  lgroup_.Empty();
  lgroup2_.Empty();
  neighbors_.Empty();
  neighbors2_.Empty();
  joiners_.Empty();
  conn_ = 0;
  ipaddr_ = 0;
  client_ipaddr_ = 0;
  update_addr_.Init((unsigned long) 0, (unsigned short) 0);
  login_time_ = 0;
  t_items_.erase(t_items_.begin(), t_items_.end());
  g_items_.erase(g_items_.begin(), g_items_.end());
  channeltarget = 0;
  channellevel = 0;
}
