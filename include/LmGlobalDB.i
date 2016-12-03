// LmGlobalDB.i  -*- C++ -*-
// $Id: LmGlobalDB.i,v 1.3 1997-09-25 16:26:33-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE const TCHAR* LmGlobalDB::RootDir() const
{
  return rootdir_;
}

INLINE const TCHAR* LmGlobalDB::LogDir() const
{
  return logdir_;
}
