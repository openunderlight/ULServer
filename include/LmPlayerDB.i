// LmPlayerDB.i  -*- C++ -*-
// $Id: LmPlayerDB.i,v 1.16 1997-11-20 16:26:34-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE lyra_id_t LmPlayerDB::PlayerID() const
{
  return playerid_;
}

INLINE lyra_id_t LmPlayerDB::BillingID() const
{
  return billing_id_;
}

INLINE int LmPlayerDB::PMareBilling() const
{
  return pmare_billing_;
}


INLINE const TCHAR* LmPlayerDB::PlayerName() const
{
  return playername_;
}

INLINE const TCHAR* LmPlayerDB::Password() const
{
  return password_;
}


INLINE const TCHAR* LmPlayerDB::AvatarDescrip() const
{
  return avatar_descrip_;
}

INLINE const LmStats& LmPlayerDB::Stats() const
{
  return stats_;
}

INLINE const LmArts& LmPlayerDB::Arts() const
{
  return arts_;
}

INLINE const LmAvatar& LmPlayerDB::Avatar() const
{
  return avatar_;
}

INLINE const LmInventory& LmPlayerDB::Inventory() const
{
  return inv_;
}

INLINE time_t LmPlayerDB::LastLogin() const
{
  return last_login_;
}

INLINE int LmPlayerDB::TimeOnline() const
{
  return time_online_;
}

INLINE int LmPlayerDB::NumLogins() const
{
  return num_logins_;
}

INLINE const TCHAR* LmPlayerDB::RealName() const
{
  return realname_;
}

INLINE const TCHAR* LmPlayerDB::Email() const
{
  return email_;
}

INLINE int LmPlayerDB::AccountType() const
{
  return acct_type_;
}

INLINE LmStats& LmPlayerDB::Stats()
{
  return stats_;
}

INLINE LmArts& LmPlayerDB::Arts()
{
  return arts_;
}

INLINE LmAvatar& LmPlayerDB::Avatar()
{
  return avatar_;
}

INLINE LmInventory& LmPlayerDB::Inventory()
{
  return inv_;
}

INLINE int LmPlayerDB::XPGained() const
{
  return xp_gain_;
}

INLINE int LmPlayerDB::XPLost() const
{
  return xp_loss_;
}

INLINE lyra_id_t LmPlayerDB::Initiator(int guild_num) const
{
  return initiators_[guild_num];
}

INLINE int LmPlayerDB::NewlyAlert() const
{
  return newly_alert_;
}

INLINE int LmPlayerDB::NewlyAwakened() const
{
  return newly_awakened_;
}

INLINE short LmPlayerDB::X() const
{
  return x_;
}

INLINE short LmPlayerDB::Y() const
{
  return y_;
}

INLINE unsigned char LmPlayerDB::LevelID() const
{
  return level_id_;
}

INLINE unsigned char LmPlayerDB::Gamesite() const
{
  return gamesite_;
}

INLINE lyra_id_t LmPlayerDB::GamesiteID() const
{
  return gamesite_id_;
}


//INLINE int LmPlayerDB::TCPOnly() const
//{  return tcp_only_; }

/*
INLINE bool LmPlayerDB::PlayerModified() const
{
  if (player_modified_ || stats_.XPModified() || avatar_.Modified())
	return true;
  else 
    return false;
}

INLINE bool LmPlayerDB::StatModified() const
{
  return stats_.StatsModified();
}

INLINE bool LmPlayerDB::SkillModified() const
{
  return arts_.Modified();
}

INLINE bool LmPlayerDB::GuildPlayerModified() const
{
  return stats_.GuildModified();
}
*/


INLINE void LmPlayerDB::UpdateStats(const LmStats& stats)
{
  stats_ = stats;
}

INLINE void LmPlayerDB::SetArts(const LmArts& arts)
{
  arts_ = arts;
}

INLINE void LmPlayerDB::SetLastLogin(time_t time)
{
  last_login_ = time;
}

