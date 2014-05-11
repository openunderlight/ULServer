// SMsg_GS_Action.i  -*- C++ -*-
// $Id: SMsg_GS_Action.i,v 1.3 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_GS_Action::Action() const
{
  return data_.action;
}

INLINE void SMsg_GS_Action::SetAction(int action)
{
  data_.action = action;
}
