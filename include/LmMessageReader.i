// LmMessageReader.i  -*- C++ -*-
// $Id: LmMessageReader.i,v 1.8 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE LmDispatch* LmMessageReader::Dispatch() const
{
  return dispatch_;
}
