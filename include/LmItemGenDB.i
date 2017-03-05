// LmItemGenDB.i  -*- C++ -*-
// $Id: LmItemGenDB.i,v 1.5 1997-09-09 02:52:42-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE const LmPosition& LmItemGenDB::Position() const
{
  return pos_;
}

INLINE int LmItemGenDB::GeneratorType() const
{
  return gen_type_;
}

INLINE int LmItemGenDB::BaseDelay() const
{
  return base_delay_;
}

INLINE int LmItemGenDB::VarDelay() const
{
  return var_delay_;
}

INLINE int LmItemGenDB::ItemTTL() const
{
  return item_ttl_;
}

INLINE int LmItemGenDB::VarTTL() const
{
  return var_ttl_;
}
