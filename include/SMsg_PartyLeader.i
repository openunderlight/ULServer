// SMsg_PartyLeader.i  -*- C++ -*-
// $Id: SMsg_PartyLeader.i,v 1.1 1997-11-10 18:34:50-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_PartyLeader::PlayerID() const
{
  return data_.playerid;
}

INLINE int SMsg_PartyLeader::LeaderTime() const
{
  return data_.leader_time;
}

INLINE void SMsg_PartyLeader::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_PartyLeader::SetLeaderTime(int leader_time)
{
  data_.leader_time = leader_time;
}
