// GsPlayer.cpp  -*- C++ -*-
// $Id: GsPlayer1.cpp,v 1.20 1998/06/18 02:00:18 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// public methods dealing with items/inventory

#include <stdio.h>
#include <string.h>
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
#include "RMsg_PlayerUpdate.h"

////
// StartItemTake - add item to list of items being taken
////

void GsPlayer::StartItemTake(lyra_id_t playerid, const LmItem& item)
{
  LmLocker mon(lock_); // lock object during method duration
  take_t take;
  take.item = item;
  take.playerid = playerid;
  t_items_.push_back(take);
}

////
// EndItemTake - remove item from list of items being taken, and add to inventory
//   if item was taken
////

void GsPlayer::EndItemTake(const LmItemHdr& itemhdr, bool taken)
{
  LmLocker mon(lock_); // lock object during method duration
  // remove item from take list
  for (std::list<take_t>::iterator i = t_items_.begin(); !(bool)(i == t_items_.end()); ++i) {
    LmItem item = (*i).item;
    if (item.Header() == itemhdr) {
      t_items_.erase(i);
      if (taken) {
	// if taken, add to inventory
	db_.Inventory().AddItem(item);
      }
      break;
    }
  }
}

////
// ItemGiver - return playerid who is giving me the item
////

lyra_id_t GsPlayer::ItemGiver(const LmItemHdr& itemhdr) const
{
  LmLocker mon(lock_); // lock object during method duration
  for (std::list<take_t>::const_iterator i = t_items_.begin(); !(bool)(i == t_items_.end()); ++i) {
    LmItem item = (*i).item;
    if (item.Header() == itemhdr) {
      return (*i).playerid;
    }
  }
  return Lyra::ID_UNKNOWN;
}

////
// StartItemPickup - add item header to list of items being picked up
////

void GsPlayer::StartItemPickup(const LmItemHdr& itemhdr)
{
  LmLocker mon(lock_); // lock object during method duration
  // add item header to pickup list
  p_items_.push_back(itemhdr);
}

////
// EndItemPickup - remove item from list of items being picked up, and add to inventory
//   if item was picked up
////

void GsPlayer::EndItemPickup(const LmItem& item, bool retrieved)
{
  LmLocker mon(lock_); // lock object during method duration
  // remove item header from pickup list
  p_items_.remove(item.Header());
  // if item was retrieved, add to inventory
  if (retrieved) {
    db_.Inventory().AddItem(item);
  }
}

////
// StartItemDrop - remove item from inventory, and add to list of items being dropped
////

void GsPlayer::StartItemDrop(const LmItem& item)
{
  LmLocker mon(lock_); // lock object during method duration
  // remove item from inventory
  db_.Inventory().RemoveItem(item.Header());
  // add item to drop list
  d_items_.push_back(item);
}

////
// EndItemDrop - remove item from list of items being dropped; if item was not dropped,
//   then put back in inventory
////

void GsPlayer::EndItemDrop(const LmItemHdr& itemhdr, bool dropped)
{
  LmLocker mon(lock_); // lock object during method duration
  // remove item from drop list
  for (std::list<LmItem>::iterator i = d_items_.begin(); !(bool)(i == d_items_.end()); ++i) {
    LmItem item = *i;
    if (item.Header() == itemhdr) {
      d_items_.erase(i);
      if (!dropped) {
	// if not dropped, put back in inventory
	db_.Inventory().AddItem(item);
      }
      break;
    }
  }
}

void GsPlayer::UndoDrops() 
{

  // end all current drops and gives
	for (std::list<LmItem>::iterator i = d_items_.begin(); !(bool)(i == d_items_.end()); ++i) {
    LmItem item = *i;
    db_.Inventory().AddItem(item);
  }
  return;
}

////
// CanUpdateItem - return true if item is updatable (in inventory, mutable by player, ...)
////

