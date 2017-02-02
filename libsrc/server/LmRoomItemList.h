// LmRoomItemList.h  -*- C++ -*-
// $Id: LmRoomItemList.h,v 1.1 1997-09-04 02:29:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// list of room items

#ifndef INCLUDED_LmRoomItemList
#define INCLUDED_LmRoomItemList

#ifdef __GNUC__
#pragma interface
#endif

#include <list>

#include "LmRoomItem.h"

class LmRoomItemList : public std::list<LmRoomItem> {
  // empty
};

#endif /* INCLUDED_LmRoomItemList */




