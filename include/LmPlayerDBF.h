// LmPlayerDBF.h  -*- C++ -*-
// $Id: LmPlayerDBF.h,v 1.3 1997-10-30 12:38:19-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// GDBM interface for LmPlayerDB

#ifndef INCLUDED_LmPlayerDBF
#define INCLUDED_LmPlayerDBF

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"

// forward declarations

class LmDatabase;
class LmPlayerDB;

// the class

class LmPlayerDBF {

public:

  static int LoadFromFile(LmDatabase& db, LmPlayerDB& player);
  static int SaveToFile(LmDatabase& db, const LmPlayerDB& player);

private:

  // not instantiable
  LmPlayerDBF();
  ~LmPlayerDBF();

};

#ifdef USE_INLINE
#include "LmPlayerDBF.i"
#endif

#endif /* INCLUDED_LmPlayerDBF */
