// SMsg_GS_Login.i  -*- C++ -*-
// $Id: SMsg_GS_Login.i,v 1.1 1997-08-13 15:39:04-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_GS_Login::PlayerID() const
{
  return data_.playerid;
}

INLINE void SMsg_GS_Login::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
