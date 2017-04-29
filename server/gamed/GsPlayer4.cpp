// GsPlayer4.cpp  -*- C++ -*-
// $Id: GsPlayer4.cpp,v 1.15 1998/04/17 02:08:10 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// methods dealing with stats/skills/xp

#include <stdio.h>
#include <stdlib.h>

#include "GsPlayer.h"
#include "PThMutex.h"
#include "GsMain.h"
#include "LmPlayerDB.h"
#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSockType.h"
#include "LmSockAddrInet.h"
#include "LmLogFile.h"
#include "LmLevelDBC.h"
#include "GMsg_All.h"
#include "LmLocker.h"
#include "LmGoalInfo.h"
#include "GMsg_RcvGoalDetails.h"
#include "RMsg_PlayerUpdate.h"
#include "LmUtil.h"

////
// SetPoolXP
////

void GsPlayer::SetPoolXP(int guild_num, int xp_pool)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Stats().SetPoolXP(guild_num, xp_pool);
}

////
// SetQuestPoolXP
////

void GsPlayer::SetQuestPoolXP(int xp_pool)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Stats().SetQuestPoolXP(xp_pool);
}

////
// SetPPoints
////

void GsPlayer::SetPPoints(int pp)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Stats().SetPP(pp);
}

////
// SetPPPool
////

void GsPlayer::SetPPPool(int pool)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Stats().SetPPPool(pool);
}




////
// UpdateStats - update player stats
////

void GsPlayer::UpdateStats(const LmStats& stats, const LmArts& /* arts */)
{
  LmLocker mon(lock_); // lock object during method duration
  // update only the current stats
  for (int stat = 0; stat < NUM_PLAYER_STATS; ++stat) {
    // check if stat is DS, and if it decreased
    if ((stat == Stats::DREAMSOUL) && (stats.CurrentStat(stat) < db_.Stats().CurrentStat(stat))) {
      ds_decreased_ = true;
    }
    db_.Stats().SetCurrentStat(stat, stats.CurrentStat(stat));
  }
#if 0
  db_.UpdateStats(stats);
  db_.SetArts(arts);
#endif
}

////
// ChangeCurrentStat - change player's current stat; return true if successful, false if not
////

bool GsPlayer::ChangeCurrentStat(int stat_num, int value)
{
  LmLocker mon(lock_); // lock object during method duration
  bool can_change = false;
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || (db_.AccountType() == LmPlayerDB::ACCT_MONSTER) 
      || db_.Stats().CanSetCurrentStat(stat_num, value)) {
    can_change = true;
  }
  
  // make the change
  if (can_change) {
    if ((stat_num == Stats::DREAMSOUL) &&
	(value < db_.Stats().CurrentStat(stat_num))) 
      ds_decreased_ = true;
 
    db_.Stats().SetCurrentStat(stat_num, value);
  }
  return can_change;
}

////
// ChangeXP - change player's XP; return true if successful, false if not
////

bool GsPlayer::ChangeXP(int xp)
{
  LmLocker mon(lock_); // lock object during method duration
  // TODO: check if this is really ok
  db_.Stats().SetXP(xp);
  return true;
}

////
// AdvanceToNextSphere - update player's record, fill in changestat with changes
////

void GsPlayer::AdvanceToNextSphere(GMsg_ChangeStat& changemsg)
{
  LmLocker mon(lock_); // lock object during method duration
  // init message
  changemsg.Init(0);
  // check that sphere advancement is possible
  int new_sphere = db_.Stats().Sphere() + 1;
  if (!db_.Stats().ReadyToAdvance(new_sphere)) {
    return;
  }
  db_.Stats().GainOrbit(new_sphere * 10);
  db_.Stats().AdvanceToSphere(new_sphere);
  // max stats (may have) changed, due to orbit change; send them all to client
  changemsg.SetNumChanges(6); // XP + 5 max
  changemsg.InitChange(0, GMsg_ChangeStat::SET_XP, 0, db_.Stats().XP());
  for (int i = 0; i < NUM_PLAYER_STATS; ++i) {
    changemsg.InitChange(1 + i, GMsg_ChangeStat::SET_STAT_MAX, i, db_.Stats().MaxStat(i));
  }
}

////
// AdjustXP - adjust player's XP by given amount, return actual adjustment and fill in
//   ChangeStat message with all changes (to xp and stats, if orbit changes); handles
//   both positive and negative adjustments
////

