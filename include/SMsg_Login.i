// SMsg_Login.i  -*- C++ -*-
// $Id: SMsg_Login.i,v 1.3 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_Login::ServerType() const
{
  return data_.servertype;
}

INLINE lyra_id_t SMsg_Login::ID() const
{
  return data_.id;
}

INLINE void SMsg_Login::SetServerType(int servertype)
{
  data_.servertype = servertype;
}

INLINE void SMsg_Login::SetID(lyra_id_t id)
{
  data_.id = id;
}
