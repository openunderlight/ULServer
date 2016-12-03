// LmSockType.i  -*- C++ -*-
// $Id: LmSockType.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE int LmSockType::Family() const
{
  return family_;
}

INLINE int LmSockType::Type() const
{
  return type_;
}

INLINE int LmSockType::Protocol() const
{
  return protocol_;
}
