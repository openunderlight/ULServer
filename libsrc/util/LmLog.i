// LmLog.i  -*- C++ -*-
// $Id: LmLog.i,v 1.4 1997-10-16 18:14:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inlined methods

INLINE void LmLog::SetLogLevel(int level)
{
  log_level_ = level;
}

INLINE int LmLog::LogLevel() const
{
  return log_level_;
}

INLINE bool LmLog::LogOpened() const
{
  return log_opened_;
}

INLINE void LmLog::SetLogOpened(bool opened)
{
  log_opened_ = opened;
}

INLINE void LmLog::FlushLog()
{
  // empty
}
