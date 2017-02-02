// SMsg_Error.i  -*- C++ -*-
// $Id: SMsg_Error.i,v 1.5 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_Error::MsgType() const
{
  return data_.msg_type;
}

INLINE int SMsg_Error::ErrorStringLength() const
{
  return data_.err_len;
}

INLINE const TCHAR* SMsg_Error::ErrorString() const
{
  return data_.err_msg;
}

INLINE void SMsg_Error::SetMsgType(int mesgtype)
{
  data_.msg_type = mesgtype;
}