int GsPlayer::AdjustXP(int xpdelta, GMsg_ChangeStat& changemsg)
{
  LmLocker mon(lock_); // lock object during method duration
  // init message to "no-op"
  changemsg.Init(0);
  if (xpdelta == 0) {
    return 0;
  }
  // if player is a monster, don't do anything
  if (db_.AccountType() == LmPlayerDB::ACCT_MONSTER)
  {
    return 0;
  }

  // add/sub xp from player
  int old_orbit = db_.Stats().Orbit();
  int xp_adj = adjust_xp(xpdelta);
  // check if an adjustment was actually made
  if (xp_adj == 0) {
    return 0;
  }
  // xp change was made, so put that in changestat message
  changemsg.SetNumChanges(1);
  changemsg.InitChange(0, GMsg_ChangeStat::SET_XP, 0, db_.Stats().XP());
  // dbetermine if orbit changed
  int new_orbit = db_.Stats().Orbit();
  
  if (db_.AccountType() == LmPlayerDB::ACCT_PLAYER) {
    if (old_orbit != new_orbit) { // orbit has changed
      // max stats (may have) changed, due to orbit change; send them all to client
      changemsg.SetNumChanges(6); // XP + 5 max
      for (int i = 0; i < NUM_PLAYER_STATS; ++i) {
	changemsg.InitChange(1 + i, GMsg_ChangeStat::SET_STAT_MAX, i, db_.Stats().MaxStat(i));
      }
    }
  }

  // return actual adjustment
  return xp_adj;
}


// AdjustOfflineXP - adjust player's offline XP by given amount, return actual adjustment and fill in
//   ChangeStat message with all changes (to xp and stats, if orbit changes); handles
//   both positive and negative adjustments
////

// if xpdelta is zero, zero everything
int GsPlayer::AdjustOfflineXP(int xpdelta)
{
  LmLocker mon(lock_); // lock object during method duration

  if ((db_.AccountType() == LmPlayerDB::ACCT_MONSTER) || 
      (db_.AccountType() == LmPlayerDB::ACCT_PMARE)) {
    return 0;
  }

  if (xpdelta == 0)
    {
      db_.SetXPGained(0);
      db_.SetXPLost(0);
    } 
  else if (xpdelta > 0)
    {
  // add/sub xp from player
  int new_gain = db_.XPGained() + xpdelta;
  db_.SetXPGained(new_gain);
    return 0;
  } else 
    {
  // add/sub xp from player
  int new_lost = db_.XPLost() + xpdelta;
  db_.SetXPLost(new_lost);
    return 0;

    }
  return xpdelta;
}


////
// adjust_xp - non-locking method helper; return actual adjustment made
////

int GsPlayer::adjust_xp(int xpdelta)
{
  // add/sub xp from player
  int old_orbit = db_.Stats().Orbit();
  int xp_adj = 0;

  if (db_.AccountType() == LmPlayerDB::ACCT_PMARE) {
    xp_adj = db_.Stats().PMareAdjustXP(xpdelta);
  }  else {
    xp_adj = db_.Stats().AdjustXP(xpdelta);

    // check if an adjustment was actually made
    if (xp_adj == 0) {
      return 0;
    }
    // determine if orbit changed
    int new_orbit = db_.Stats().Orbit();
    if (old_orbit < new_orbit) { // gained an orbit
      db_.Stats().GainOrbit(new_orbit);
    }
    else if (old_orbit > new_orbit) { // lost an orbit
      db_.Stats().LoseOrbit(new_orbit);
    }
  }

  return xp_adj;
}

////
// ChangeSkill - change player's skill in given art; return true if OK, false if not
//   handles training by GMs and players, as well as self-learning
////

