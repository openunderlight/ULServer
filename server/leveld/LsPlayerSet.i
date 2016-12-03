// LsPlayerSet.i  -*- C++ -*-
// $Id: LsPlayerSet.i,v 1.4 1997-09-09 02:58:41-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE bool LsPlayerSet::IsInLevel(lyra_id_t playerid) const
{
  return (GetPlayer(playerid) != 0);
}

INLINE int LsPlayerSet::MaxPlayers() const
{
  return max_players_;
}

INLINE int LsPlayerSet::NumLogins() const
{
  return logins_;
}
