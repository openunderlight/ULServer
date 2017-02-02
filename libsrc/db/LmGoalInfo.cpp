// LmGoalInfo.cpp  -*- C++ -*-
// $Id: LmGoalInfo.cpp,v 1.2 1997-11-17 14:04:43-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <string.h>

#include "LmGoalInfo.h"

#ifndef USE_INLINE
#include "LmGoalInfo.i"
#endif

////
// Constructor
////

LmGoalInfo::LmGoalInfo()
{
  Init();
}

////
// Init - reset
////

void LmGoalInfo::Init()
{
  // zero out
  memset(&gi_, 0, sizeof(gi_));
}

////
// Dump
////

void LmGoalInfo::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmGoalInfo[%p,%d]: "), this, sizeof(LmGoalInfo));
  Dump1(f);
 _ftprintf(f, _T(">\n"));
}

////
// Dump1
////

void LmGoalInfo::Dump1(FILE* f) const
{
 _ftprintf(f, _T("[id=%u,creator=%u,guild=%d,rank=%d,accepts=%d/%d,expire=%d,votes=%d:%d,vexpire=%d,status=%d,flags=%d]"),
	  GoalID(), CreatorID(), Guild(), Rank(), NumAccepted(), MaxAccepts(),
	  ExpirationTime(), NumberYes(), NumberNo(), VoteExpiration(), 
	  Status(), Flags());
}