bool GsPlayer::ChangeSkill(int art_num, int skill, bool trained)
{
  DEFMETHOD(GsPlayer, ChangeSkill);
  LmLocker mon(lock_); // lock object during method duration
  int my_skill = db_.Arts().Skill(art_num);
  bool trained_by_gm = false;
  bool house_art = HouseArt(art_num);
  if (trained && (skill > 100)) {
    trained_by_gm = true;
    skill = skill - 100;
  }
#if 0
  // check for certain non-trainable arts here
  switch (art_num) {
  case Arts::LEVELTRAIN: // can't change this skill
    return false;
  default:
    break;
  }
#endif
  // are the art/skill combo valid?
  if (!db_.Arts().CanSetSkill(art_num, skill)) {
    PDEBUG((_T("%s: art/skill combination invalid"), method));
    return false;
  }
  // admins can do anything
  if (db_.AccountType() == LmPlayerDB::ACCT_ADMIN) {
    db_.Arts().SetSkill(art_num, skill);
    return true;
  }
  // is this a no-op?
  if (my_skill == skill) {
    return true;
  }
  // can we learn this on our own?
  if ((!trained) && (!house_art)) {
    // can only increase by 1, can't learn something by oneself, can't go past plateau boundary
    if (((my_skill % 10) == 9) || (skill != (my_skill + 1)) || (my_skill == 0)) {
      PDEBUG((_T("%s: not trained, and cannot learn more"), method));
      return false;
    }
    // check that we haven't reached the maximum skill level based on focus stat, orbit
    if (skill > max_skill(art_num)) {
      return false;
    }
  }
  // being trained - automatically OK, just reset skill to next level, or if 
  // trained by a gm, to the absolute skill level (gms can decrease skills too)
  else {
    if (!trained_by_gm) {
      // check that we can learn this skill based on focus stat, orbit
      if (!can_learn(art_num)) {
	return false;
      }
      skill = my_skill + 1;
    }
  }
  // finally, make the change
  db_.Arts().SetSkill(art_num, skill);
  return true;
}

////
// ChangeMaxStat - change player's max stat; return true if OK, false if not
////

bool GsPlayer::ChangeMaxStat(int stat_num, int value)
{
  LmLocker mon(lock_); // lock object during method duration
  // TODO: check if this is really ok
  bool can_change = false;
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || db_.Stats().CanSetMaxStat(stat_num, value)) {
    can_change = true;
  }
  if (can_change) {
    db_.Stats().SetMaxStat(stat_num, value);
  }
  return can_change;
}

////
// ChangeAvatar - change player's avatar (both permanent and session handled)
////

int GsPlayer::ChangeAvatar(const LmAvatar& avatar, int which)
{
  LmLocker mon(lock_); // lock object during method duration
  int retval = 0; // successful
  if (which == GMsg_ChangeAvatar::AVATAR_PERMANENT) {
    db_.SetAvatar(avatar);
    retval = db_.FixAvatar(true);
    avatar_ = db_.Avatar(); // get fixed version from db
  }
  else {
    retval = db_.CheckAvatar(avatar, db_.AccountType());
    if (retval == 0) { // avatar is ok
      avatar_ = avatar;
    }
  }
  return retval;
}

////
// CanUseArt - return true if player has at least the given skill in the given art
////

bool GsPlayer::CanUseArt(int art, int skill) const
{
  LmLocker mon(lock_); // lock object during method duration
  // valid art
  if (!LmArts::ArtNumOK(art)) {
    return false;
  }


  // check skill level
  if (skill > db_.Arts().Skill(art)) {
    return false;
  }
  // ok
  return true;
}

////
// CanDreamStrike - return true if player is allowed to use dreamstrike art against target
////

bool GsPlayer::CanDreamStrike(lyra_id_t /* target */) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must be orbit 60 or higher
  if (db_.Stats().Orbit() < 10) {
    return false;
  }

  // dreamstrike not allowed on many levels
  for (int i=0; i<num_no_dreamstrike_levels; i++) 
	if (no_dreamstrike_levels[i] == levelid_)
		return false;

  // max and current dreamsoul must be greater than a minimum
  int min_ds = 25;
  if ((db_.Stats().MaxStat(Stats::DREAMSOUL) <= min_ds) ||
      (db_.Stats().CurrentStat(Stats::DREAMSOUL) <= min_ds)) {
    return false;
  }
  // must have the dreamstrike art
  if (db_.Arts().Skill(Arts::DREAMSTRIKE) < 1) {
    return false;
  }
  // ok
  return true;
}

// Check if an art is an Auto-Trained House Art.  Return True if yes, False if no.
bool GsPlayer::HouseArt(int art) const {
	 switch (art) {
		case Arts::HOUSE_MEMBERS:
		case Arts::CUP_SUMMONS:
		case Arts::ASCEND:
		case Arts::INITIATE:
		case Arts::EMPATHY:
		case Arts::SUPPORT_DEMOTION:
		case Arts::SUPPORT_ASCENSION:
		case Arts::DEMOTE:
		case Arts::POWER_TOKEN:
		case Arts::EXPEL:
		case Arts::KNIGHT:
		case Arts::CREATE_ID_TOKEN:
		case Arts::SUMMON_PRIME:
			return true;
			break;
		default:
			return false;
	  }
}

