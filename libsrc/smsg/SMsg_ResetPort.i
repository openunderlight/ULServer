// SMsg_ResetPort.i  -*- C++ -*-
// $Id: SMsg_ResetPort.i,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions


INLINE lyra_id_t SMsg_ResetPort::PlayerID() const
{
  return data_.playerid;
}

INLINE int SMsg_ResetPort::Port() const
{
  return data_.port;
}

INLINE void SMsg_ResetPort::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_ResetPort::SetPort(int port)
{
  data_.port = port;
}
