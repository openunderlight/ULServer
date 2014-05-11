// SMsg_LS_Action.h  -*- C++ -*-
// $Id: SMsg_LS_Action.h,v 1.18 1998-02-24 23:27:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server: do some action

#ifndef INCLUDED_SMsg_LS_Action
#define INCLUDED_SMsg_LS_Action

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_LS_Action : public LmMesg {

public:

  enum {
    // action codes
    ACTION_NONE               = 'U',

    ACTION_READCODEX          = 'c',
    ACTION_CHECK_IDLE_CLIENTS = 'C',
    ACTION_EXIT               = 'E',
    ACTION_FREEBUFS           = 'F',
    ACTION_GENERATE_ITEMS     = 'g',
    ACTION_COMPUTE_GROUPS     = 'G',
    ACTION_HEARTBEAT          = 'H',
    ACTION_CHECK_MSG_THREADS  = 'M',
    ACTION_PING               = 'p',
    ACTION_CHECK_IDLE_PLAYERS = 'P',
    ACTION_REAP_ITEMS         = 'r',
    ACTION_RESTART            = 'R',
    ACTION_SAVE_STATE_DB      = 's',
    ACTION_SAVE_STATE_FILE    = 'S'
  };

public:

  SMsg_LS_Action();
  ~SMsg_LS_Action();

  void Init(int action, short parameter);
  void Init(int action);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors/mutators
  int Action() const;
  void SetAction(int action);
  int Parameter() const;
  void SetParameter(int parameter);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    short action;
	short parameter;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_LS_Action.i"
#endif

#endif /* INCLUDED_SMsg_LS_Action */