////
// CanTrain - return true if player can train the given art at the given skill level
////

bool GsPlayer::CanTrain(int art, int skill) const
{
  LmLocker mon(lock_); // lock object during method duration
  // admins can do anything
  if (db_.AccountType() == LmPlayerDB::ACCT_ADMIN) {
    return true;
  }
  // non-GM checks below
  // art/skill must be valid
  if (!db_.Arts().CanSetSkill(art, skill)) {
    return false;
  }

  // cannot train the train skills 
  if ((art == Arts::TRAIN && db_.Arts().Skill(Arts::TRAIN_SELF) < 1) || (art == Arts::LEVELTRAIN) ||
      (art == Arts::TRAIN_SELF) || (art == Arts::DREAMSMITH_MARK) ||
	  (art == Arts::NP_SYMBOL) || (art == Arts::WORDSMITH_MARK)) {
    return false;
  }
#if 0
  // level training? -- handled elsewhere
  if (art == Arts::LEVELTRAIN) {
    // skill is sent as orbit
    if (skill > db_.Stats().Orbit()) {
      return false;
    }
    // skill /= 10;
    // skill is equivalent to highest sphere person can leveltrain, which is defined
    // as one below their current sphere
    int train_sphere = db_.Stats().Sphere() - 1;
    // if (skill > train_sphere) {
    // return false;
    // }
    // must have TRAIN at appropriate skill level (10 * target sphere)
    if ((train_sphere * 10) > db_.Arts().Skill(Arts::TRAIN)) {
      return false;
    }
  }
#endif
  // normal skill training

  // auto-training house arts doesn't require skill/halo
  bool house_art = HouseArt(art);
  if (house_art) {
	  return true;
  }
  


  // Convert the flame/blade from the Quest focus to the Student's focus -- takes the first available art
  switch (art)
  {
  case Arts::SOULREAPER:
  case Arts::GATESMASHER:
  case Arts::DREAMBLADE:
  case Arts::FATESLAYER:
	  if (db_.Arts().Skill(Arts::DREAMBLADE) > 0)
		  art = Arts::DREAMBLADE;
	  else if (db_.Arts().Skill(Arts::SOULREAPER) > 0)
		  art = Arts::SOULREAPER;
	  else if (db_.Arts().Skill(Arts::GATESMASHER) > 0)
		  art = Arts::GATESMASHER;
	  else if (db_.Arts().Skill(Arts::FATESLAYER) > 0)
		  art = Arts::FATESLAYER;
	  break;
  case Arts::TRANCEFLAME:
  case Arts::FLAMESEAR:
  case Arts::FLAMESHAFT:
  case Arts::FLAMERUIN:
	  if (db_.Arts().Skill(Arts::TRANCEFLAME) > 0)
		  art = Arts::TRANCEFLAME;
	  else if (db_.Arts().Skill(Arts::FLAMESEAR) > 0)
		  art = Arts::FLAMESEAR;
	  else if (db_.Arts().Skill(Arts::FLAMESHAFT) > 0)
		  art = Arts::FLAMESHAFT;
	  else if (db_.Arts().Skill(Arts::FLAMERUIN) > 0)
		  art = Arts::FLAMERUIN;
	  break;
  default:
	  // do nothing
	  break;
  }

  // must have art at that skill or greater
  if (skill > db_.Arts().Skill(art)) {
    return false;
  }
  // must have TRAIN at that skill or greater
  if (skill > db_.Arts().Skill(Arts::TRAIN)) {
    return false;
  }
  // ok
  return true;
}

////
// CanBeTrained - return true if player can be trained the given art at the given skill
//   (actually, only if the given skill is greater than the player's current skill,
//   and player is right below a plateau)
////

