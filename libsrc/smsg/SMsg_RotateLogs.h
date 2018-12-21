// SMsg_Rotate.h  -*- C++ -*-
// $Id: SMsg_Rotate.h,v 1.1 1997-08-15 16:30:14-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// tell servers to rotate their logs

#ifndef INCLUDED_SMsg_Rotate
#define INCLUDED_SMsg_Rotate

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_RotateLogs : public LmMesg {

public:

  SMsg_RotateLogs();
  ~SMsg_RotateLogs();

  void Init(int type);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int RotateLogsType() const;

  // mutators
  void SetRotateLogsType(int type);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int type;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_RotateLogs.i"
#endif

#endif /* INCLUDED_SMsg_Rotate */
