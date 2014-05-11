// SMsg_LocateAvatar.i  -*- C++ -*-
// $Id: SMsg_LocateAvatar.i,v 1.2 1997-10-02 14:19:09-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_LocateAvatar::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_LocateAvatar::LocatorID() const
{
  return data_.locatorid;
}

INLINE lyra_id_t SMsg_LocateAvatar::PlayerID() const
{
  return data_.playerid;
}

INLINE lyra_id_t SMsg_LocateAvatar::RoomID() const
{
  return data_.roomid;
}

INLINE void SMsg_LocateAvatar::SetLocatorID(lyra_id_t locatorid)
{
  data_.locatorid = locatorid;
}

INLINE void SMsg_LocateAvatar::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_LocateAvatar::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}
