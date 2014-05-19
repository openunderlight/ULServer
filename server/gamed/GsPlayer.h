// GsPlayer.h  -*- C++ -*-
// $Id: GsPlayer.h,v 1.55 1998-05-11 11:11:35-07 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server player state

#ifndef INCLUDED_GsPlayer
#define INCLUDED_GsPlayer

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"
#include "LmPlayerDB.h"
#include "LmPosition.h"
#include "LmPeerUpdate.h"
#include "LmParty.h"
#include "LmIdSet.h"
#include "PThMutex.h"
#include "LmSockAddrInet.h"
#include "LmItem.h"

// class forward declarations

class GsMain;
class LmConnection;
class RmRemotePlayer;
class LmLevelDBC;
class LmGoalInfo;
class GMsg_ChangeStat;
class RMsg_PlayerUpdate;
class LmLog;
class GMsg_RcvGoalDetails;

// helper function for quest item completion
struct quest_item_t
{
	int graphic;
	int color1;
	int color2;
	int charges;
	int item_type;
	int field1;
	int field2;
	int field3;
};

// the class

class GsPlayer {

public:

  GsPlayer();
  ~GsPlayer();

  void SetMain(GsMain* gsmain);

  int Login(lyra_id_t playerid, int pmare_type);
  void Init(LmConnection* conn, int serv_port, LmLog* log, bool firewall, short tcp_only);
  void Logout(bool save = true);

  int SaveToFile() const;
  int SaveToDB(bool force);
  void RemoveLocalFile() const;

  // selectors
  lyra_id_t PlayerID() const;
  const LmPlayerDB& DB() const;
  const LmAvatar& Avatar() const;

  LmConnection* Connection() const;
  const LmSockAddrInet& ClientAddress() const;
  const LmSockAddrInet& UpdateAddress() const;

  LmConnection* LevelConnection() const;
  const LmSockAddrInet& LevelAddress() const;

  //  mpath_addr_t MPathAddress() const;

  bool InLevel() const;
  const LmLevelDBC* LevelDBC() const;
  lyra_id_t LevelID() const;
  lyra_id_t RoomID() const;

  lyra_id_t ReturnLevelID() const;
  lyra_id_t ReturnRoomID() const;

  lyra_id_t GoalReturnLevelID() const;
  lyra_id_t GoalReturnRoomID() const;

  const LmPeerUpdate& PlayerUpdate() const;
  time_t LastUpdateTime() const;
  int NumUpdates() const;
  int Online() const;

  // locking selectors
  bool InUse() const;
  int IdleTime(); // can't be const - modified idle_null_connection_

  bool CanUseArt(int art, int skill) const; 
  bool CanDreamStrike(lyra_id_t playerid) const;
  bool CanTrain(int art, int skill) const;
  bool CanBeTrained(int art, int skill) const;
  bool CanCompleteQuest(LmItem& item, GMsg_RcvGoalDetails& msg) const;
  void ItemToQuestItem (LmItem& item, quest_item_t& questitem) const;
  bool CanAcceptGoal(const LmGoalInfo& goalinfo) const;
  bool CanVoteOnGoal(const LmGoalInfo& goalinfo) const;
  bool CanGetGoalText(const LmGoalInfo& goalinfo) const;
  bool CanGetGoalDetails(const LmGoalInfo& goalinfo) const;
  bool CanGetGoalHeaders(int guild, int rank) const;
  bool CanGetReportHeaders(const LmGoalInfo& goalinfo) const;
  bool CanPostReport(const LmGoalInfo& goalinfo, int awardxp, bool source_in_acceptees,
		     lyra_id_t targetid, bool target_in_acceptees) const;
  bool CanPostGoal(int guild, int rank, int quest_xp) const;
  bool CanUpdateGoal(const LmGoalInfo& goalinfo) const;

  bool FindPrime(int guild_id, int min_charges) const;
  bool CanInitiate(int guild) const;
  bool CanBeInitiated(int guild) const;
  bool CanKnight(int guild) const;
  bool CanBeKnighted(int guild) const;
  bool CanDemote(int guild, int num_tokens, lyra_id_t playerid) const;
  bool CanBeDemoted(int guild, int num_tokens) const;
  bool CanAscend(int guild) const;
  int SphereTokens(lyra_id_t target_id) const;
  int TokensToDemote(int guild) const;

