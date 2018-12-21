// SMsg_SetAvatarDescription.h  -*- C++ -*-
// $Id: SMsg_SetAvatarDescription.h,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// set avatar desc

#ifndef INCLUDED_SMsg_SetAvatarDescription
#define INCLUDED_SMsg_SetAvatarDescription

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// message class

class SMsg_SetAvatarDescription : public LmMesg {

public:

  SMsg_SetAvatarDescription();
  ~SMsg_SetAvatarDescription();

  void Init(lyra_id_t playerid, const TCHAR* description);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const TCHAR* Description() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetDescription(const TCHAR* description);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    TCHAR description[Lyra::MAX_AVATARDESC];
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_SetAvatarDescription.i"
#endif

#endif /* INCLUDED_SMsg_SetAvatarDescription */
