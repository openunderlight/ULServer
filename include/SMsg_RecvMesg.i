// SMsg_RecvMesg.i  -*- C++ -*-
// $Id: SMsg_RecvMesg.i,v 1.1 1997-08-12 14:38:54-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE LmConnection* SMsg_RecvMesg::Connection() const
{
  return data_.conn;
}

INLINE void SMsg_RecvMesg::SetConnection(LmConnection* conn)
{
  data_.conn = conn;
}
