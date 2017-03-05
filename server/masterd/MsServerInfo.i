// MsServerInfo.i  -*- C++ -*-
// $Id: MsServerInfo.i,v 1.1 1997-07-09 18:24:50-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE int MsServerInfo::NumServers() const
{
  return children_.size();
}
