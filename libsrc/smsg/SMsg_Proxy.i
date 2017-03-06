// SMsg_Proxy.i  -*- C++ -*-
// $Id: SMsg_Proxy.i,v 1.6 1997-08-04 18:39:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE void SMsg_Proxy::InitProcess(lyra_id_t playerid, LmMesg& msg)
{
  Init(playerid, PROXY_PROCESS, msg);
}

INLINE void SMsg_Proxy::InitProcess(lyra_id_t playerid, LmMesgBuf* msgbuf)
{
  Init(playerid, PROXY_PROCESS, msgbuf);
}

INLINE void SMsg_Proxy::InitForward(lyra_id_t playerid, LmMesg& msg)
{
  Init(playerid, PROXY_FORWARD, msg);
}

INLINE void SMsg_Proxy::InitForward(lyra_id_t playerid, LmMesgBuf* msgbuf)
{
  Init(playerid, PROXY_FORWARD, msgbuf);
}

INLINE lyra_id_t SMsg_Proxy::PlayerID() const
{
  return data_.playerid;
}

INLINE int SMsg_Proxy::ProxyType() const
{
  return data_.ptype;
}

INLINE int SMsg_Proxy::EnclosedMessageType() const
{
  return data_.mtype;
}

INLINE int SMsg_Proxy::EnclosedMessageSize() const
{
  return data_.msize;
}

INLINE const TCHAR* SMsg_Proxy::MessageBytes() const
{
  return data_.msgbytes;
}

INLINE void SMsg_Proxy::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_Proxy::SetProxyType(int proxy_type)
{
  data_.ptype = proxy_type;
}
