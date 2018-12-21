// LmPlayerDB.h  -*- C++ -*-
// $Id: LmPlayerDB.h,v 1.23 1998-04-16 19:02:41-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player database class

#ifndef INCLUDED_LmPlayerDB
#define INCLUDED_LmPlayerDB

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../shared/LmStats.h"
#include "../server/LmInventory.h"
#include "../shared/LmAvatar.h"
#include "../shared/LmArts.h"
#include "../util/LmIdSet.h"
#include "../../libsrc/shared/SharedConstants.h"


// forward declarations

// the class

class LmPlayerDB {

public:

  enum {
    // player account types
    ACCT_ADMIN = 'A',    // (65) Lyra admin / gamemaster
    ACCT_KILLED = 'K',   // (75) killed (dreamstrike)
    ACCT_LOCKED = 'L',   // (76) locked out
    ACCT_MONSTER = 'M',  // (77) nightmare (agent)
    ACCT_PLAYER = 'P',   // (80) normal player
    ACCT_PMARE = 'S',   // (83) monster player 
    ACCT_ADMIN_EXPIRED = 'E', // (69) expired admin
    ACCT_PLAYER_EXPIRED = 'X', // (88) expired player

    // constants
    EMAIL_MAX = Lyra::EMAIL_MAX,
    REALNAME_MAX = 80
  };
  static const TCHAR* DEFAULT_PLAYERNAME;
  static const TCHAR* DEFAULT_PASSWORD;

public:

  LmPlayerDB();
  ~LmPlayerDB();

  void Init();

  bool CheckPassword(const TCHAR* password) const;
  int FixAvatar(bool modify);
  int CheckAvatar(const LmAvatar& avatar, int acct_type) const;
  bool PeggedBelowSphere() const;

  // selectors
  lyra_id_t PlayerID() const;
  lyra_id_t BillingID() const;
  int PMareBilling() const;
  const TCHAR* PlayerName() const;
  const TCHAR* Password() const;
  const TCHAR* AvatarDescrip() const;
  const LmStats& Stats() const;
  const LmAvatar& Avatar() const;
  const LmInventory& Inventory() const;
  const LmArts& Arts() const;
  time_t LastLogin() const;
  int TimeOnline() const;
  int NumLogins() const;
  const TCHAR* RealName() const;
  const TCHAR* Email() const;
  const LmIdSet& GoalBook() const;
  int AccountType() const;
  int XPGained() const;
  int XPLost() const;
  int NewlyAlert() const;
  int NewlyAwakened() const;
  short X() const;
  short Y() const;
  unsigned char LevelID() const;
  unsigned char Gamesite() const;
  lyra_id_t GamesiteID() const;

//  int TCPOnly() const;
  lyra_id_t Initiator(int guild_num) const;
//  bool PlayerModified() const;
//  bool StatModified() const;
//  bool SkillModified() const;
//  bool GuildPlayerModified() const;
//  bool GoalBookModified() const;

  // mutable selectors
  LmStats& Stats();
  LmInventory& Inventory();
  LmAvatar& Avatar();
  LmArts& Arts();
  LmIdSet& GoalBook();

  // mutators
  void UpdateStats(const LmStats& stats);
  void SetAvatar(const LmAvatar& avatar);
  void SetArts(const LmArts& arts);
  int AcceptGoal(lyra_id_t goalid);
  int RemoveGoal(lyra_id_t goalid);
  void SetLastLogin(time_t time);
  void SetTimeOnline(int online);
  void SetNumLogins(int logins);

  void SetAccountType(int acct_type);
  void SetPlayerID(lyra_id_t playerid);
  void SetBillingID(lyra_id_t billing_id);
  void SetPMareBilling(int pmare_billing);
  void SetPlayerName(const TCHAR* playername);
  void SetPassword(const TCHAR* password);
  void SetRealName(const TCHAR* realname);
  void SetAvatarDescrip(const TCHAR* descrip);
  void SetEmail(const TCHAR* email);
  void SetXPGained(int xp_gain);
  void SetXPLost(int xp_loss);
  void SetInitiator(int guild_num, lyra_id_t initiator);
  void SetNewlyAlert(int newly_alert);
  void SetNewlyAwakened(int newly_awakened);
  void SetX(short x);
  void SetY(short y);
  void SetLevelID(unsigned char level_id);
  void SetGamesite(unsigned char gamesite);
  void SetGamesiteID(lyra_id_t gamesite_id);

//  void SetTCPOnly(int tcp_only);
//  void ClearModified(void);

  void Dump(FILE* f, int indent = 0) const;

private:

  friend class LmPlayerDBF;

  // not implemented
  LmPlayerDB(const LmPlayerDB&);
  //operator=(const LmPlayerDB&);

  // these bools determine if info in a particular database table has changed
  // we use them to determine what to save
//  bool player_modified_;

  // character information
  lyra_id_t playerid_;
  TCHAR playername_[Lyra::PLAYERNAME_MAX];
  TCHAR password_[Lyra::PASSWORD_MAX];
  LmStats stats_;
  LmInventory inv_;
  LmAvatar avatar_;
  LmArts arts_;
  LmIdSet goalbook_;
  int acct_type_;
  TCHAR avatar_descrip_[Lyra::MAX_AVATARDESC];

  // who initiated person into guild
  lyra_id_t initiators_[NUM_GUILDS];

  // xp gained/lost due to "offline" initiations/demotions
  int xp_gain_;
  int xp_loss_;

  // personal information
  lyra_id_t billing_id_;
  TCHAR email_[EMAIL_MAX];
  TCHAR realname_[REALNAME_MAX];

  // bookkeeping
  time_t last_login_;
  int newly_alert_;
  int newly_awakened_;
//  int tcp_only_;
  int time_online_;
  int num_logins_;
  int pmare_billing_;
  short x_;
  short y_;
  unsigned char level_id_;
  unsigned char gamesite_;
  lyra_id_t gamesite_id_;

};

#ifdef USE_INLINE
#include "LmPlayerDB.i"
#endif

#endif /* INCLUDED_LmPlayerDB */
