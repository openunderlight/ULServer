// SMsg_ShowItem.i  -*- C++ -*-
// $Id: SMsg_ShowItem.i,v 1.1 1997-12-11 15:11:35-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_ShowItem::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_ShowItem::SourceID() const
{
  return data_.sourceid;
}

INLINE lyra_id_t SMsg_ShowItem::TargetID() const
{
  return data_.targetid;
}

INLINE const LmItemHdr& SMsg_ShowItem::ItemHeader() const
{
  return data_.itemheader;
}

INLINE const TCHAR* SMsg_ShowItem::ItemName() const
{
  return data_.itemname;
}

INLINE void SMsg_ShowItem::SetSourceID(lyra_id_t sourceid)
{
  data_.sourceid = sourceid;
}

INLINE void SMsg_ShowItem::SetTargetID(lyra_id_t targetid)
{
  data_.targetid = targetid;
}

INLINE void SMsg_ShowItem::SetItemHeader(const LmItemHdr& itemheader)
{
  data_.itemheader = itemheader;
}