bool GsPlayer::CanUpdateItem(const LmItem& item) const
{
  LmLocker mon(lock_); // lock object during method duration
  // admins do what they want
  if (db_.AccountType() == LmPlayerDB::ACCT_ADMIN) {
    return true;
  }
  // must have item
  if (!db_.Inventory().HasItem(item.Header())) {
    return false;
  }
  // item can't be immutable
  if (item.Header().FlagSet(LyraItem::FLAG_IMMUTABLE)) {
    return false;
  }
  // get "current" item from inventory
  LmItem citem = ((class LmInventory&)db_.Inventory()).Item(item.Header());
  // check number of functions are the same
  if (citem.NumFields() != item.NumFields()) {
    return false;
  }
  // check if individual state fields can be updated
  for (int i = 0; i < citem.NumFields(); ++i) {
    if (!can_update_state(citem.StateField(i), item.StateField(i))) {
      return false;
    }
  }
  // TODO: if FLAG_CHANGE_CHARGES, check that only charges changed? (not really necessary,
  // since the UpdateItem method only changes charges if this flag is set anyway)
  // if charges increased, check that player has RECHARGE art, and that increase was acceptable,
  // and that item is rechargable
  if (item.Charges() > citem.Charges()) {
    if (LyraItem::StateFunction(citem.StateField(0)) == LyraItem::LyraItem::SUPPORT_FUNCTION) {
      return true;
    }
    if (db_.Arts().Skill(Arts::RECHARGE_TALISMAN) < 1) {
      return false;
    }
    // scrolls cannot be recharged
    if (LyraItem::StateFunction(citem.StateField(0)) == LyraItem::SCROLL_FUNCTION) {
      return false;
    }
    int charge_diff = item.Charges() - citem.Charges();
    if (charge_diff > 100) {
      return false;
    }
  }
  // ok
  return true;
}

////
// can_update_state - return true if item state field can be updated from old to new (non-locking)
////