  bool CanSelfTrain(int art, TCHAR* names_buffer);

  bool CanGotoLevel(lyra_id_t levelid, lyra_id_t roomid) const;
  bool CanGotoRoom(lyra_id_t roomid) const;

  static int MinOrbit(int art);
  static int FocusStat(int art);
  static bool FocusOnly(int art);
  static bool Learnable(int art);
  static short PPMultiplier(int art_id);

  // mutators
  void SetInUse(bool inuse);
  void UndoDrops();
  void GotoLevel(LmConnection* lsconn, const LmLevelDBC* leveldb, lyra_id_t roomid);
  void GotoRoom(lyra_id_t roomid);
  void UpdateStats(const LmStats& stats, const LmArts& arts);
  int ChangeAvatar(const LmAvatar& avatar, int which);

  bool ChangeCurrentStat(int stat_num, int value);
  bool ChangeMaxStat(int stat_num, int value);
  bool ChangeGuildRank(int guild_num, int rank);
  bool ChangeSkill(int art_num, int skill, bool trained = false);
  bool ChangeXP(int xp);
  int AdjustXP(int xpdelta, GMsg_ChangeStat& changemsg);
  int AdjustOfflineXP(int xpdelta);
  void AdvanceToNextSphere(GMsg_ChangeStat& changemsg);
  int Demote(int guild_num, int& tokens_used, GMsg_ChangeStat& changemsg);
  void Initiate(int guild_num, lyra_id_t initiator);

  void SetPoolXP(int guild_num, int xp_pool);
  void SetQuestPoolXP(int xp_pool);
  void SetPPoints(int pp);
  void SetPPPool(int pool);
  int AcceptGoal(lyra_id_t goalid);
  int RemoveGoal(lyra_id_t goalid);
  void PostReport(int guild_num, int awardxp, lyra_id_t recipient);
  void PostQuest(int quest_xp);

  void ReceivedUpdate(const LmPeerUpdate& update);
  int CheckAndReceiveUpdate(LmPeerUpdate& update);
  int NumWeaponChecksFailed() const;
  void ReceivedServerUpdate(const RMsg_PlayerUpdate& player_update);
  void SetUpdateAddress(unsigned long clientip, int servport);

  void SaveReturnInfo();
  void SaveRecallInfo();
  void SaveGoalReturnInfo();

  bool CanAddItem(const LmItemHdr& itemhdr) const;
  bool CanCreateItem(const LmItem& item) const;
  int AddItem(const LmItem& item);
  void SetItemX(int serial, int item_x);

  int RemoveItem(const LmItemHdr& itemhdr);
  bool CanDestroyItem(const LmItemHdr& itemhdr) const;
  bool CanUpdateItem(const LmItem& item) const;
  void UpdateItem(const LmItem& item);
  bool CanDropItem(const LmItemHdr& itemhdr) const;

  void StartItemPickup(const LmItemHdr& itemhdr);
  void EndItemPickup(const LmItem& item, bool retrieved);
  void StartItemDrop(const LmItem& item);
  void EndItemDrop(const LmItemHdr& itemhdr, bool dropped);

  void StartItemTake(lyra_id_t playerid, const LmItem& item);
  void EndItemTake(const LmItemHdr& itemhdr, bool taken);
  lyra_id_t ItemGiver(const LmItemHdr& itemhdr) const;

  bool IsHidden() const;
  void SetHidden(bool hidden);

  bool DreamsoulDecreased() const;
  bool HasFired() const;
  bool BeenHit() const;
  int NumKills() const;
  int NumRecentDeaths() const;
  int NumOrbitKills() const;
  void SetNumRecentDeaths(int value);
  void SetNumOrbitKills(int value);
  int NumDeaths() const;
  void AddKill(int orbit);
  void AddDeath();
  bool Firewall() const;
  bool TCPOnly() const;
  int PPEvoking() const;
  void SetPPEvoking(int value);
  int PPSkill() const;
  void SetPPSkill(int value);

  void DumpInventory(const TCHAR* when = NULL);


  //  const TCHAR* Description() const;
  //  void SetDescription(const TCHAR* desc);

  void SetInLevel(bool in_level);

