// LmInventory.cpp  -*- C++ -*-
// $Id: LmInventory.cpp,v 1.14 1997-10-30 12:38:51-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmInventory.h"
#endif

#ifdef WIN32
#define STRICT
#include "unix.h"
#include <winsock.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#include "LmInventory.h"
#include "LmItem.h"

#ifndef USE_INLINE
#include "LmInventory.i"
#endif

// default objects
const LmInventory LmInventory::DEFAULT_INSTANCE;

////
// Constructor
////

LmInventory::LmInventory()
{
  RemoveAll();
}

////
// CanAddItem: return true if item can be added, false otherwise
////

bool LmInventory::CanAddItem(const LmItemHdr& itemhdr) const
{
  if (IsFull() || HasItem(itemhdr)) {
    return false;
  }
  return true;
}
////
// AddItem: return 0 if successful, -1 otherwise
////

int LmInventory::AddItem(const LmItem& item)
{
  if (!CanAddItem(item.Header())) {
    return -1;
  }
  // add weapon info
  add_weapon(item);
  // add to end of array
  items_[num_items_].item = item;
  items_[num_items_].item_x = 0;
  num_items_++;
  return 0;
}

////
// RemoveItem: return 0 if successful, -1 otherwise
////

int LmInventory::RemoveItem(const LmItemHdr& itemhdr)
{
  int idx = index_of(itemhdr);
  if (idx == -1) {
    return -1;
  }
  // remove weapon info
  remove_weapon(items_[idx].item);
  // swap with tail
  num_items_--;
  items_[idx] = items_[num_items_];
  // clear tail
  items_[num_items_].item = LmItem::DEFAULT_INSTANCE;
  items_[num_items_].item_x = 0;
  return 0;
}



////
// RemoveAll
////

void LmInventory::RemoveAll()
{
  int i;
  num_items_ = 0;
  for (i = 0; i < Lyra::INVENTORY_MAX; ++i) {
    items_[i].item = LmItem::DEFAULT_INSTANCE;
    items_[i].item_x = 0;
  }
  num_weapons_ = 0;
  for (i = 0; i < MAX_WEAPONS; ++i) {
    weapons_[i].bitmap_id = 0;
    weapons_[i].velocity = 0;
    weapons_[i].effect = 0;
    weapons_[i].damage = 0;
  }
}

////
// UpdateItemState
////

void LmInventory::UpdateItemState(const LmItem& item)
{
  int idx = index_of(item.Header());
  if (idx != -1) {
    // update state fields
    items_[idx].item.SetState(item.State1(), item.State2(), item.State3());
  }
}

////
// SetItemX
////

void LmInventory::SetItemX(int serial, int item_x)
{
  // linear search for item with given serial number
  for (int i = 0; i < NumItems(); ++i) {
    if (ItemByIndex(i).Header().Serial() == serial) {
      // update position
      items_[i].item_x = item_x;
      return;
    }
  }
}

////
// HasWeapon
////

bool LmInventory::HasWeapon(int bitmap_id, int damage, int velocity, int effect) const
{
  // linear search
  for (int i = 0; i < num_weapons_; ++i) {
    if ((weapons_[i].bitmap_id == bitmap_id) &&
	(weapons_[i].damage == damage) &&
	// (weapons_[i].velocity == velocity) && // this doesn't work for some reason
	(weapons_[i].effect == effect)) {
      return true;
    }
  }
  // not found
  return false;
}

////
// Dump
////

#ifdef USE_DEBUG
void LmInventory::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmInventory[%p,%d]: items=%d weapons=%d>\n"), this, sizeof(LmInventory), NumItems(), num_weapons_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("x [hi,lo]: "));
  int i;
  for (i = 0; i < NumItems(); ++i) {
    int h = (ItemX(i) & 0xFF00) >> 8;
    int l = ItemX(i) & 0x00FF;
   _ftprintf(f, _T("[%x,%x] "), h, l);
  }
 _ftprintf(f, _T("\n"));
  INDENT(indent + 1, f);
 _ftprintf(f, _T("weapons [bm,vel,dmg,fx]: "));
  for (i = 0; i < num_weapons_; ++i) {
   _ftprintf(f, _T("[%d,%d,%d,%d] "), weapons_[i].bitmap_id, weapons_[i].velocity, weapons_[i].damage, weapons_[i].effect);
  }
 _ftprintf(f, _T("\n"));
  for (i = 0; i < NumItems(); ++i) {
    ItemByIndex(i).Dump(f, indent + 1);
  }
}
#endif /* USE_DEBUG */

////
// index_of: returns index of given item, or -1 if not found
////

const int LmInventory::index_of(const LmItemHdr& itemhdr) const
{
  // linear search
  for (int i = 0; i < NumItems(); ++i) {
    if (ItemByIndex(i).Header().Equals(itemhdr)) {
      return i;
    }
  }
  return -1;
}

int LmInventory::index_of(const LmItemHdr& itemhdr)
{
  // linear search
  for (int i = 0; i < NumItems(); ++i) {
    if (ItemByIndex(i).Header().Equals(itemhdr)) {
      return i;
    }
  }
  return -1;
}


////
// add_weapon
////

void LmInventory::add_weapon(const LmItem& item)
{
  if (num_weapons_ == MAX_WEAPONS) {
    // doh!
    return;
  }
  // check if item has a missile function; if so, add to the weapons array
  for (int j = 0; j < item.NumFields(); ++j) {
    if (LyraItem::StateFunction(item.StateField(j)) == LyraItem::MISSILE_FUNCTION) {
      lyra_item_missile_t missile;
      memcpy(&missile, item.StateField(j), sizeof(missile));
      // found it, add to array
      weapons_[num_weapons_] = missile;
      num_weapons_++;
      // done
      return;
    }
  }
}

////
// remove_weapon
////

void LmInventory::remove_weapon(const LmItem& item)
{
  if (num_weapons_ == 0) {
    return;
  }
  // check if item has a missile function
  for (int j = 0; j < item.NumFields(); ++j) {
    if (LyraItem::StateFunction(item.StateField(j)) == LyraItem::MISSILE_FUNCTION) {
      lyra_item_missile_t missile;
      memcpy(&missile, item.StateField(j), sizeof(missile));
      // found it; must find entry in array
      for (int i = 0; i < num_weapons_; ++i) {
	if ((missile.bitmap_id == weapons_[i].bitmap_id) && (missile.damage == weapons_[i].damage) &&
	    (missile.velocity == weapons_[i].velocity) && (missile.effect == weapons_[i].effect)) {
	  // found entry - switch with tail
	  num_weapons_--;
	  weapons_[i] = weapons_[num_weapons_];
	  // clear tail
	  weapons_[num_weapons_].bitmap_id = 0;
	  weapons_[num_weapons_].damage = 0;
	  weapons_[num_weapons_].velocity = 0;
	  weapons_[num_weapons_].effect = 0;
	  // done
	  return;
	}
      }
    }
  }
}
