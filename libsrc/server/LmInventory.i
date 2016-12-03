// LmInventory.i  -*- C++ -*-
// $Id: LmInventory.i,v 1.10 1997-10-30 12:38:51-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmInventory::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int LmInventory::NumItems() const
{
  return num_items_;
}

INLINE const LmItem& LmInventory::Item(const LmItemHdr& itemhdr)
{
  return ItemByIndex(index_of(itemhdr));
}

INLINE const LmItem& LmInventory::ItemByIndex(int itemnum) const
{
  return INDEX_OK(itemnum, 0, Lyra::INVENTORY_MAX) ? items_[itemnum].item : LmItem::DEFAULT_INSTANCE;
}

INLINE LmItem& LmInventory::ItemByIndex(int itemnum) // non-const
{
  return INDEX_OK(itemnum, 0, Lyra::INVENTORY_MAX) ? (LmItem&)items_[itemnum].item : (LmItem&)LmItem::DEFAULT_INSTANCE;
}

INLINE bool LmInventory::HasItem(const LmItemHdr& itemhdr) const 
{
  return (index_of(itemhdr) != -1);
}

INLINE bool LmInventory::IsEmpty() const
{
  return (NumItems() == 0);
}

INLINE bool LmInventory::IsFull() const
{
  return (NumItems() == Lyra::INVENTORY_MAX);
}

INLINE int LmInventory::ItemX(int itemnum) const
{
  return INDEX_OK(itemnum, 0, Lyra::INVENTORY_MAX) ? items_[itemnum].item_x : 0;
}
