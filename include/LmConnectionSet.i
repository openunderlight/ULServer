// LmConnectionSet.i  -*- C++ -*-
// $Id: LmConnectionSet.i,v 1.3 1997-09-09 02:52:56-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE int LmConnectionSet::MaxConnections() const
{
  return max_conns_;
}
