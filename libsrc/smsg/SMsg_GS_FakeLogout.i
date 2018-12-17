// SMsg_GS_FakeLogout.i  -*- C++ -*-
// $Id: SMsg_GS_FakeLogout.i,v 1.5 1997-10-03 18:30:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_GS_FakeLogout::PlayerID() const
{
  return data_.playerid;
}

INLINE int SMsg_GS_FakeLogout::OriginatingGamed() const
{
  return data_.gamed;
}
/*
INLINE int SMsg_GS_FakeLogout::Status() const
{
  return data_.status;
}
*/
INLINE void SMsg_GS_FakeLogout::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_GS_FakeLogout::SetOriginatingGamed(int gamed)
{
  data_.gamed = gamed;
}
