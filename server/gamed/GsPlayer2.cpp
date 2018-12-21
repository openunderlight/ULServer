// GsPlayer.cpp  -*- C++ -*-
// $Id: GsPlayer2.cpp,v 1.5 1997/11/21 00:45:53 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// public methods dealing with goals and reports

#include <stdio.h>

#include "GsPlayer.h"
#include "../../libsrc/pth/PThMutex.h"
#include "GsMain.h"
#include "LmPlayerDB.h"
#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSockType.h"
#include "LmSockAddrInet.h"
#include "LmLogFile.h"
#include "LmLevelDBC.h"
#include "LmGuildDBC.h"
#include "GMsg_All.h"
#include "LmLocker.h"
#include "LmGoalInfo.h"
#include "RMsg_PlayerUpdate.h"

////
// CanUpdateGoal - return true if player is allowed to update given goal
////

bool GsPlayer::CanUpdateGoal(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration

  // for quests, they just need to be a teacher or guardian
  if (goalinfo.Rank() == Guild::QUEST) {  
		bool can_modify = false;

		for (int guild = 0; guild < NUM_GUILDS; ++guild) 
			if (this->DB().Stats().GuildRank(guild) >= Guild::KNIGHT) 
				can_modify = true;

		if ((this->DB().Arts().Skill(Arts::TRAIN) > 0) || (this->DB().Arts().Skill(Arts::LEVELTRAIN) > 0)) 
			can_modify = true;

		return can_modify;
  }


  // can only update initiate missions
  if (goalinfo.Rank() != Guild::INITIATE) {
    return false;
  }
  // ok
  return true;
}

////
// CanAcceptGoal - return true if player is allowed to accept given goal
////

bool GsPlayer::CanAcceptGoal(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration
  // can't be the goal creator
  if (goalinfo.CreatorID() == db_.PlayerID()) {
    return false;
  }

  if (goalinfo.NumAccepted() >= goalinfo.MaxAccepts())
	  return false;
  
  if (goalinfo.Rank() == Guild::QUEST) {
	  // can't accept the same quest twice
	  if (main_->GuildDBC()->HasAcceptedQuest(db_.PlayerID(), goalinfo.GoalID()))
		  return false;
	  return true;
  }

  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have guild rank at least that of the goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) < goalinfo.Rank()) {
    return false;
  }

  // no checking: suggested sphere, focus stat
  // all conditions met
  return true;
}

////
// CanGetGoalText - return true if player is allowed to read goal text for given goal
////

bool GsPlayer::CanGetGoalText(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration

  if (goalinfo.Rank() == Guild::QUEST) {
	  return true;
  }
  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have guild rank at least that of the goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) < goalinfo.Rank()) {
    return false;
  }
  // no checking: suggested sphere, focus stat
  // all conditions met
  return true;
}

////
// CanGetGoalDetails - return true if player is allowed to read details for given goal
////

bool GsPlayer::CanGetGoalDetails(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration

  if (goalinfo.Rank() == Guild::QUEST) {
	  return true;
  }
  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have guild rank greater than that of the goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) <= goalinfo.Rank()) {
    return false;
  }
  // no checking: suggested sphere, focus stat
  // all conditions met
  return true;
}

////
// CanVoteOnGoal - return true if player is allowed to vote on given goal
////

bool GsPlayer::CanVoteOnGoal(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration
  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have be in the guild ruler council
  if (db_.Stats().GuildRank(goalinfo.Guild()) != Guild::RULER) {
    return false;
  }
  // no checking: suggested sphere, focus stat
  // all conditions met
  return true;
}

////
// CanGetGoalHeaders - return true if player is allowed to get goal headers for given guild/rank
////

bool GsPlayer::CanGetGoalHeaders(int guild, int rank) const
{
  LmLocker mon(lock_); // lock object during method duration

  if (rank == Guild::QUEST) {
	  return true;
  }

  // have to be in the guild
  if (!db_.Stats().IsInGuild(guild)) {
    return false;
  }
  // have to have guild rank at least that of the requested rank
  if (db_.Stats().GuildRank(guild) < rank) {
    return false;
  }
  // all conditions met
  return true;
}

////
// CanGetReportHeaders - return true if player is allowed to get report headers for given goal
////

