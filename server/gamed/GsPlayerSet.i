// GsPlayerSet.i  -*- C++ -*-
// $Id: GsPlayerSet.i,v 1.3 1997-09-09 02:58:33-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE bool GsPlayerSet::IsInGame(lyra_id_t playerid) const
{
  return (GetPlayer(playerid) != 0);
}

INLINE int GsPlayerSet::MaxPlayers() const
{
  return max_players_;
}

INLINE int GsPlayerSet::NumLogins() const
{
  return total_logins_;
}
