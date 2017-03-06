// SMsg_RotateLogs.i  -*- C++ -*-
// $Id: SMsg_RotateLogs.i,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_RotateLogs::RotateLogsType() const
{
  return data_.type;
}

INLINE void SMsg_RotateLogs::SetRotateLogsType(int type)
{
  data_.type = type;
}