bool GsPlayer::can_update_state(const void* old_state, const void* new_state) const
{
  DEFMETHOD(GsPlayer, can_update_state);
  // functions must match
  if (LyraItem::StateFunction(old_state) != LyraItem::StateFunction(new_state)) {
    return false;
  }
  // get function, and size of state field
  int state_func = LyraItem::StateFunction(new_state);
  int state_size = LyraItem::FunctionSize(state_func);
  // check for state field not changing
  if (memcmp(old_state, new_state, state_size) == 0) {
    return true;
  }
  // state field changed somehow
  // check for state field being immutable, or only allowing charges to change
  // (immutable: WARD, AMULET, SUPPORT, SUPPORT_TRAIN; change_charges: CHANGE_STAT, MISSILE, EFFECT_PLAYER)
  if (LyraItem::FunctionImmutable(state_func) || LyraItem::FunctionChangeCharges(state_func)) {
    return false; // state was changed, not allowed
  }
  // functions that are not caught by above checks: ESSENCE, ARMOR, META_ESSENCE; handle these individually
  switch (state_func) {
  case LyraItem::ESSENCE_FUNCTION: {
    lyra_item_essence_t old_essence, new_essence;
    memcpy(&old_essence, old_state, sizeof(lyra_item_essence_t));
    memcpy(&new_essence, new_state, sizeof(lyra_item_essence_t));
    // mare_type, weapon_type, slaver_id cannot change
    if ((old_essence.mare_type != new_essence.mare_type) ||
	(old_essence.weapon_type != new_essence.weapon_type) ||
	(old_essence.slaver_id != new_essence.slaver_id)) {
      return false;
    }
    // strength can only decrease
    if (new_essence.strength > old_essence.strength) {
      return false;
    }
  }
  break;
  case LyraItem::ARMOR_FUNCTION: {
    lyra_item_armor_t old_armor, new_armor;
    memcpy(&old_armor, old_state, sizeof(lyra_item_armor_t));
    memcpy(&new_armor, new_state, sizeof(lyra_item_armor_t));
    // max_durability, absorption cannot change
    if ((old_armor.max_durability != new_armor.max_durability) ||
	(old_armor.absorption != new_armor.absorption)) {
      return false;
    }
    // curr_durability cannot exceed max_durability
    if (new_armor.curr_durability > new_armor.max_durability) {
      return false;
    }
    // curr_durability can only increase if REWEAVE art is known
    if ((new_armor.curr_durability > old_armor.curr_durability) &&
	(db_.Arts().Skill(Arts::REWEAVE) < 1)) {
      return false;
    }
  }
  break;
  case LyraItem::META_ESSENCE_FUNCTION: {
    lyra_item_meta_essence_t old_essence, new_essence;
    memcpy(&old_essence, old_state, sizeof(lyra_item_meta_essence_t));
    memcpy(&new_essence, new_state, sizeof(lyra_item_meta_essence_t));
    // guild_id cannot change
    if (old_essence.guild_id != new_essence.guild_id) {
      return false;
    }
    // num_mares cannot decrease
    if (new_essence.num_mares() < old_essence.num_mares()) {
      return false;
    }
    // not checked: strength (can decrease and increase)
	break;
  }
  case LyraItem::META_ESSENCE_NEXUS_FUNCTION: {
    lyra_item_meta_essence_nexus_t old_nexus, new_nexus;
    memcpy(&old_nexus, old_state, sizeof(lyra_item_meta_essence_nexus_t));
    memcpy(&new_nexus, new_state, sizeof(lyra_item_meta_essence_nexus_t));
    if (old_nexus.essence_cap != new_nexus.essence_cap || 
      old_nexus.strength_cap != new_nexus.strength_cap)
      return false;
    
    if (new_nexus.essences < old_nexus.essences)
      return false;
    
    break;
  }
  case LyraItem::GRATITUDE_FUNCTION: {
    lyra_item_gratitude_t gratitude;
    memcpy(&gratitude, old_state, sizeof(lyra_item_gratitude_t));
    // guild_id cannot change
    if (gratitude.creatorid() != this->PlayerID()) {
      return false;
    }
	break;
  }

  break;
  default:
    PDEBUG((_T("%s: item function %d encountered"), method, state_func));
    return false;
    break;
  }
  // passed all the checks
  return true;
}

////
// UpdateItem - update inventory item state
////

void GsPlayer::UpdateItem(const LmItem& item)
{
  LmLocker mon(lock_); // lock object during method duration
  // double-check that item is in inventory
  if (!db_.Inventory().HasItem(item.Header())) {
    return;
  }
  // get "actual" item
  LmItem it = db_.Inventory().Item(item.Header());
  // if header specifies only charges can be updated, do so
  if (it.Header().FlagSet(LyraItem::FLAG_CHANGE_CHARGES)) {
    it.SetCharges(item.Charges());
  }
  else { // update entire state
    it.SetState(item.State1(), item.State2(), item.State3());
  }
  db_.Inventory().UpdateItemState(it);
}

////
// SetItemX - change item's inventory "X" value
////

void GsPlayer::SetItemX(int serial, int item_x)
{
  LmLocker mon(lock_); // lock object during method duration
  db_.Inventory().SetItemX(serial, item_x);
}

////
// CanAddItem - return true if player can add given item to inventory
////

bool GsPlayer::CanAddItem(const LmItemHdr& itemhdr) const
{
  LmLocker mon(lock_); // lock object during method duration
#if 0
  // monsters can't pick up anything
  if (db_.AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return false;
  }
#endif
  // must have room in inventory (pending drops/pickups)
  if (potential_inventory_size() >= Lyra::INVENTORY_MAX) {
    return false;
  }
  // can't be in inventory
  if (db_.Inventory().HasItem(itemhdr)) {
    return false;
  }
  // ok
  return true;
}


////
// CanCreateItem - return true if player is allowed to create given item (does not check
//   if item can be added to inventory)
////

