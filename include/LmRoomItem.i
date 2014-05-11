// LmRoomItem.i  -*- C++ -*-
// $Id: LmRoomItem.i,v 1.1 1997-09-04 02:29:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE const LmItem& LmRoomItem::Item() const
{
  return item_;
}

INLINE const LmPosition& LmRoomItem::Position() const
{
  return pos_;
}

INLINE time_t LmRoomItem::TimeToDie() const
{
  return timetodie_;
}

INLINE LmItem& LmRoomItem::Item()
{
  return item_;
}

INLINE LmPosition& LmRoomItem::Position()
{
  return pos_;
}

INLINE void LmRoomItem::SetLifetime(int lifetime)
{
  timetodie_ = time(NULL) + lifetime;
}
