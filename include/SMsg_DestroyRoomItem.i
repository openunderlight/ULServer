// SMsg_DestroyRoomItem.i  -*- C++ -*-
// $Id: SMsg_DestroyRoomItem.i,v 1.3 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_DestroyRoomItem::PlayerID() const
{
  return data_.playerid;
}

INLINE lyra_id_t SMsg_DestroyRoomItem::RoomID() const
{
  return data_.roomid;
}

INLINE const LmItemHdr& SMsg_DestroyRoomItem::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE void SMsg_DestroyRoomItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void SMsg_DestroyRoomItem::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_DestroyRoomItem::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}