bool GsPlayer::CanCreateItem(const LmItem& item) const
{
  DEFMETHOD(GsPlayer, CanCreateItem);
  LmLocker mon(lock_); // lock object during method duration
  // created item's name must not be null
  if (_tcslen(item.Name()) == 0) {
    return false;
  }
  // gamemaster, monsters can create anything
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || (db_.AccountType() == LmPlayerDB::ACCT_MONSTER)) {
    return true;
  }

  // non-GM checks below
  // created item cannot be an artifact
  if (item.IsArtifact()) {
    return false;
  }
  // created item must be reapable by server
  if (item.FlagSet(LyraItem::FLAG_NOREAP)) {
    return false;
  }
  // created item can have a single function
  if (item.NumFields() > 1) {
    return false;
  }
  // get item function, state pointer
  const void* func_field = item.StateField(0);
  int func_type = LyraItem::StateFunction(func_field);
  // check for functions that the player can create
  if (!LyraItem::FunctionCreateByArt(func_type) && 
      !LyraItem::FunctionCreateByForge(func_type)) {
      return false;
  }

  if (this->PPEvoking()) { // we spent pp's to evoke an item creation art once
		return true;
  }


  // check function type
  switch (func_type) {
  case LyraItem::NOTHING_FUNCTION:
	   // anybody can create a do nothing
	  break;
  case LyraItem::WARD_FUNCTION: {
    // player must have WARD art
    if (db_.Arts().Skill(Arts::WARD) < 1) {
      return false;
    }
    lyra_item_ward_t ward;
    memcpy(&ward, func_field, sizeof(lyra_item_ward_t));
    // check creator field
    if (ward.player_id() != db_.PlayerID()) {
      return false;
    }
    // TODO: check strength?
  }  
  break;
  case LyraItem::META_ESSENCE_NEXUS_FUNCTION: {
    int nexusSkill = db_.Arts().Skill(Arts::CHAOS_WELL);
    if (nexusSkill < 1) 
      return false;
    lyra_item_meta_essence_nexus_t nexus;
    memcpy(&nexus, func_field, sizeof(lyra_item_meta_essence_nexus_t));
    if (nexus.essence_cap > 20*((nexusSkill/10)+1) ||
      nexus.strength_cap > 20*((nexusSkill/10)+1))
      return false;
    if (nexus.essences > 0 || nexus.strength > 0)
      return false;
  }
  break;
  case LyraItem::AMULET_FUNCTION: {
    // player must have AMULET art
    if (db_.Arts().Skill(Arts::AMULET) < 1) {
      return false;
    }
    lyra_item_amulet_t amulet;
    memcpy(&amulet, func_field, sizeof(lyra_item_amulet_t));
    // check creator field
    if (amulet.player_id != db_.PlayerID()) {
      return false;
    }
    // TODO: check strength?
  }
  case LyraItem::AREA_EFFECT_FUNCTION: {
     return true;		
  }
  break;
  case LyraItem::SUPPORT_FUNCTION: {
    lyra_item_support_t support;
    memcpy(&support, func_field, sizeof(lyra_item_support_t));
    // check creator id
    if (support.creator_id() != db_.PlayerID()) {
      return false;
    }
    // check support type
    if (support.token_type() == Tokens::DEMOTION) {
      // must have support_demotion
      if (db_.Arts().Skill(Arts::SUPPORT_DEMOTION) < 1) {
	return false;
      }
      // must have rank at least of knight in guild
      if (db_.Stats().GuildRank(support.guild_id()) < Guild::KNIGHT) {
	return false;
      }
    }
    else if (support.token_type() == Tokens::ASCENSION_TO_RULER) {
      // must have support_ascension
      if (db_.Arts().Skill(Arts::SUPPORT_ASCENSION) < 1) {
	return false;
      }
      // must be a ruler in guild
      if (db_.Stats().GuildRank(support.guild_id()) != Guild::RULER) {
	return false;
      }
    }
    else if (support.token_type() == Tokens::MEMBERSHIP) {
      // must have initiate
      if (db_.Arts().Skill(Arts::INITIATE) < 1) {
	return false;
      }
      // must be at least a knight in the guild
      if (db_.Stats().GuildRank(support.guild_id()) < Guild::KNIGHT) {
	return false;
      }
	}
	else if (support.token_type() == Tokens::POWER_TOKEN) {
      // must be at least an initiate in the guild
      if (db_.Stats().GuildRank(support.guild_id()) < Guild::INITIATE) {
	return false;
		}

	  // can use any House prime for Power Tokens
	   
	if (support.token_type() == Tokens::POWER_TOKEN) {
		bool prime = false;
		for (int i=0; i<NUM_GUILDS; i++) 
			if (FindPrime(i, Arts::POWER_TOKEN_DRAIN))
				prime = true;
		if (!prime)
			return false;
	}  
	  // must have proper House artifact
	  else if (!FindPrime(support.guild_id(), Arts::INITIATE_DRAIN)) {
		  return false;
	  }

	}
    else { // token type not known
      return false;
    }
  }
  break;
  case LyraItem::ESSENCE_FUNCTION: {
    // player must have BANISH_NIGHTMARE, ENSLAVE_NIGHTMARE, 
	// CLEANSE_NIGHTMARE, CORRUPT_ESSENCE, or SACRIFICE
    if ((db_.Arts().Skill(Arts::ENSLAVE_NIGHTMARE) < 1) &&
		(db_.Arts().Skill(Arts::BANISH_NIGHTMARE) < 1) &&
		(db_.Arts().Skill(Arts::CLEANSE_NIGHTMARE) < 1) &&
		(db_.Arts().Skill(Arts::CORRUPT_ESSENCE) < 1) &&
		(db_.Arts().Skill(Arts::SACRIFICE) < 1)) {
      return false;
    }
    lyra_item_essence_t essence;
    memcpy(&essence, func_field, sizeof(lyra_item_essence_t));
    // strength must be 0
    //if (essence.strength != 0) {
    //  return false;
    //}
    // slaver_id must be playerid
    //if (essence.slaver_id != db_.PlayerID()) {
    //  return false;
    //}
  }
  break;
  case LyraItem::SCROLL_FUNCTION: {
    // must have scroll art?
    if (db_.Arts().Skill(Arts::WRITE_SCROLL) < 1) {
      return false;
    }
    // item must have a description
    if (!item.FlagSet(LyraItem::FLAG_HASDESCRIPTION)) {
      return false;
    }
    lyra_item_scroll_t scroll;
    memcpy(&scroll, func_field, sizeof(scroll));
    // creatorid must be set
    if (scroll.creatorid() != db_.PlayerID()) {
      return false;
    }
  }

	  break;
  case LyraItem::CHANGE_STAT_FUNCTION: {
    // player must have FORGE_TALISMAN or COMBINE
    if ((db_.Arts().Skill(Arts::FORGE_TALISMAN) < 1) &&
		(db_.Arts().Skill(Arts::COMBINE) < 1))
	{
      return false;
    }
    // TODO: further checks?
  }
  break;
  case LyraItem::MISSILE_FUNCTION: {
	  // allow creation of vanity Freesoul Blade
	  if ((db_.Arts().Skill(Arts::FREESOUL_BLADE) > 0) ||
		  (db_.Arts().Skill(Arts::ILLUMINATED_BLADE) > 0))  {
	    lyra_item_missile_t missile;
		memcpy(&missile, func_field, sizeof(lyra_item_missile_t));
#ifdef UL3D
		if ((missile.bitmap_id == 0) && // LyraBitmap::DREAMBLADE_MISSILE) &&
#else
		if ((missile.bitmap_id == LyraBitmap::DREAMBLADE_MISSILE) &&
#endif			(missile.damage == 1) &&
			(missile.effect == 0) &&
			(missile.velocity == MELEE_VELOCITY))
			return true;
	  }

    // otherwise player must have FORGE_TALISMAN or COMBINE
    if ((db_.Arts().Skill(Arts::FORGE_TALISMAN) < 1) &&
		(db_.Arts().Skill(Arts::COMBINE) < 1))
	{
      return false;
    }
    // TODO: further checks?
  }
  break;
  case LyraItem::EFFECT_PLAYER_FUNCTION: {
    // player must have FORGE_TALISMAN or COMBINE
    if ((db_.Arts().Skill(Arts::FORGE_TALISMAN) < 1) &&
		(db_.Arts().Skill(Arts::COMBINE) < 1))
	{
      return false;
    }
    // TODO: further checks?
  }
  break;
  case LyraItem::ARMOR_FUNCTION: {
    // player must have FORGE_TALISMAN
    if (db_.Arts().Skill(Arts::FORGE_TALISMAN) < 1) {
      return false;
    }
    // TODO: further checks?
  }
  break;
  case LyraItem::SUPPORT_TRAIN_FUNCTION: {
    lyra_item_train_support_t support;
    memcpy(&support, func_field, sizeof(lyra_item_train_support_t));
    // check creator id
    // note: this check disabled due to lack of entire source ID
    //    if (support.creator_id() != db_.PlayerID()) {
    //return false;
    //}
    // check player has appropriate skill

    if (support.art_id == 255) { // sphere
      if (db_.Stats().Sphere() != support.art_level)
	  return false;
    } else if (db_.Arts().Skill(support.art_id) != support.art_level) {
	return false;
      }

    //    const char* target_name = main_->PlayerNameMap()->PlayerName(((lyra_id_t)support.target_id()));

    main_->Log()->Security(-6, _T("%s: player %s created Support Training token for player ID %u in art %d"), method, db_.PlayerName(), support.target_id(), support.art_id);

  }
  break;

  // not creatable by a player
  case LyraItem::COLLECTION_FUNCTION:
  case LyraItem::META_ESSENCE_FUNCTION:
  case LyraItem::GRATITUDE_FUNCTION: 
  case LyraItem::MAP_FUNCTION:            
  case LyraItem::TELEPORTER_FUNCTION:
    PDEBUG((_T("%s: illegal function type %d"), method, func_type));
    return false;
    break;
  default: // unknown
    PDEBUG((_T("%s: unknown function type %d"), method, func_type));
    return false;
    break;
  }
  // passed all tests
  return true;
}

