// LmBillingDBC.i  -*- C++ -*-
// $Id: LmBillingDBC.i,v 1.3 1997/10/30 20:38:19 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE long LmBillingDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmBillingDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmBillingDBC::Log() const
{
  return log_;
}

INLINE long LmBillingDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmBillingDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmBillingDBC::TotalSQLTime() const
{
  return sql_ms_;
}




