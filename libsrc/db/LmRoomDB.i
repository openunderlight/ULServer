// LmRoomDB.i  -*- C++ -*-
// $Id: LmRoomDB.i,v 1.6 1997-11-04 23:02:47-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE lyra_id_t LmRoomDB::RoomID() const
{
  return roomid_;
}

INLINE const TCHAR* LmRoomDB::RoomName() const
{
  return roomname_;
}

INLINE bool LmRoomDB::EntryArea() const
{
  return (bool)entry_;
}

INLINE int LmRoomDB::MaxPlayers() const
{
  return maxplayers_;
}

INLINE int LmRoomDB::NumGenerators() const
{
  return num_generators_;
}

INLINE const LmItemGenDB& LmRoomDB::Generator(int index) const
{
  return generators_[index];
}

INLINE int LmRoomDB::NumPortals() const
{
  return num_portals_;
}

INLINE lyra_id_t LmRoomDB::PortalLevelID(int index) const
{
  return portals_[index].levelid;
}

INLINE lyra_id_t LmRoomDB::PortalRoomID(int index) const
{
  return portals_[index].roomid;
}

INLINE bool LmRoomDB::NoReapItems() const
{
  return ((bool)no_reap_items_);
}

INLINE const TCHAR* LmRoomDB::RoomDescription() const
{
  return description_;
} 


