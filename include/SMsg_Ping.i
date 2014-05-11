// SMsg_Ping.i  -*- C++ -*-
// $Id: SMsg_Ping.i,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE void SMsg_Ping::InitPing(int nonce)
{
  Init(nonce, PING);
}

INLINE void SMsg_Ping::InitPong(int nonce)
{
  Init(nonce, PONG);
}

INLINE int SMsg_Ping::Nonce() const
{
  return data_.nonce;
}

INLINE int SMsg_Ping::PingType() const
{
  return data_.type;
}

INLINE void SMsg_Ping::SetNonce(int nonce)
{
  data_.nonce = nonce;
}

INLINE void SMsg_Ping::SetPingType(int ptype)
{
  data_.type = ptype;
}
