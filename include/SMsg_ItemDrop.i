// SMsg_ItemDrop.i  -*- C++ -*-
// $Id: SMsg_ItemDrop.i,v 1.5 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE const LmItemHdr& SMsg_ItemDrop::ItemHeader() const
{
  return data_.itemhdr;
}

INLINE int SMsg_ItemDrop::Status() const
{
  return data_.status;
}

INLINE lyra_id_t SMsg_ItemDrop::PlayerID() const
{
  return data_.playerid;
}

INLINE void SMsg_ItemDrop::SetItemHeader(const LmItemHdr& itemhdr)
{
  data_.itemhdr = itemhdr;
}

INLINE void SMsg_ItemDrop::SetStatus(int status)
{
  data_.status = status;
}

INLINE void SMsg_ItemDrop::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
