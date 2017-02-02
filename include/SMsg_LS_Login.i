// SMsg_LS_Login.i  -*- C++ -*-
// $Id: SMsg_LS_Login.i,v 1.3 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_LS_Login::PlayerID() const
{
  return data_.playerid;
}

INLINE void SMsg_LS_Login::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