////
// AddItem - add item to inventory, return 0 if ok, -1 if couldn't add
////

int GsPlayer::AddItem(const LmItem& item)
{
  LmLocker mon(lock_); // lock object during method duration
  return db_.Inventory().AddItem(item);
}

////
// RemoveItem - remove item from inventory, return 0 if ok, -1 if couldn't remove (not found)
////

int GsPlayer::RemoveItem(const LmItemHdr& itemhdr)
{
  LmLocker mon(lock_); // lock object during method duration
  return db_.Inventory().RemoveItem(itemhdr);
}

////
// CanDropItem - return true if player can drop given item
////

bool GsPlayer::CanDropItem(const LmItemHdr& itemhdr) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must be in inventory
  if (!db_.Inventory().HasItem(itemhdr)) {
    return false;
  }
  // ok
  return true;
}

////
// CanDestroyItem
////

bool GsPlayer::CanDestroyItem(const LmItemHdr& itemhdr) const
{
  LmLocker mon(lock_); // lock object during method duration
  // must be in inventory
  if (!db_.Inventory().HasItem(itemhdr)) {
    return false;
  }
  // GMs can destroy anything
  if (db_.AccountType() == LmPlayerDB::ACCT_ADMIN) {
    return true;
  }
  // can't be an artifact
  if (((class LmInventory&)db_.Inventory()).Item(itemhdr).IsArtifact()) {
    return false;
  }
  // ok
  return true;
}
