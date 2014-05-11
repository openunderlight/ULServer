// SMsg_Error.h  -*- C++ -*-
// $Id: SMsg_Error.h,v 1.13 1997-07-29 19:32:31-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// generic server error message

#ifndef INCLUDED_SMsg_Error
#define INCLUDED_SMsg_Error

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_Error : public LmMesg {

public:

  enum {
    // constants
    ERRMSG_LEN = 80
  };

public:

  SMsg_Error();
  ~SMsg_Error();

  void Init(int mesgtype, const TCHAR* errmsg);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int MsgType() const;
  int ErrorStringLength() const;
  const TCHAR* ErrorString() const;

  // mutators
  void SetMsgType(int mesgtype);
  void SetErrorString(const TCHAR* errstr);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    short msg_type;
    short err_len;
    TCHAR err_msg[ERRMSG_LEN];
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_Error.i"
#endif

#endif /* INCLUDED_SMsg_Error */
