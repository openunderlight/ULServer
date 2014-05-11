// SMsg_ItemPickup.i  -*- C++ -*-
// $Id: SMsg_ItemPickup.i,v 1.4 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_ItemPickup::PlayerID() const
{
  return data_.playerid;
}

INLINE const LmItem& SMsg_ItemPickup::Item() const
{
  return data_.item;
}

INLINE int SMsg_ItemPickup::Status() const
{
  return data_.status;
}

INLINE void SMsg_ItemPickup::SetItem(const LmItem& item)
{
  data_.item = item;
}

INLINE void SMsg_ItemPickup::SetStatus(int status)
{
  data_.status = status;
}

INLINE void SMsg_ItemPickup::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
