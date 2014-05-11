// LmGlobalDB.cpp  -*- C++ -*-
// $Id: LmGlobalDB.cpp,v 1.12 1997-12-05 14:50:48-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LmGlobalDB.h"
#endif

#include <stdio.h>
#include <string.h>

#include "LmGlobalDB.h"
#include "LyraDefs.h"

#ifndef USE_INLINE
#include "LmGlobalDB.i"
#endif


////
// Constructor
////

LmGlobalDB::LmGlobalDB(const TCHAR* root_dir)
{
  // initialize directories
 _tcscpy(rootdir_, root_dir);
#ifdef WIN32
 _stprintf(logdir_, _T("%s\\var\\log"), rootdir_);
#else
 _stprintf(logdir_, _T("%s/var/log"), rootdir_);
#endif
}

////
// Destructor
////

LmGlobalDB::~LmGlobalDB()
{
  // empty
}

////
// GetLogFile: copy absolute pathname of logfile into result string
////

//void LmGlobalDB::GetLogFile(TCHAR* logfileresult, const TCHAR* logfilename) const
//{
// _stprintf(logfileresult, "%s/var/log/%s", RootDir(), logfilename);
//}

////
// GetPidFile: copy absolute pathname of pidfile into result string
////

void LmGlobalDB::GetPidFile(TCHAR* pidfileresult, const TCHAR* pidfilename) const
{
 _stprintf(pidfileresult, _T("%s/var/pid/%s"), RootDir(), pidfilename);
}

////
// GetDumpFile: copy absolute pathname of dumpfile into result string
////

void LmGlobalDB::GetDumpFile(TCHAR* dumpfileresult, const TCHAR* dumpfilename) const
{
 _stprintf(dumpfileresult, _T("%s/var/dump/%s"), RootDir(), dumpfilename);
}

////
// GetExecFile: copy absolute pathname of executable (sbin) into result string
////

void LmGlobalDB::GetExecFile(TCHAR* execfileresult, const TCHAR* execname) const
{
 _stprintf(execfileresult, _T("%s/sbin/%s"), RootDir(), execname);
}

////
// GetTextFile: copy absolute pathname of textfile into result string
////

void LmGlobalDB::GetTextFile(TCHAR* textfileresult, const TCHAR* textname) const
{
 _stprintf(textfileresult, _T("%s/var/text/%s"), RootDir(), textname);
}

////
// GetPasswordFile: copy absolute pathname of database password file into given string
////

void LmGlobalDB::GetPasswordFile(TCHAR* pw_file) const
{
 _stprintf(pw_file, _T("%s/src/pw.txt"), RootDir());
}

////
// GetLevelState: copy absolute pathname of level state file into given string
////

void LmGlobalDB::GetLevelState(TCHAR* statefile, int level_num) const
{
 _stprintf(statefile, _T("%s/var/db/level/level_%d.state"), RootDir(), level_num);
}

////
// GetPlayerFile: copy absolute pathname of player file into given string
////

void LmGlobalDB::GetPlayerFile(TCHAR* playerfile, lyra_id_t playerid) const
{
 _stprintf(playerfile, _T("%s/var/db/player/player_%u.db"), RootDir(), playerid);
}

////
// GetPlayerDir: copy absolute pathname of player file directory into given string
////

void LmGlobalDB::GetPlayerDir(TCHAR* playerfile) const
{
 _stprintf(playerfile, _T("%s/var/db/player"), RootDir());
}



////
// Dump
////

void LmGlobalDB::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LmGlobalDB[%p,%d]: rootdir='%s'>\n"), this, sizeof(LmGlobalDB), RootDir());
}
