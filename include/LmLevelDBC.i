// LmLevelDBC.i  -*- C++ -*-
// $Id: LmLevelDBC.i,v 1.3 1997/10/30 20:38:19 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE long LmLevelDBC::LastSQLCode() const
{
  return last_sql_code_;
}

INLINE void LmLevelDBC::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE LmLog* LmLevelDBC::Log() const
{
  return log_;
}


INLINE long LmLevelDBC::LastCallTime() const
{
  return last_ms_;
}

INLINE long LmLevelDBC::TotalCallTime() const
{
  return num_ms_;
}

INLINE long LmLevelDBC::TotalSQLTime() const
{
  return sql_ms_;
}

INLINE lyra_id_t LmLevelDBC::LevelID() const
{
  return level_id_;
}

INLINE const TCHAR* LmLevelDBC::LevelName() const
{
  return level_name_;
}

INLINE int LmLevelDBC::NumRooms() const
{
  return num_rooms_;
}

INLINE int LmLevelDBC::MaxPlayers() const
{
  return max_players_;
}


