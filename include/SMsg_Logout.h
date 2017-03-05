// SMsg_Logout.h  -*- C++ -*-
// $Id: SMsg_Logout.h,v 1.7 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server logout message

#ifndef INCLUDED_SMsg_Logout
#define INCLUDED_SMsg_Logout

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_Logout : public LmMesg {

public:

  SMsg_Logout();
  ~SMsg_Logout();

  void Init();

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int dummy;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_Logout.i"
#endif

#endif /* INCLUDED_SMsg_Logout */
