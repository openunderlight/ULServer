// SMsg_GetServerStatus.h  -*- C++ -*-
// $Id: SMsg_GetServerStatus.h,v 1.2 1998-01-22 17:15:39-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server/player status request

#ifndef INCLUDED_SMsg_GetServerStatus
#define INCLUDED_SMsg_GetServerStatus

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_GetServerStatus : public LmMesg {

public:

  enum {
    STATUS_CONN   = 'C',  // get connection status
    STATUS_PLAYER = 'P',  // get player status
    STATUS_SERVER = 'S'   // get server status
  };

public:

  SMsg_GetServerStatus();
  ~SMsg_GetServerStatus();

  void Init(int status = STATUS_SERVER, lyra_id_t id = 0);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int Status() const;
  lyra_id_t ID() const;

  //mutators
  void SetStatus(int status);
  void SetID(lyra_id_t id);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int status;
    lyra_id_t id;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_GetServerStatus.i"
#endif

#endif /* INCLUDED_SMsg_GetServerStatus */
