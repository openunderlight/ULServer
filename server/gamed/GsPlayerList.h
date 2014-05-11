// GsPlayerList.h  -*- C++ -*-
// $Id: GsPlayerList.h,v 1.1 1997-07-28 16:11:55-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// list of players

#ifndef INCLUDED_GsPlayerList
#define INCLUDED_GsPlayerList

#ifdef __GNUC__
#pragma interface
#endif

#include <list>

// forward declarations

class GsPlayer;

// the list class

class GsPlayerList : public std::list<GsPlayer*> {
  // empty
};

#endif /* INCLUDED_GsPlayerList */
