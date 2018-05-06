// LmPlayerDB.cpp  -*- C++ -*-
// $Id: LmPlayerDB.cpp,v 1.25 1998/04/17 02:02:41 jason Exp jason $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmPlayerDB.h"
#endif

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LmPlayerDB.h"

#ifndef USE_INLINE
#include "LmPlayerDB.i"
#endif

// default objects
const TCHAR* LmPlayerDB::DEFAULT_PLAYERNAME = _T("");
const TCHAR* LmPlayerDB::DEFAULT_PASSWORD = _T("");

////
// Constructor
////

LmPlayerDB::LmPlayerDB()
  : goalbook_(Lyra::MAX_ACTIVE_GOALS)
{
  Init();
}

////
// Destructor
////

LmPlayerDB::~LmPlayerDB()
{
  // empty
}

////
// Init: initialize to unused default state
////

void LmPlayerDB::Init()
{
  playerid_ = Lyra::ID_UNKNOWN;
  billing_id_ = Lyra::ID_UNKNOWN;
 _tcscpy(playername_, DEFAULT_PLAYERNAME);
 _tcscpy(password_, DEFAULT_PASSWORD);
 _tcscpy(avatar_descrip_, _T("(none)"));
 _tcscpy(email_, _T("(unknown)"));
 _tcscpy(realname_, _T("(unknown)"));
  stats_.Init();
  avatar_.Init(0, 0);
  arts_.Init();
  goalbook_.Empty();
  inv_.RemoveAll();
  acct_type_ = ACCT_LOCKED;
  last_login_ = 0;
  time_online_ = 0;
  num_logins_ = 0;
  xp_gain_ = 0;
  xp_loss_ = 0;
  newly_alert_ = 0;
  x_ = y_ = 0;
  level_id_ = 0;
//  tcp_only_ = 0;
  for (int i = 0; i < NUM_GUILDS; ++i) {
    initiators_[i] = Lyra::ID_UNKNOWN;
  }
//  player_modified_ = false;
}

////
// PeggedBelowSphere: return true if player is 1 xp away from next sphere, ie. cannot 
//   earn any more xp
////

bool LmPlayerDB::PeggedBelowSphere() const
{
  bool retval = true;
  int xp = stats_.XP();
  int next_base = LmStats::SphereXPBase(stats_.Sphere() + 1);
  if (xp != (next_base - 1)) { // not 1 point under next sphere base
    retval = false;
  }
  return retval;
}

////
// CheckPassword: return true if password matches, false otherwise
////

bool LmPlayerDB::CheckPassword(const TCHAR* password) const
{
  return (_tcsncmp(password, password_, sizeof(password_)) == 0);
}

////
// CheckAvatar: check given avatar such that any house shields, sphere, or teacher indicators are valid;
//   return > 0 if error, 0 if not
////

int LmPlayerDB::CheckAvatar(const LmAvatar& avatar, int acct_type) const
{
  int retval = 0;
  if (avatar.AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) {
    return retval;
  }
  // check house shield
  if (avatar.ShowGuild() && (Stats().GuildRank(avatar.GuildID()) != avatar.GuildRank())) {
    retval = 1;
  }
  // check sphere
  if (avatar.ShowSphere() && (Stats().Sphere() != avatar.Sphere())) {
    retval = 2;
  }
  // check teacher
  if (avatar.Teacher() && (Arts().Skill(Arts::TRAIN) == 0)) {
    retval = 3;
  }

  // check teacher
  if (avatar.Teacher() && (Arts().Skill(Arts::TRAIN) == 0)) {
    retval = 3;
  }

  if (avatar.Dreamstrike() && (Arts().Skill(Arts::DREAMSTRIKE) == 0)) {
    retval = 4;
  }

  if (avatar.WordSmith() && (Arts().Skill(Arts::WORDSMITH_MARK) == 0)) {
    retval = 5;
  }

  if (avatar.DreamSmith() && (Arts().Skill(Arts::DREAMSMITH_MARK) == 0)) {
    retval = 6;
  }

  if (avatar.NPSymbol() && (Arts().Skill(Arts::NP_SYMBOL) == 0)) {
    retval = 7;
  }

  return retval;
}

////
// FixAvatar: check and optionally modify avatar such that any house shields, sphere, or teacher indicators are valid;
//   return > 0 if changes were made, 0 if not
////

