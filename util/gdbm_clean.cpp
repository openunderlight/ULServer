// gdbm_clean.cpp  -*- C++ -*-
// $Id: gdbm_clean.cpp,v 1.2 1997-07-14 16:02:08-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// reorganize GDBM database 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "LyraDefs.h"
#include "gdbm.h"

int _tmain(int argc, TCHAR** argv)
{
  if (argc != 3) {
   _tprintf(_T("usage: gdbm_clean infile.db outfile.db\n"));
    exit(1);
  }

  st_init();

  // parse args

  TCHAR* infile = argv[1];
  TCHAR* outfile = argv[2];

  // open databases
  GDBM_FILE db_in, db_out;
  db_in = gdbm_open(infile, 512, GDBM_READER, 0444, 0);
  if (!db_in) {
   _tprintf(_T("Error: could not open input '%s': %d = '%s'\n"),
	   infile, gdbm_errno, gdbm_strerror(gdbm_errno));
    exit(1);
  }
  db_out = gdbm_open(outfile, 512, GDBM_NEWDB | GDBM_FAST, 0666, 0);
  if (!db_out) {
   _tprintf(_T("Error: could not open output '%s': %d = '%s'\n"),
	   outfile, gdbm_errno, gdbm_strerror(gdbm_errno));
    exit(1);
  }

  // copy from in to out
  datum key, nextkey, val;
  key = gdbm_firstkey(db_in);
  while (key.dptr) {
    nextkey = gdbm_nextkey(db_in, key);
    val = gdbm_fetch(db_in, key);
    gdbm_store(db_out, key, val, GDBM_REPLACE);
    key = nextkey;
  }

  gdbm_reorganize(db_out);

  // close databases
  gdbm_close(db_in);
  gdbm_close(db_out);

  //st_kill();

  return 0;
}

