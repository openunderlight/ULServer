// LmPlayerDBC.i  -*- C++ -*-
// $Id: LmPlayerDBC.i,v 1.3 1997-10-30 12:38:19-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE long LmPlayerDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmPlayerDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmPlayerDBC::Log() const
{
  return log_;
}

INLINE int LmPlayerDBC::NumCalls() const
{
  return num_calls_;
}

INLINE long LmPlayerDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmPlayerDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmPlayerDBC::TotalSQLTime() const
{
  return sql_ms_;
}
