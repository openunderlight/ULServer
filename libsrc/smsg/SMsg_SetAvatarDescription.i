// SMsg_SetAvatarDescription.i  -*- C++ -*-
// $Id: SMsg_SetAvatarDescription.i,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

#ifndef USE_DEBUG
INLINE void SMsg_SetAvatarDescription::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE lyra_id_t SMsg_SetAvatarDescription::PlayerID() const
{
  return data_.playerid;
}

INLINE const TCHAR* SMsg_SetAvatarDescription::Description() const
{
  return data_.description;
}

INLINE void SMsg_SetAvatarDescription::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}
