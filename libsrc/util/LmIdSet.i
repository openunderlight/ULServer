// LmIdSet.i  -*- C++ -*-
// $Id: LmIdSet.i,v 1.3 1997-08-05 14:58:03-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE int LmIdSet::Size() const
{
  return num_ids_;
}

INLINE int LmIdSet::MaxSize() const
{
  return max_ids_;
}

INLINE bool LmIdSet::IsFull() const
{
  return (Size() == MaxSize());
}

INLINE bool LmIdSet::IsEmpty() const
{
  return (num_ids_ == 0);
}

INLINE bool LmIdSet::HasMember(lyra_id_t id) const
{
  return (index_of(id) != -1);
}
