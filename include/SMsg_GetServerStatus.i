// SMsg_GetServerStatus.i  -*- C++ -*-
// $Id: SMsg_GetServerStatus.i,v 1.1 1997-09-09 01:55:51-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_GetServerStatus::Status() const
{
  return data_.status;
}

INLINE lyra_id_t SMsg_GetServerStatus::ID() const
{
  return data_.id;
}

INLINE void SMsg_GetServerStatus::SetStatus(int status)
{
  data_.status = status;
}

INLINE void SMsg_GetServerStatus::SetID(lyra_id_t id)
{
  data_.id = id;
}
