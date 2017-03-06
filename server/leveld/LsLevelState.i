// LsLevelState.h  -*- C++ -*-
// $Id: LsLevelState.i,v 1.4 1997-09-26 14:59:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE LsLevelSerials& LsLevelState::Serials()
{
  // this object has its own lock, so this is safe to do
  return serials_;
}
