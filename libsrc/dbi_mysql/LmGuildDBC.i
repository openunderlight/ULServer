// LmGuildDBC.i  -*- C++ -*-
// $Id: LmGuildDBC.i,v 1.4 1997-10-30 12:38:19-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE long LmGuildDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmGuildDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmGuildDBC::Log() const
{
  return log_;
}

INLINE int LmGuildDBC::NumCalls() const
{
  return num_calls_;
}

INLINE long LmGuildDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmGuildDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmGuildDBC::TotalSQLTime() const
{
  return sql_ms_;
}
