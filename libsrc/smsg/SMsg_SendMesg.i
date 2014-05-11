// SMsg_SendMesg.i  -*- C++ -*-
// $Id: SMsg_SendMesg.i,v 1.1 1997-08-17 16:32:02-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE LmConnection* SMsg_SendMesg::Connection() const
{
  return data_.conn;
}

INLINE LmSrvMesgBuf* SMsg_SendMesg::MesgBuf() const
{
  return data_.mbuf;
}

INLINE void SMsg_SendMesg::SetConnection(LmConnection* conn)
{
  data_.conn = conn;
}

INLINE void SMsg_SendMesg::SetMesgBuf(LmSrvMesgBuf* mbuf)
{
  data_.mbuf = mbuf;
}
