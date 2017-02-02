// LsRoomState.i  -*- C++ -*-
// $Id: LsRoomState.i,v 1.9 1997-09-16 03:03:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE const LmRoomDB* LsRoomState::DB() const
{
  return db_;
}

INLINE const std::list<lyra_id_t>& LsRoomState::PlayerList() const
{
  return players_;
}

INLINE const LmRoomItemList& LsRoomState::ItemList() const
{
  return items_;
}
