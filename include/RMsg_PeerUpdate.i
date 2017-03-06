// RMsg_PeerUpdate.i  -*- C++ -*-
// $Id: RMsg_PeerUpdate.i,v 1.12 1998-04-16 19:03:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

#ifndef USE_DEBUG
INLINE void RMsg_PeerUpdate::Dump(FILE*, int) const
{
  // empty
}
#endif /* !USE_DEBUG */

INLINE const LmPeerUpdate& RMsg_PeerUpdate::Update() const
{
  return data_.update;
}

INLINE void RMsg_PeerUpdate::SetUpdate(const LmPeerUpdate& update)
{
  data_.update = update;
}
