// SMsg_DS_Action.h  -*- C++ -*-
// $Id: SMsg_DS_Action.h,v 1.14 1998-03-11 15:30:39-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server: do some action

#ifndef INCLUDED_SMsg_DS_Action
#define INCLUDED_SMsg_DS_Action

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_DS_Action : public LmMesg {

public:

  enum {
    // action codes
    ACTION_NONE               = 'U',

    ACTION_EXIT               = 'E',
    ACTION_FREEBUFS           = 'F',
    ACTION_HEARTBEAT          = 'H',
    ACTION_PING               = 'p',
    ACTION_RESTART            = 'R',
  };

public:

  SMsg_DS_Action();
  ~SMsg_DS_Action();

  void Init(int action);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  int Action() const;
  void SetAction(int action);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int action;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_DS_Action.i"
#endif

#endif /* INCLUDED_SMsg_DS_Action */
