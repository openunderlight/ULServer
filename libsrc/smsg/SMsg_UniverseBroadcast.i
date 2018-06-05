// SMsg_UniverseBroadcast.i  -*- C++ -*-
// $Id: SMsg_UniverseBroadcast.i,v 1.6 1997-08-04 18:39:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_UniverseBroadcast::EnclosedMessageType() const
{
  return data_.mtype;
}

INLINE int SMsg_UniverseBroadcast::EnclosedMessageSize() const
{
  return data_.msize;
}

INLINE const TCHAR* SMsg_UniverseBroadcast::MessageBytes() const
{
  return data_.msgbytes;
}


