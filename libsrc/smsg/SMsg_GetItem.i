// SMsg_GetItem.i  -*- C++ -*-
// $Id: SMsg_GetItem.i,v 1.5 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_GetItem::PlayerID() const
{
  return data_.playerid;
}

INLINE lyra_id_t SMsg_GetItem::RoomID() const
{
  return data_.roomid;
}

INLINE const LmItemHdr& SMsg_GetItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void SMsg_GetItem::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_GetItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void SMsg_GetItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
