// LsPlayerList.h  -*- C++ -*-
// $Id: LsPlayerList.h,v 1.1 1997-07-14 22:00:47-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// list of players

#ifndef INCLUDED_LsPlayerList
#define INCLUDED_LsPlayerList

#ifdef __GNUC__
#pragma interface
#endif

#include <list>

// forward declarations

class LsPlayer;

// the list class

class LsPlayerList : public std::list<LsPlayer*> {
  // empty
};

#endif /* INCLUDED_LsPlayerList */
