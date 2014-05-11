// LsItemGen.i  -*- C++ -*-
// $Id: LsItemGen.i,v 1.3 1997-07-09 18:24:38-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE const LmItemGenDB* LsItemGen::DB() const
{
  return db_;
}

INLINE bool LsItemGen::ReadyToGenerate() const
{
  return (time(NULL) > next_gen_);
}