bool GsPlayer::CanGetReportHeaders(const LmGoalInfo& goalinfo) const
{
  LmLocker mon(lock_); // lock object during method duration
  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have guild rank at least that of the goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) < goalinfo.Rank()) {
    return false;
  }
  // no checking: suggested sphere, focus stat
  // all conditions met
  return true;
}

////
// CanPostReport - return true if player is allowed to post a report re: the given goal
////

bool GsPlayer::CanPostReport(const LmGoalInfo& goalinfo, int awardxp, bool source_in_acceptees,
			     lyra_id_t targetid, bool target_in_acceptees) const
{
  LmLocker mon(lock_); // lock object during method duration

  // can't send yourself a report
  if (db_.PlayerID() == targetid) {
    return false;
  }
  // have to be in the guild associated with the goal
  if (!db_.Stats().IsInGuild(goalinfo.Guild())) {
    return false;
  }
  // have to have guild rank at least that of the goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) < goalinfo.Rank()) {
    return false;
  }
  // can't award negative xp
  if (awardxp < 0) {
    return false;
  }
  // if no xp awarded, that's all
  if (awardxp == 0) {
    return true;
  }

  // otherwise, xp was awarded

  // target cannot be goal creator
  if (goalinfo.CreatorID() == targetid) {
    return false;
  }
#if 0
  // player can't be in acceptees
  if (source_in_acceptees) {
    return false;
  }
  // target must be in acceptees
  if (!target_in_acceptees) {
    return false;
  }
#endif
  // if goal is not guardian-managed, must be creator
  if (!goalinfo.IsGuardianManaged() && (goalinfo.CreatorID() != db_.PlayerID())) {
    return false;
  }
  // rank must be greater than goal
  if (db_.Stats().GuildRank(goalinfo.Guild()) <= goalinfo.Rank()) {
    return false;
  }
  // must have enough xp in pool
  if (db_.Stats().PoolXP(goalinfo.Guild()) < awardxp) {
    return false;
  }
  // xp awarded can't exceed max
  if (awardxp > Lyra::MAX_AWARDXP) {
    return false;
  }
  // all conditions met
  return true;
}

////
// CanPostGoal - return true if player can post a goal for the given guild/rank
////

bool GsPlayer::CanPostGoal(int guild, int rank, int quest_xp) const
{
  LmLocker mon(lock_); // lock object during method duration

  if (db_.AccountType() == LmPlayerDB::ACCT_ADMIN)
	return true;

  // for quests, they just need to be a teacher or guardian
  if (rank == Guild::QUEST) { 
	  
		// must have enough xp in pool
		if (db_.Stats().QuestPoolXP() < quest_xp) 
			return false;

		bool can_post = false;

		for (int guild = 0; guild < NUM_GUILDS; ++guild) 
			if (this->DB().Stats().GuildRank(guild) >= Guild::KNIGHT) 
				can_post = true;

		if ((this->DB().Arts().Skill(Arts::TRAIN) > 0) || (this->DB().Arts().Skill(Arts::LEVELTRAIN) > 0)) 
			can_post = true;

		return can_post;
  }

  // have to be in the guild
  if (!db_.Stats().IsInGuild(guild)) {
    return false;
  }
  // have to have guild rank greater that of the requested rank
  if (db_.Stats().GuildRank(guild) <= rank) {
    return false;
  }
  // all conditions met
  return true;
}

////
// PostReport - remove award xp from xp pool
////

void GsPlayer::PostReport(int guild_num, int awardxp, lyra_id_t recipient)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Stats().UsePoolXP(guild_num, awardxp);
}

////
// PostQuest - remove award xp from xp pool
////

void GsPlayer::PostQuest(int quest_xp)
{
  LmLocker mon(lock_); // lock object during method duration
  if (db_.AccountType() != LmPlayerDB::ACCT_ADMIN)
	db_.Stats().UseQuestPoolXP(quest_xp);
}


////
// AcceptGoal - add goal to goalbok, return 0 if ok, -1 if not
////

int GsPlayer::AcceptGoal(lyra_id_t goalid)
{
  LmLocker mon(lock_); // lock object during method duration
  return db_.AcceptGoal(goalid);
}

////
// RemoveGoal - remove goal from goalbook, return 0 if ok, -1 if not
////

int GsPlayer::RemoveGoal(lyra_id_t goalid)
{
  LmLocker mon(lock_); // lock object during method duration
  return db_.RemoveGoal(goalid);
}
