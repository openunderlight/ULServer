// GsPlayer3.cpp  -*- C++ -*-
// $Id: GsPlayer3.cpp,v 1.12 1998/06/18 00:53:13 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// methods dealing with house ranks

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
#include "LmPlayerDBC.h"
#include "LmLocker.h"
#include "LmGoalInfo.h"
#include "RMsg_PlayerUpdate.h"
#include "LmUtil.h"


////
// ChangeGuildRank - change player's rank in given guild; return true if OK, false if not
////

bool GsPlayer::ChangeGuildRank(int guild_num, int rank)
{
  LmLocker mon(lock_); // lock object during method duration
  // TODO: check if this is really ok
  bool retval = db_.Stats().CanSetGuildRank(guild_num, rank);
  if (retval) {
    db_.Stats().SetGuildRank(guild_num, rank);
    main_->PlayerDBC()->SaveGuildRanks(db_.PlayerID(), db_.Stats());
  }

  return retval;
}

////
// FindPrime - find a prime for guild guild_id with at least min_charges in it to use to initiate
////

bool GsPlayer::FindPrime(int guild_id, int min_charges) const {

  for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
    const LmItem& item = db_.Inventory().ItemByIndex(i);
    // check item type; demote tokens only have a single function
    if (LyraItem::StateFunction(item.StateField(0)) == LyraItem::META_ESSENCE_FUNCTION) {
      lyra_item_meta_essence_t meta_essence; 
      const void* state;
      memcpy(&meta_essence, item.StateField(0), sizeof(meta_essence));
      if (meta_essence.guild_id != guild_id) {
	continue;
      }
      if (meta_essence.strength() >= min_charges) {
	return true;
      }
    }
  }
  return false;
}

////
// CanInitiate - return true if player can initiate someone else into given guild
////

bool GsPlayer::CanInitiate(int guild) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must have a rank of knight or higher in guild
  if (db_.Stats().GuildRank(guild) < Guild::KNIGHT) {
    return false;
  }
  // must have initiate art
  if (db_.Arts().Skill(Arts::INITIATE) < 0) {
    return false;
  }
  // must have prime for the house with enough charges
  if (!FindPrime(guild, Arts::INITIATE_DRAIN)) {
    return false;
  }
  // ok
  return true;
}

////
// CanBeInitiated - return true if player can be initiated into given guild
////

bool GsPlayer::CanBeInitiated(int guild) const
{
  LmLocker mon(lock_); // lock object during method duration
  // only regular players participate in the guild system
  if (db_.AccountType() != LmPlayerDB::ACCT_PLAYER) {
    return false;
  }
  // must not hold any rank in guild
  if (db_.Stats().GuildRank(guild) >= Guild::INITIATE) {
    return false;
  }
  // ok
  return true;
}

////
// Initiate - update player's initiator in given guild
////

void GsPlayer::Initiate(int guild_num, lyra_id_t initiator)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.SetInitiator(guild_num, initiator);
}

////
// CanKnight - return true if player can knight someone else into given guild
////

bool GsPlayer::CanKnight(int guild) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must have a rank of ruler
  if (db_.Stats().GuildRank(guild) != Guild::RULER) {
    return false;
  }
  // must have knight art
  if (db_.Arts().Skill(Arts::KNIGHT) < 0) {
    return false;
  }
  // must have prime for the house with enough charges
  if (!FindPrime(guild, Arts::GUARDIAN_DRAIN)) {
    return false;
  }
  // ok
  return true;
}

////
// CanBeKnighted - return true if player can be knighted into given guild
////

bool GsPlayer::CanBeKnighted(int guild) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must be an initiate in the guild
  if (db_.Stats().GuildRank(guild) != Guild::INITIATE) {
    return false;
  }
  // ok
  return true;
}

////
// CanDemote - return true if player can demote someone in given guild
////

bool GsPlayer::CanDemote(int guild, int num_tokens, lyra_id_t playerid) const
{
  DEFMETHOD(GsPlayer, CanDemote);
  LmLocker mon(lock_); // lock object during method duration
  // must be a ruler in guild
  if (db_.Stats().GuildRank(guild) != Guild::RULER) {
    return false;
  }
  // must have demote skill
  if (db_.Arts().Skill(Arts::DEMOTE) < 1) {
    return false;
  }
  // must have the given number of unique tokens to perform demotion of given player;
  // tokens must be created by other players
  bool has_membership_token = false;
  std::list<lyra_id_t> creators;
  for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
    const LmItem& item = db_.Inventory().ItemByIndex(i);
    // check item type; demote tokens only have a single function
    if (LyraItem::StateFunction(item.StateField(0)) == LyraItem::SUPPORT_FUNCTION) {
      lyra_item_support_t support;
      memcpy(&support, item.StateField(0), sizeof(support));
      if ((support.guild_id() == guild) &&
	  (support.creator_id() != db_.PlayerID()) &&
	  ((support.token_type() == Tokens::DEMOTION) || (support.token_type() == Tokens::MEMBERSHIP)) &&
	  (support.target_id() == playerid)) {
	creators.push_back(support.creator_id());
      }
      // even if player created membership token, count it
      if (support.token_type() == Tokens::MEMBERSHIP) {
	has_membership_token = true;
      }
    }
  }
  // should have the number of unique tokens given, or have the membership token
  if (has_membership_token) {
    return true;
  }
  creators.sort();
  creators.unique();
  if (creators.size() < num_tokens) {
    PDEBUG((_T("%s: not enough unique tokens; have %d, said %d"), method, creators.size(), num_tokens));
    return false;
  }
  // ok
  return true;
}

