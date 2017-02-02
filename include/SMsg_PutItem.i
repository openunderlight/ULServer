// SMsg_PutItem.i  -*- C++ -*-
// $Id: SMsg_PutItem.i,v 1.4 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_PutItem::PlayerID() const
{
  return data_.playerid;
}

INLINE lyra_id_t SMsg_PutItem::RoomID() const
{
  return data_.roomid;
}

INLINE const LmItem& SMsg_PutItem::Item() const
{
  return data_.item;
}

INLINE const LmPosition& SMsg_PutItem::Position() const
{
  return data_.position;
}

INLINE int SMsg_PutItem::Lifetime() const
{
  return data_.lifetime;
}

INLINE void SMsg_PutItem::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_PutItem::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void SMsg_PutItem::SetItem(const LmItem& item)
{
  data_.item = item;
}

INLINE void SMsg_PutItem::SetPosition(const LmPosition& position)
{
  data_.position = position;
}

INLINE void SMsg_PutItem::SetLifetime(int lifetime)
{
  data_.lifetime = lifetime;
}
