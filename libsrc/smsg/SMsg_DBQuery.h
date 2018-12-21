// SMsg_DBQuery.h  -*- C++ -*-
// $Id: SMsg_DBQuery.h,v 1.1 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// set avatar desc

#ifndef INCLUDED_SMsg_DBQuery
#define INCLUDED_SMsg_DBQuery

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// message class

class SMsg_DBQuery : public LmMesg {

public:

	enum {
		DBQUERY_WANT_RESULT = 1,
		DBQUERY_NO_RESULT = 2,

		SERVER_DB = 1,
		PLAYER_DB = 2,
		ITEM_DB = 3,
		GUILD_DB = 4,
		LEVEL_DB = 5,
		BILLING_DB = 6,
	};

  SMsg_DBQuery();
  ~SMsg_DBQuery();

  void Init(lyra_id_t player_id, unsigned char result, unsigned char db, 
			unsigned char thread_type, const TCHAR* query);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  unsigned char Result() const;
  unsigned char DB() const;
  unsigned char ThreadType() const;
  lyra_id_t ID() const;
  const TCHAR* Query() const;

  // mutators
  void SetResult(unsigned char result);
  void SetDB(unsigned char db);
  void SetThreadType(unsigned char thread_type);
  void SetID(lyra_id_t id);
  void SetQuery(const TCHAR* query);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    unsigned char result;
	unsigned char db;
	unsigned char thread_type;
	unsigned char unused;
	lyra_id_t player_id;
    TCHAR query[Lyra::MAX_DB_QUERY];
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_DBQuery.i"
#endif

#endif /* INCLUDED_SMsg_DBQuery */
