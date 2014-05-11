// LmInventory.h  -*- C++ -*-
// $Id: LmInventory.h,v 1.12 1997-10-30 12:38:51-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player inventory

#ifndef INCLUDED_LmInventory
#define INCLUDED_LmInventory

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmItem.h"

// forward declarations

// base structure

// the class

class LmInventory {

public:

  enum {
    MAX_WEAPONS = Lyra::INVENTORY_MAX
  };

  static const LmInventory DEFAULT_INSTANCE;

public:

  LmInventory();

  // selectors
  int NumItems() const;
  const LmItem& Item(const LmItemHdr& itemhdr);
  bool HasItem(const LmItemHdr& itemhdr) const;
  bool IsFull() const;
  bool IsEmpty() const;
  bool CanAddItem(const LmItemHdr& itemhdr) const;
  int ItemX(int itemnum) const;
  bool HasWeapon(int bitmap, int damage, int velocity, int effect) const;
  const LmItem& ItemByIndex(int itemnum) const;

  // mutable selectors
  LmItem& ItemByIndex(int itemnum);

  // mutators
  int AddItem(const LmItem& item);
  int RemoveItem(const LmItemHdr& itemhdr);
  void SetItemX(int serial, int item_x);
  void RemoveAll();
  void UpdateItemState(const LmItem& item);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmInventory(const LmInventory&);
  //operator=(const LmInventory&);

  int index_of(const LmItemHdr& itemhdr);
  const int index_of(const LmItemHdr& itemhdr) const;
  void add_weapon(const LmItem& item);
  void remove_weapon(const LmItem& item);

  struct iteminfo_t {
    LmItem item;  // the item
    int item_x;   // its position/flags (16 significant bits)
  };

  int num_items_;  // number of items
  iteminfo_t items_[Lyra::INVENTORY_MAX];

  int num_weapons_; // number of missile weapons
  lyra_item_missile_t weapons_[MAX_WEAPONS];

};

#ifdef USE_INLINE
#include "LmInventory.i"
#endif

#endif /* INCLUDED_LmInventory */
