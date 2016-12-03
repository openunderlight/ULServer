// SMsg_LS_Action.i  -*- C++ -*-
// $Id: SMsg_LS_Action.i,v 1.4 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE int SMsg_LS_Action::Action() const
{
  return data_.action;
}

INLINE void SMsg_LS_Action::SetAction(int action)
{
  data_.action = action;
}

INLINE int SMsg_LS_Action::Parameter() const
{
  return data_.parameter;
}

INLINE void SMsg_LS_Action::SetParameter(int parameter)
{
  data_.parameter = parameter;
}

