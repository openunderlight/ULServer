// RMsg_PeerUpdate.h  -*- C++ -*-
// $Id: RMsg_PeerUpdate.h,v 1.11 1997-11-06 18:21:32-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// LmPeerUpdate wrapper class - used for clients who can't use UDP

#ifndef INCLUDED_RMsg_PeerUpdate
#define INCLUDED_RMsg_PeerUpdate

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "RMsg.h"
#include "LmPeerUpdate.h"

// forward decls

// base datatype

// the class

class RMsg_PeerUpdate : public LmMesg {

public:

public:

  RMsg_PeerUpdate();

  void Init(const LmPeerUpdate& update);

  // selector/mutator for base type
  const LmPeerUpdate& Update() const;
  void SetUpdate(const LmPeerUpdate& update);

  void Dump(FILE* f, int indent = 0) const;

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    LmPeerUpdate update;
  } data_;
};

#ifdef USE_INLINE
#include "RMsg_PeerUpdate.i"
#endif

#endif /* INCLUDED_RMsg_PeerUpdate */
