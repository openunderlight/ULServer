// LsPlayer.h  -*- C++ -*-
// $Id: LsPlayer.h,v 1.28 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server player state

#ifndef INCLUDED_LsPlayer
#define INCLUDED_LsPlayer

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>
#include <unordered_map>
//#include <hash_map.h>
#include <time.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmPosition.h"
#include "LmParty.h"
#include "LmIdSet.h"
#include "PThMutex.h"
#include "LmPeerUpdate.h"
#include "LmSockAddrInet.h"
#include "LmAvatar.h"
#include "LmItem.h"

// class forward declarations

class LsMain;
class LmConnection;
class RmRemotePlayer;
class LmUpdateSetImp;

// an item being given/taken

struct LsLastUpdate {
  LmPeerUpdate last_update;
  int num_updates;
};

class LsUpdateSet :
  public std::unordered_map< lyra_id_t, LsLastUpdate*, std::hash<lyra_id_t>, std::equal_to<lyra_id_t> > {
  // nothing added
};

struct LsGiveTakeItem {
  LmItem item;
  lyra_id_t playerid;
};

class LsGiveTakeItemList : public std::list<LsGiveTakeItem> {
  // no additional methods
};


// the class

class LsPlayer {

public:

  LsPlayer();
  ~LsPlayer();

  int Login(LsMain* lsmain, lyra_id_t playerid);
  void Init(LmConnection* conn, const TCHAR* playername, int acct_type, lyra_id_t roomid,
	    const LmPeerUpdate& update, unsigned long client_addr, int servport, const LmAvatar& avatar,
	    const TCHAR* description, bool newly_alert, bool newly_awakened);
  void Logout();

  void CopyInfo(RmRemotePlayer& rp);

  // selectors
  lyra_id_t PlayerID() const;
  const TCHAR* PlayerName() const;
  int AccountType() const;
  int ServerPort() const;
  int Online() const;
  const LmSockAddrInet& UpdateAddress() const;
  const LmAvatar& Avatar() const;
  time_t LastPositionUpdate() const;
  LmConnection* Connection() const;
  unsigned long IPAddress() const;
  unsigned long ClientIPAddress() const;
  unsigned short RealtimeID() const;

  // locking selectors
  lyra_id_t RoomID() const;
  const LmPosition& Position() const;
  const LmPeerUpdate& LastUpdate() const;
  const LmParty& Party() const;
  bool InUse() const;
  const LmIdSet& LocalGroup() const;
  const LmIdSet& Neighbors() const;
  const LmPeerUpdate& PlayerUpdate() const;
  int IdleTime() const;
  time_t PartyJoinTime() const;
  LsUpdateSet* UpdateSet() const;

  // mutators
  void SetAvatar(const LmAvatar& avatar);
  void GotoRoom(lyra_id_t roomid, const LmPeerUpdate& update);
  void SetPlayerUpdate(const LmPeerUpdate& update);
  void SetParty(const LmParty& party);
  void SetPartyJoinTime(time_t time);
  void SetInUse(bool inuse);
  void SetRealtimeID(unsigned short realtimeid);

  void ClearGroupsAndCopyParty();
  void AddGroupMember(lyra_id_t playerid);
  void AddNeighbor(lyra_id_t playerid);
  void EmptyParty();
  void AddPartyMember(lyra_id_t playerid);
  void RemovePartyMember(lyra_id_t playerid);
  void SetUpdateAddress(unsigned long clientip, int servport);

  void AddJoiner(lyra_id_t playerid);
  void RemoveJoiner(lyra_id_t playerid);
  const LmIdSet& Joiners() const;

  void GivingItem(lyra_id_t takerid, const LmItem& item);
  void GaveItem(const LmItemHdr& hdr);
  bool IsGivingItem(lyra_id_t takerid, const LmItemHdr& hdr) const;
  const LsGiveTakeItemList& GivingList() const;

  void TakingItem(lyra_id_t giverid, const LmItem& item);
  void TookItem(const LmItemHdr& hdr);
  bool IsTakingItem(lyra_id_t giverid, const LmItemHdr& hdr) const;
  const LsGiveTakeItemList& TakingList() const;

  bool IsHidden() const;
//  bool TCPOnly() const;
  bool IsNewlyAwakened() const;
  bool GetNewlyAlerts() const;
  void SetHidden(bool hidden);
  void SetNewlyAlert(bool newly_alert);
  void SetNewlyAwakened(bool newly_awakened);
//  void SetTCPOnly(bool tcp_only);

  const TCHAR* Description() const;
  void SetDescription(const TCHAR* desc);

  void Dump(FILE* f, int indent = 0) const;
  void SetChannelLevel(int channelLevel);
  void SetChannelTarget(lyra_id_t channelTarget);
  lyra_id_t ChannelTarget();
  int ChannelLevel();
  bool IsChannelling();
  
private:

  void SetLocalGroup(const LmIdSet& lgroup);
  void SetNeighbors(const LmIdSet& neighbors);

  // not implemented
  LsPlayer(const LsPlayer&);
  //operator=(const LsPlayer&);

  void clear_information();

  // non-MT safe mutators
  void set_roomid(lyra_id_t roomid);
  void set_party(const LmParty& party);
  void set_update(const LmPeerUpdate& update);

  PThMutex lock_;

  bool in_use_;

  LsUpdateSet* neighbor_updates_;

  lyra_id_t playerid_;
  unsigned short realtimeid_; // used to optimize real-time update packets
  TCHAR playername_[Lyra::PLAYERNAME_MAX];
  int acct_type_;
  time_t login_time_;

  lyra_id_t roomid_;
  time_t last_pos_update_;
  LmParty party_, party2_;
  time_t party_join_;
  LmAvatar avatar_;
  TCHAR desc_[Lyra::MAX_AVATARDESC];
  bool hidden_;
  bool newly_alert_;
  bool newly_awakened_;
  bool stationary_;
//  bool tcp_only_;

  PThMutex u_lock_; // update mutex
  LmPosition pos_;
  LmPeerUpdate last_update_;
  
  LmPeerUpdate pupdate_, pupdate2_;

  LmIdSet lgroup_, lgroup2_;
  LmIdSet neighbors_, neighbors2_;

  LmIdSet joiners_;

  LsGiveTakeItemList t_items_; // items being taken
  LsGiveTakeItemList g_items_; // items being given

  LmConnection* conn_;
  int serv_port_;
  unsigned long ipaddr_;
  unsigned long client_ipaddr_;
  LmSockAddrInet update_addr_;
  lyra_id_t channeltarget;
  unsigned int channellevel;
};

#ifdef USE_INLINE
#include "LsPlayer.i"
#endif

#endif /* INCLUDED_LsPlayer */
