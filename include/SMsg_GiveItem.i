// SMsg_GiveItem.i  -*- C++ -*-
// $Id: SMsg_GiveItem.i,v 1.1 1997-09-18 16:49:05-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_GiveItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_GiveItem::SourceID() const
{
  return data_.sourceid;
}

INLINE lyra_id_t SMsg_GiveItem::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItem& SMsg_GiveItem::Item() const
{
  return data_.item;
}

INLINE void SMsg_GiveItem::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void SMsg_GiveItem::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void SMsg_GiveItem::SetItem(const LmItem& item)
{
  data_.item = item;
}