int LmPlayerDB::FixAvatar(bool modify)
{
  int retval = 0;
  if (this->AccountType() == LmPlayerDB::ACCT_PMARE)
    {
      if (Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) 
	return 0;
      //      avatar_.SetAvatarType(Avatars::MIN_NIGHTMARE_TYPE);
      //      return 1;
  }
  else if (Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE)
    return retval;

  if (modify) {

	avatar_.SetFocus(this->Stats().FocusStat());
 	avatar_.SetPlayerInvis(0);
	if (this->Arts().Skill(Arts::DREAMSTRIKE) > 0 ) 
		avatar_.SetDreamstrike(1);
	else
		avatar_.SetDreamstrike(0);

	if (this->Arts().Skill(Arts::WORDSMITH_MARK) > 0)
		avatar_.SetWordSmith(1);
	else
		avatar_.SetWordSmith(0);

	if (this->Arts().Skill(Arts::DREAMSMITH_MARK) > 0)
		avatar_.SetDreamSmith(1);
	else
		avatar_.SetDreamSmith(0);

	if (this->Arts().Skill(Arts::NP_SYMBOL) > 0)
		avatar_.SetNPSymbol(1);
	else
		avatar_.SetNPSymbol(0);

  }

  
  // check house shield
  if (Avatar().ShowGuild() && (Stats().GuildRank(Avatar().GuildID()) != Avatar().GuildRank())) {
    // clear it
    if (modify) {
      avatar_.SetShowGuild(0);
      avatar_.SetGuildRank(Guild::NO_RANK);
      avatar_.SetGuildID(Guild::NO_GUILD);
    }
    retval = 2;
  }
  // check sphere
  if (Avatar().ShowSphere() && (Stats().Sphere() != Avatar().Sphere())) {
    if (modify) {
      avatar_.SetShowSphere(0);
      avatar_.SetSphere(Stats().Sphere());
    }
    retval = 3;
  }
  // check teacher
  if (Avatar().Teacher() && (Arts().Skill(Arts::TRAIN) == 0)) {
    if (modify) {
      avatar_.SetTeacher(0);
    }
    retval = 4;
  }
  // check master teacher
  if (Avatar().MasterTeacher() && (Arts().Skill(Arts::TRAIN_SELF) == 0)) {
    if (modify) {
      avatar_.SetMasterTeacher(0);
    }
    retval = 5;
  }

  // check GM invisibility, Show Lyran
  if (this->AccountType() != LmPlayerDB::ACCT_ADMIN)
  {
	  if ((avatar_.Hidden())  || (avatar_.ShowLyran())) {
		  if (modify) {
			avatar_.SetHidden(0);
			avatar_.SetShowLyran(0);
		  }
		retval = 6;
	  }
  }

  // GM Invis for dreamers only
  if ((Avatar().AvatarType() >= Avatars::MIN_NIGHTMARE_TYPE) &&
		Avatar().Hidden()) {
	  if (modify)
		  avatar_.SetHidden(0);
	retval = 7;
  }


  // GM Invis for %INVIS% real named accounts only
  if (avatar_.Hidden())
  {
	if ((NULL == _tcsstr(realname_, _T("INVIS"))) && 
		(NULL == _tcsstr(realname_, _T("invis"))) &&
		(NULL == _tcsstr(realname_, _T("Invis"))))
	{
		 if (modify)
			 avatar_.SetHidden(0);
		retval = 8;
	}
  }



  return retval;
}


////
// Dump
////

void LmPlayerDB::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmPlayerDB[%p,%d]: id=%u billing_id=%u name='%s' real='%s' email='%s'>\n"),
	  this, sizeof(LmPlayerDB),
	  PlayerID(), BillingID(), PlayerName(), RealName(), Email());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("logins=%d online=%d laston=%lu acct=%c xpgain=%d xploss=%d x=%d y=%d level_id=%d\n"),
	  NumLogins(), TimeOnline(), LastLogin(), AccountType(), XPGained(), XPLost(), X(), Y(), LevelID());
  avatar_.Dump(f, indent + 1);
  // the following items only matter for non-monsters
  if (AccountType() != ACCT_MONSTER) {
    INDENT(indent + 1, f);
   _ftprintf(f, _T("initiators: "));
    for (int i = 0; i < NUM_GUILDS; ++i) {
      if (Initiator(i) != Lyra::ID_UNKNOWN) {
_ftprintf(f, _T("%d:%u "), i, Initiator(i));
      }
    }
   _ftprintf(f, _T("\n"));
    INDENT(indent + 1, f);
   _ftprintf(f, _T("goalbook: "));
    goalbook_.Dump(f);
    stats_.Dump(f, indent + 1);
    arts_.Dump(f, indent + 1);
    inv_.Dump(f, indent + 1);
  }
}
