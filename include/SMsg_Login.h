// SMsg_Login.h  -*- C++ -*-
// $Id: SMsg_Login.h,v 1.5 1997-07-18 17:26:19-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// server-server login

#ifndef INCLUDED_SMsg_Login
#define INCLUDED_SMsg_Login

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_Login : public LmMesg {

public:

  SMsg_Login();
  ~SMsg_Login();

  void Init(int servertype, lyra_id_t id);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int ServerType() const;
  lyra_id_t ID() const;

  // mutators
  void SetServerType(int servertype);
  void SetID(lyra_id_t id);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    int servertype;
    lyra_id_t id;
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_Login.i"
#endif

#endif /* INCLUDED_SMsg_Login */