  void SetLog(LmLog* log);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  GsPlayer(const GsPlayer&);
  //operator=(const GsPlayer&);

  void clear_information();

  void remove_art(int art_id, GMsg_ChangeStat& changemsg);
  int adjust_xp(int xpdelta);
  bool has_neighbor(lyra_id_t target) const;
  //const LmPeerUpdate& neighbor_update(lyra_id_t target) const;
  int potential_inventory_size() const;
  bool check_update(LmPeerUpdate& update);
  bool can_update_state(const void* old_state, const void* new_state) const;
  int max_skill(int art_num) const;
  bool can_learn(int art_num) const;
  int save_to_file() const;
  int save_to_db(bool force);

  GsMain* main_;               // main object

  bool idle_null_connection_;   // if we've had a null connection on an idle check

  PThMutex lock_;          // mutex to protect object from multiple accesses

  LmLog* log_;                 // log file

  bool in_use_;                // object in use?

  bool is_mare_;                // true if a player mare

  LmPlayerDB db_;              // player database
  LmAvatar avatar_;            // current player avatar
  // TCHAR desc_[Lyra::MAX_AVATARDESC]; // player description

  time_t login_time_;   // UNIX time at login (used for pmares)

  int pmare_type_;             // stores pmare type for billing
  int num_kills_;              // number of times player dissolved another plaeyr
  int num_deaths_;             // number of times player was dissolved
  int num_orbit_kills_;      // # of orbits worth of dreamers collapsed during this phase
  int num_recent_deaths_;    // # of collapses inflicted on us during this phase
  bool has_fired_;             // true if player "fired a shot"
  bool been_hit_;              // true if player was hit
  bool ds_decreased_;          // true if player's DS has decreased
  bool hidden_;                // true if player is hidden (non-locatable)
  bool firewall_;			   // true if we need to send UDP updates back via the game server
  bool tcp_only_; // true if we're tcp only
  int pp_evoking_;			   // set to the art # being evoked via pp; usually NO_ART
  int pp_skill_;

  bool in_level_;              // player in level?
  const LmLevelDBC* ldbc_;       // current level
  lyra_id_t levelid_;          // ID of level player is currently in
  lyra_id_t roomid_;           // ID of room player is currently in

  lyra_id_t r_levelid_;        // ID of level player can RETURN to
  lyra_id_t r_roomid_;         // ID of room player can RETURN to

  lyra_id_t rc_levelid_;       // ID of level player can RECALL to
  lyra_id_t rc_roomid_;        // ID of room player can RECALL to

  lyra_id_t g_levelid_;        // ID of level player left to read goals
  lyra_id_t g_roomid_;         // ID of room player left to read goals

  std::list<LmItem> d_items_;       // items being dropped
  std::list<LmItemHdr> p_items_;    // items being picked up

  struct take_t {
    LmItem item;
    lyra_id_t playerid;
  };
  std::list<take_t> t_items_;       // items being taken

  LmConnection* conn_;         // connection to client
  LmSockAddrInet client_addr_; // IP address of client
  LmSockAddrInet update_addr_; // UDP address for S->C updates

  LmConnection* lsconn_;       // connection to level server
  LmSockAddrInet level_addr_;  // UDP address of level server (for C->S updates)

  //  mpath_addr_t mpaddr_;        // MPADDR for player

  PThMutex u_lock_;        // lock for update variables
  LmPeerUpdate update_;        // last position update received from client
  time_t last_update_;         // time of last received position update
  bool weapon_bad_;            // flag indicating last weapon check failed
  int num_bad_;                // number of weapon checks failed
  int num_updates_;            // number of updates received from client
  time_t last_mod_100_update_;  // time of last update where (update num)%100 = 0
  int num_too_fast_updates_;	   // # of times the player has sent > 10 updates in 1 sec

  PThMutex n_lock_;        // lock for neighbor variables
  int num_neighbors_;          // number of neighbors
  lyra_id_t nb_[Lyra::ROOMPEOPLE_MAX]; // neighbors

};


// macro to make debug logging easier (usage: PDEBUG(("level 3 warning %d", foo)); )
#define PDEBUG(x) if (log_) { log_->Debug x ; }

#ifdef USE_INLINE
#include "GsPlayer.i"
#endif

#endif /* INCLUDED_GsPlayer */
