// LmSrvMesgBuf.i  -*- C++ -*-
// $Id: LmSrvMesgBuf.i,v 1.3 1997-07-15 21:33:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// conditionally inlined methods/functions

INLINE int LmSrvMesgBuf::UseCount() const
{
  return use_count_;
}

INLINE void LmSrvMesgBuf::SetUseCount(int use)
{
  use_count_ = use;
  total_uses_ += use;
}

INLINE void LmSrvMesgBuf::AddReference()
{
  use_count_++;
  total_uses_++;
}

INLINE void LmSrvMesgBuf::RemoveReference()
{
  use_count_--;
}

INLINE int LmSrvMesgBuf::TotalUses() const
{
  return total_uses_;
}

INLINE time_t LmSrvMesgBuf::CreationTime() const
{
  return creation_time_;
}

INLINE time_t LmSrvMesgBuf::LastUseTime() const
{
  return last_use_;
}

INLINE void LmSrvMesgBuf::SetUseTime(time_t usetime)
{
  last_use_ = usetime;
}
