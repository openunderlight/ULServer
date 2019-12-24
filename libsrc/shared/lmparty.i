// LmParty.i  -*- C++ -*-
// $Id: LmParty.i,v 1.2 1997-11-17 14:05:12-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void LmParty::Dump(FILE*, int) const
{
  // empty
}

INLINE void LmParty::Dump1(FILE*) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE int LmParty::PartySize() const
{
  return num_members_;
}

INLINE int LmParty::MaxPartySize() const
{
  return MAX_PARTYSIZE;
}

INLINE bool LmParty::IsEmpty() const
{
  return (PartySize() == 0);
}

INLINE bool LmParty::IsFull() const
{
  return (PartySize() == MaxPartySize());
}

INLINE bool LmParty::HasPlayer(lyra_id_t playerid) const
{
  return (index_of(playerid) != -1);
}

INLINE lyra_id_t LmParty::LeaderID() const
{
  return leaderid_;
}

INLINE lyra_id_t LmParty::CreatorID() const
{
  return creatorid_;
}

INLINE lyra_id_t LmParty::PlayerID(int index) const
{
  return members_[index];
}
