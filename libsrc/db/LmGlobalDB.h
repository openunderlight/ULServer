// LmGlobalDB.h  -*- C++ -*-
// $Id: LmGlobalDB.h,v 1.10 1997-12-05 14:50:48-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// global properties "database"

#ifndef INCLUDED_LmGlobalDB
#define INCLUDED_LmGlobalDB

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"


// forward declarations

// the class

class LmGlobalDB {

public:

  LmGlobalDB(const TCHAR* root_dir);
  ~LmGlobalDB();

  // selectors
  const TCHAR* RootDir() const;
  const TCHAR* LogDir() const;

  // compute other directories/filenames based on rootdir
  //void GetLogFile(TCHAR* logfileresult, const TCHAR* logfilename) const;
  void GetPidFile(TCHAR* pidfileresult, const TCHAR* pidfilename) const;
  void GetDumpFile(TCHAR* dumpfileresult, const TCHAR* dumpfilename) const;
  void GetExecFile(TCHAR* execfileresult, const TCHAR* execname) const;
  void GetTextFile(TCHAR* textfileresult, const TCHAR* textfilename) const;
  void GetPasswordFile(TCHAR* pw_file) const;
  void GetPlayerDir(TCHAR* playerfile) const;
  void GetLevelState(TCHAR* statefile, int level_num) const;
  void GetPlayerFile(TCHAR* playerfile, lyra_id_t playerid) const;

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LmGlobalDB(const LmGlobalDB&);
  //operator=(const LmGlobalDB&);

  TCHAR rootdir_[FILENAME_MAX];  // top-level runtime directory
  TCHAR logdir_[FILENAME_MAX];   // logfile directory

};

#ifdef USE_INLINE
#include "LmGlobalDB.i"
#endif

#endif /* INCLUDED_LmGlobalDB */