INLINE void LmPlayerDB::SetTimeOnline(int online)
{
  time_online_ = online;
//  player_modified_ = true;
}

INLINE void LmPlayerDB::SetNumLogins(int logins)
{
  num_logins_ = logins;
//  player_modified_ = true;
}

INLINE void LmPlayerDB::SetAvatar(const LmAvatar& avatar)
{
  avatar_ = avatar;
//  player_modified_ = true;
}

INLINE const LmIdSet& LmPlayerDB::GoalBook() const
{
  return goalbook_;
}

INLINE LmIdSet& LmPlayerDB::GoalBook()
{
  return goalbook_;
}

INLINE int LmPlayerDB::AcceptGoal(lyra_id_t goalid)
{
  int retval = 0;
  if (goalid != 0) {
    retval = goalbook_.Add(goalid);
  }
  return retval;
}

INLINE int LmPlayerDB::RemoveGoal(lyra_id_t goalid)
{
  return goalbook_.Remove(goalid);
}

INLINE void LmPlayerDB::SetPlayerID(lyra_id_t playerid)
{
  playerid_ = playerid;
}

INLINE void LmPlayerDB::SetBillingID(lyra_id_t billing_id)
{
  billing_id_ = billing_id;
}


INLINE void LmPlayerDB::SetPMareBilling(int pmare_billing)
{
  pmare_billing_ = pmare_billing;
}

INLINE void LmPlayerDB::SetPlayerName(const TCHAR* playername)
{
 _tcscpy(playername_, playername);
}

INLINE void LmPlayerDB::SetPassword(const TCHAR* password)
{
 _tcscpy(password_, password);
}

INLINE void LmPlayerDB::SetAvatarDescrip(const TCHAR* descrip)
{
 _tcscpy(avatar_descrip_, descrip);
//  player_modified_ = true;
}

INLINE void LmPlayerDB::SetRealName(const TCHAR* realname)
{
 _tcscpy(realname_, realname);
}

INLINE void LmPlayerDB::SetEmail(const TCHAR* email)
{
 _tcscpy(email_, email);
}

INLINE void LmPlayerDB::SetAccountType(int acct_type)
{
  acct_type_ = acct_type;
}

INLINE void LmPlayerDB::SetXPGained(int xp_gain)
{
  xp_gain_ = xp_gain;
  //player_modified_ = true;
}

INLINE void LmPlayerDB::SetXPLost(int xp_loss)
{
  xp_loss_ = xp_loss;
  //player_modified_ = true;
}

INLINE void LmPlayerDB::SetInitiator(int guild_num, lyra_id_t initiator)
{
  initiators_[guild_num] = initiator;
}
 
INLINE void LmPlayerDB::SetNewlyAlert(int newly_alert)
{
  newly_alert_ = newly_alert;
}

INLINE void LmPlayerDB::SetNewlyAwakened(int newly_awakened)
{
  newly_awakened_ = newly_awakened;
}

INLINE void LmPlayerDB::SetX(short x)
{
	x_ = x;
}

INLINE void LmPlayerDB::SetY(short y)
{
	y_ = y;
}

INLINE void LmPlayerDB::SetLevelID(unsigned char level_id)
{
	level_id_ = level_id;
}

INLINE void LmPlayerDB::SetGamesite(unsigned char gamesite)
{
  gamesite_ = gamesite;
}

INLINE void LmPlayerDB::SetGamesiteID(lyra_id_t gamesite_id)
{
  gamesite_id_ = gamesite_id;
}


//INLINE void LmPlayerDB::SetTCPOnly(int tcp_only)
//{  tcp_only_ = tcp_only; }

/*

INLINE void LmPlayerDB::ClearModified(void)
{
  player_modified_ = false; 
  arts_.SetModified(false);
  avatar_.SetModified(false);
  stats_.SetStatsModified(false);
  stats_.SetXPModified(false);
  stats_.SetGuildModified(false);
}
*/