////
// CanBeDemoted - return true if player's rank in given guild can be demoted, if there
//   are given number of demotion tokens available
////

bool GsPlayer::CanBeDemoted(int guild, int num_tokens) const
{
  LmLocker mon(lock_); // lock object during method duration
  // can't be demoted if you aren't in the guild
  int rank = db_.Stats().GuildRank(guild);
  if (rank < Guild::INITIATE) {
    return false;
  }
  // get number of tokens required
  int num_needed = LmStats::TokensToDemote(rank);
  // do we have enough?
  if (num_needed > num_tokens) {
    return false;
  }
  // ok
  return true;
}

////
// TokensToDemote - return number of tokens needed to demote player in given guild
////

int GsPlayer::TokensToDemote(int guild) const
{
  LmLocker mon(lock_); // lock object during method duration
  return LmStats::TokensToDemote(db_.Stats().GuildRank(guild));
}

bool GsPlayer::HasMinRank(int rank) const
{
	LmLocker mon(lock_);
	for(int i = 0; i < NUM_GUILDS; i++)
	{
		if(db_.Stats().GuildRank(i) >= rank)
			return true;
	}	 
	
	return false;
}

void GsPlayer::remove_art(int art_id, GMsg_ChangeStat& changemsg)
{
	LmLocker mon(lock_);
	if (0 == db_.Arts().Skill(art_id))
		return;
	db_.Arts().SetSkill(art_id, 0);
	int i = changemsg.NumChanges();
	changemsg.InitChange(i, GMsg_ChangeStat::SET_SKILL, art_id, 0);
	changemsg.SetNumChanges(i + 1);
	// remove in the database; SavePlayer won't do this properly
	main_->PlayerDBC()->DeleteArt(db_.PlayerID(), art_id);
}

////
// Demote - reduce player's rank in guild to next lowest, return new rank; store tokens used
//   in reference passed
////


int GsPlayer::Demote(int guild_num, int& tokens_used, GMsg_ChangeStat& changemsg)
{
  LmLocker mon(lock_); // lock object during method duration
  int rank = db_.Stats().GuildRank(guild_num);
  int new_rank = rank - 1;
  tokens_used = LmStats::TokensToDemote(rank);
  db_.Stats().SetGuildRank(guild_num, new_rank);
  changemsg.Init(0);

  bool has_rank = false;
  bool has_ruler = false;
  bool has_knight = false;

  for (int i = 0; i < NUM_GUILDS; i++) {
    if (db_.Stats().GuildRank(i) > 0)
      has_rank = true;
    if (db_.Stats().GuildRank(i) >= Guild::KNIGHT)
      has_knight = true;
    if (db_.Stats().GuildRank(i) >= Guild::RULER)
      has_ruler = true;
	if (db_.Stats().GuildRank(i) <= 0){
		switch (i){ // Remove house-specific arts if no rank held
		case (Guild::MOON):
			{
			remove_art(108, changemsg); // Sable Shield
			break;
			}
		case (Guild::ECLIPSE):
			{
			remove_art(107, changemsg); // Peace Aura
			break;
			}
		case (Guild::SHADOW):
			{
			remove_art(110, changemsg); // Shadow Step
			break;
			}
		case (Guild::COVENANT):
			{
			remove_art(106, changemsg); // Break Covenant
			break;
			}
		case (Guild::RADIANCE):
			{
			remove_art(104, changemsg); // Radiant Blaze
			break;
			}
		case (Guild::CALENTURE):
			{
			remove_art(105, changemsg); // Poison Cloud
			break;
			}
		case (Guild::ENTRANCED):
			{
			remove_art(109, changemsg); // Entrancement
			break;
			}
		case (Guild::LIGHT):
			{
			remove_art(111, changemsg); // Dazzle
			break;
			}
		}
	}
  }

  if (!has_rank) { // no longer has any rank; remove house arts
    remove_art(Arts::DEMOTE, changemsg);	// Demote
    remove_art(Arts::POWER_TOKEN, changemsg); // Create Power Token
	remove_art(Arts::HOUSE_MEMBERS, changemsg); // House Members
	remove_art(Arts::SACRIFICE, changemsg); // Sacrifice
	remove_art(Arts::CORRUPT_ESSENCE, changemsg); // Corrupt Essence
  }

  if (!has_knight) {
    remove_art(Arts::INITIATE, changemsg); // initiate
	remove_art(Arts::SUPPORT_ASCENSION, changemsg); // support_ascend
    remove_art(Arts::ASCEND, changemsg); // ascend
	remove_art(Arts::SUPPORT_DEMOTION, changemsg); // support_demo
	remove_art(Arts::CUP_SUMMONS, changemsg); // cup_summons
  	remove_art(Arts::RALLY, changemsg); // Rally... seriously why the FUCK did you not use constants?
  }

  if (!has_ruler) {
    remove_art(Arts::KNIGHT, changemsg); // knight
    remove_art(Arts::EXPEL, changemsg); // expel
    remove_art(Arts::CREATE_ID_TOKEN, changemsg); // create id token
	remove_art(Arts::SUMMON_PRIME, changemsg); // summon_prime
  }

//  main_->PlayerDBC()->SavePlayer(db_, false);
  main_->PlayerDBC()->SavePlayer(db_, true);


  return new_rank;
}

