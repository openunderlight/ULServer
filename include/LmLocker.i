// LmLocker.i  -*- C++ -*-
// $Id: LmLocker.i,v 1.3 1997-07-30 16:45:48-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE LmLocker::LmLocker(PThMutex& m)
  : m_(m)
{
  m_.Lock();
  locked_ = true;
}

INLINE LmLocker::LmLocker(const PThMutex& m)
  : m_(const_cast<PThMutex&>(m)) // useful for when the mutex is used in const methods
{
  m_.Lock();
  locked_ = true;
}

INLINE void LmLocker::UnLock()
{
  if (locked_) {
    m_.UnLock();
    locked_ = false;
  }
}

INLINE LmLocker::~LmLocker()
{
  UnLock();
}
