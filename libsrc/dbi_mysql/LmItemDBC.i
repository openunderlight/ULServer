// LmItemDBC.i  -*- C++ -*-
// $Id: LmItemDBC.i,v 1.3 1997/10/30 20:38:19 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE long LmItemDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmItemDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmItemDBC::Log() const
{
  return log_;
}


INLINE long LmItemDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmItemDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmItemDBC::TotalSQLTime() const
{
  return sql_ms_;
}