////
// CanAscend - return true if player can ascend to rulership in given guild
////

bool GsPlayer::CanAscend(int guild) const
{
  DEFMETHOD(GsPlayer, CanAscend);
  LmLocker mon(lock_); // lock object during method duration
  // must be a knight in the guild
  if (db_.Stats().GuildRank(guild) != Guild::KNIGHT) {
    return false;
  }
  // must have ASCEND skill
  if (db_.Arts().Skill(Arts::ASCEND) < 1) {
    return false;
  }

  // must have prime for the house with enough charges
  if (!FindPrime(guild, Arts::RULER_DRAIN)) {
    return false;
  }

  // must have enough unique ascension tokens
  int have_tokens = 0;
  std::list<lyra_id_t> creators;
  for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
    const LmItem& item = db_.Inventory().ItemByIndex(i);
    // check item type; ascension tokens only have a single function
    if (LyraItem::StateFunction(item.StateField(0)) == LyraItem::SUPPORT_FUNCTION) {
      lyra_item_support_t support;
      memcpy(&support, item.StateField(0), sizeof(support));
      if ((support.guild_id() == guild) &&
	  (support.token_type() == Tokens::ASCENSION_TO_RULER) &&
	  (support.target_id() == db_.PlayerID()) &&
	  (support.creator_id() != db_.PlayerID())) { // can't create one for yourself
	have_tokens++;
	creators.push_back(support.creator_id());
      }
    }
  }
  // have enough tokens?
  if (have_tokens < Guild::DEMOTE_RULER) {
    return false;
  }
  // and, each one must come from a different person
  creators.sort();
  creators.unique();
  if (creators.size() < Guild::DEMOTE_RULER) {
    return false;
  }
  // ok
  return true;
}



////
// SphereTokens - returns the number of tokens the player has that can be used to sphere the target
////

int GsPlayer::SphereTokens(lyra_id_t target_id) const
{
  DEFMETHOD(GsPlayer, SphereTokens);
  LmLocker mon(lock_); // lock object during method duration

  //int min_support_sphere = 9; // the lowest sphere of those who created the tokens; deprecated
  int have_tokens = 0;
  std::list<lyra_id_t> creators;
  for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
    const LmItem& item = db_.Inventory().ItemByIndex(i);
    // check item type; ascension tokens only have a single function
    if (LyraItem::StateFunction(item.StateField(0)) == LyraItem::SUPPORT_TRAIN_FUNCTION) {
      lyra_item_train_support_t support;
      //      main_->Log()->Debug(_T("%s: found spheretoken!"), method);
      memcpy(&support, item.StateField(0), sizeof(support));
      if ((support.art_id == 255) && // 255 = Sphere
	  (support.target_id() == target_id)) {
	//	main_->Log()->Debug(_T("%s: correct kind of spheretoken!"), method);
	//if (support.art_level < min_support_sphere)
	  //min_support_sphere = support.art_level;
	creators.push_back(support.creator_id());
      }
    }
  }


  // and, each one must come from a different person
  creators.sort();
  creators.unique();

  have_tokens = creators.size();
  //  main_->Log()->Debug(_T("%s: after unique sort have %d spheretokens; min sphere = %d!"), method, have_tokens, min_support_sphere);

  // now figure out what sphere they qualify for
  // sphere tokens are no longer required to be from a certain level character
  if (have_tokens >= 12) // && (min_support_sphere >= 8))
    return 9;
  else if (have_tokens >= 10) //&& (min_support_sphere >= 7))
    return 8;
  else if (have_tokens >= 8) //&& (min_support_sphere >= 6))
    return 7;
  else if (have_tokens >= 6) //&& (min_support_sphere >= 5))
    return 6;
  else if (have_tokens >= 4) //&& (min_support_sphere >= 4))
    return 5;
  else if (have_tokens >= 2) //&& (min_support_sphere >= 3))
    return 4;
  else 
    return 3;

}

