// LmConnectionList.h  -*- C++ -*-
// $Id: LmConnectionList.h,v 1.1 1997-07-08 13:58:37-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// a list of pointers to connections

#ifndef INCLUDED_LmConnectionList
#define INCLUDED_LmConnectionList

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/types.h>

#include "../../libsrc/shared/LyraDefs.h"

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <pth.h>
#include <list>


// forward declarations

class LmConnection;

// class declarations

class LmConnectionList : public std::list<LmConnection*> {
  // no extra members
};

#endif /* INCLUDED_LmConnectionList */
