// LsLevelSerials.h  -*- C++ -*-
// $Id: LsLevelSerials.h,v 1.2 1997-12-11 16:34:20-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// list of free serials owned by level

#ifndef INCLUDED_LsLevelSerials
#define INCLUDED_LsLevelSerials

#include <stdio.h>
#include <list>


#include "LyraDefs.h"

#include "PThMutex.h"

// forward declarations

class LsMain;
class LmDatabase;

// the class

class LsLevelSerials {

public:

  enum {
    SERIAL_LOW = 10,  // if # of free serials dips below, ask for more
    SERIAL_HIGH = 40, // don't ask for more than this
    SERIAL_MAX = (Lyra::MAX_ROOMITEMS - 10) // if we have more than this, delete
  };

public:

  LsLevelSerials(LsMain* lsmain);
  ~LsLevelSerials();

  void LoadFromDisk(LmDatabase& db);
  void SaveToDisk(LmDatabase& db);

  int LoadFromDB();
  int SaveToDB();

  void FreeSerial(int serialnum);
  int GetNextSerial();

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsLevelSerials(const LsLevelSerials&);
  //operator=(const LsLevelSerials&);

  int allocate_serials(int num_serials);

  PThMutex lock_;    // object lock
  LsMain* main_;         // pointer to main object
  std::list<int> serials_;    // list of serial numbers

};

#ifdef USE_INLINE
#include "LsLevelSerials.i"
#endif

#endif /* INCLUDED_LsLevelSerials */
