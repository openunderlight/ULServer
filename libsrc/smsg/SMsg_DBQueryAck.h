// SMsg_DBQueryAck.h  -*- C++ -*-
// $Id: SMsg_DBQueryAck.h,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// set avatar desc

#ifndef INCLUDED_SMsg_DBQueryAck
#define INCLUDED_SMsg_DBQueryAck

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// message class

class SMsg_DBQueryAck : public LmMesg {

public:

  SMsg_DBQueryAck();
  ~SMsg_DBQueryAck();


  void Init(lyra_id_t player_id,  unsigned char db, 
			unsigned char thread_type, unsigned int size, unsigned char* result);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  unsigned char DB() const;
  unsigned char ThreadType() const;
  lyra_id_t ID() const;
  unsigned int Size() const;
  unsigned char* Result() const;

  // mutators
  void SetResult(unsigned char* result, unsigned int size);
  void SetDB(unsigned char db);
  void SetThreadType(unsigned char thread_type);
  void SetID(lyra_id_t id);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
	lyra_id_t player_id;
	unsigned char db;
	unsigned char thread_type;
	unsigned char unused;
	unsigned char unused2;
	unsigned int size;
    unsigned char result[Lyra::MAX_DB_QUERY_ACK];
   } data_;

};

#ifdef USE_INLINE
#include "SMsg_DBQueryAck.i"
#endif

#endif /* INCLUDED_SMsg_DBQueryAck */
