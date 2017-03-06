// LmDatabaseKey.i  -*- C++ -*-
// $Id: LmDatabaseKey.i,v 1.2 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE const TCHAR* LmDatabaseKey::Type() const
{
  return type_;
}

INLINE const TCHAR* LmDatabaseKey::Field() const
{
  return field_;
}

INLINE int LmDatabaseKey::Instance() const
{
  return instance_;
}

INLINE int LmDatabaseKey::SubInstance() const
{
  return subinstance_;
}

INLINE void LmDatabaseKey::SetInstance(int instance)
{
  instance_ = instance;
}

INLINE void LmDatabaseKey::SetSubInstance(int subinstance)
{
  subinstance_ = subinstance;
}
