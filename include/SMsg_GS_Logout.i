// SMsg_GS_Logout.i  -*- C++ -*-
// $Id: SMsg_GS_Logout.i,v 1.5 1997-10-03 18:30:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE lyra_id_t SMsg_GS_Logout::PlayerID() const
{
  return data_.playerid;
}

INLINE int SMsg_GS_Logout::Online() const
{
  return data_.online;
}

INLINE int SMsg_GS_Logout::Status() const
{
  return data_.status;
}

INLINE void SMsg_GS_Logout::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_GS_Logout::SetOnline(int online)
{
  data_.online = online;
}

INLINE void SMsg_GS_Logout::SetStatus(int status)
{
  data_.status = status;
}
