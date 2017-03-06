// LmNetworkInput.i  -*- C++ -*-
// $Id: LmNetworkInput.i,v 1.6 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE LmDispatch* LmNetworkInput::Dispatch()
{
  return dispatch_;
}

INLINE LmConnectionSet* LmNetworkInput::ConnectionSet()
{
  return cset_;
}
