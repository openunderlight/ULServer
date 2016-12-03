// LmMesgBufPoolImp.i  -*- C++ -*-
// $Id: LmMesgBufPoolImp.i,v 1.2 1997-07-08 13:56:54-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods

INLINE int LmMesgBufPoolImp::MaxSpareBuffers() const
{
  return max_sparebufs_;
}

INLINE int LmMesgBufPoolImp::NumSpareBuffers() const
{
  return spare_.size();
}

INLINE int LmMesgBufPoolImp::NumUsedBuffers() const
{
  return used_.size();
}
