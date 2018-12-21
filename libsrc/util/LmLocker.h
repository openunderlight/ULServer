// LmLocker.h  -*- C++ -*-
// $Id: LmLocker.h,v 1.5 1997-07-30 16:45:48-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// object-locking class

#ifndef INCLUDED_LmLocker
#define INCLUDED_LmLocker

#ifdef __GNUC__
#pragma interface
#endif

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/pth/PThMutex.h"

// the class

class LmLocker {

public:

  LmLocker(PThMutex& m);
  LmLocker(const PThMutex& m);
  ~LmLocker();

  void UnLock(); // explicitly unlock mutex

private:

  PThMutex& m_;
  bool locked_;

};

#ifdef USE_INLINE
#include "LmLocker.i"
#endif

#endif /* INCLUDED_LmLocker */