bool GsPlayer::CanBeTrained(int art, int skill) const
{
  LmLocker mon(lock_); // lock object during method duration
  // monsters and gms cannot be trained
  if ((db_.AccountType() == LmPlayerDB::ACCT_MONSTER) ||
      (db_.AccountType() == LmPlayerDB::ACCT_PMARE) ||
      (db_.AccountType() == LmPlayerDB::ACCT_ADMIN)) {
    return false;
  }
  // being trained by a gm?
  if (skill > 100) {
    return true;
  }

  // learning an art

  // must have skill below skill being trained
  int my_skill = db_.Arts().Skill(art);
  if (my_skill >= skill) {
    return false;
  }
  // check if current skill is the maximum we can have
  if (my_skill >= max_skill(art)) {
    return false;
  }
  // check if we can learn it, based on focus/orbit
  if (!can_learn(art)) {
    return false;
  }
  // check for specific arts
  switch (art) {
    // must be a knight or ruler in some house
    case Arts::INITIATE:
    case Arts::ASCEND:
	case Arts::SUPPORT_DEMOTION:
	case Arts::SUPPORT_ASCENSION:
	case Arts::CUP_SUMMONS:
	case Arts::RALLY:
      if (!db_.Stats().IsKnight() && !db_.Stats().IsRuler()) {
	return false;
      }
      break;
    // must be a ruler in some house
	case Arts::EXPEL:
	case Arts::KNIGHT:
	case Arts::CREATE_ID_TOKEN:
      if (!db_.Stats().IsRuler()) {
	return false;
      }
      break;
    default:
      break;
  }
  // if we don't know it at all, then we can learn only if it's not Train (Regular players can only plateau Train, not initially teach it)
  if (my_skill == 0 && art != Arts::TRAIN) {
    return true;
  }
  // else, must be below next plateau in that art
  if ((my_skill % 10) != 9) {
    return false;
  }
  // ok
  return true;
}

bool GsPlayer::CanCompleteQuest(LmItem& item, GMsg_RcvGoalDetails& msg) const {

	quest_item_t candidate;
	memset(&candidate, 0, sizeof(quest_item_t));
	ItemToQuestItem(item, candidate);	

	if ((candidate.charges >= msg.Charges()) &&
		((candidate.color1 == msg.Color1()) ||
		(msg.Color1() == LmItemHdr::ANY_COLOR)) &&
		((candidate.color2 == msg.Color2()) ||
		(msg.Color2() == LmItemHdr::ANY_COLOR)) &&
		((candidate.graphic == msg.Graphic()) ||
#ifdef UL3D
		 (msg.Graphic() == 0))) //LyraBitmap::NONE))) 
#else
		 (msg.Graphic() == LyraBitmap::NONE))) 
#endif
	{	// don't check fields for any function quests
		if (msg.ItemType() == LyraItem::NO_FUNCTION)
			return true;
		else if ((candidate.field1 == msg.Field1()) &&
				(candidate.field2 == msg.Field2()) &&
				(candidate.field3 == msg.Field3()) &&
				(candidate.item_type == msg.ItemType()))
			return true;
	}
	
	return false;
}

// this is a helper function borrowed from the client
void GsPlayer::ItemToQuestItem(LmItem& item, quest_item_t& questitem) const
{
	const void* state = item.StateField(0);
	int item_function = (*((unsigned char*)state));

	questitem.charges = item.Charges();
	questitem.color1 = item.Color1();
	questitem.color2 = item.Color2();
	questitem.graphic = item.Header().Graphic();
	questitem.item_type = item_function;

	switch (item_function)
	{
		case LyraItem::CHANGE_STAT_FUNCTION:
		{
			lyra_item_change_stat_t changestat;
			memcpy(&changestat, state, sizeof(changestat));
			questitem.field1 = changestat.stat;
			questitem.field2 = changestat.modifier;
			break;
		}
		case LyraItem::MISSILE_FUNCTION:
		{
			lyra_item_missile_t missile;
			memcpy(&missile, state, sizeof(missile));
			questitem.field1 = missile.velocity;
			questitem.field2 = missile.effect;
			questitem.field3 = missile.damage;
			break;
		}
		case LyraItem::EFFECT_PLAYER_FUNCTION:
		{
			lyra_item_effect_player_t effectplayer;
			memcpy(&effectplayer, state, sizeof(effectplayer));
			questitem.field1 = effectplayer.effect;
			questitem.field2 = effectplayer.duration;
			break;
		}

		case LyraItem::ARMOR_FUNCTION:
		{
			lyra_item_armor_t armor;
			memcpy(&armor, state, sizeof(armor));
			questitem.field1 = armor.curr_durability;
			questitem.field2 = armor.max_durability;
			questitem.field3 = armor.absorption;
			break;
		}

		case LyraItem::SCROLL_FUNCTION:
		{
			TCHAR buffer[Lyra::MAX_ITEMDESC];
			lyra_item_scroll_t scroll;
			memcpy(&scroll, state, sizeof(scroll));
			questitem.field1 = scroll.targetid();
			questitem.field2 = scroll.creatorid();
			break;
		}
	}
	return;
}
