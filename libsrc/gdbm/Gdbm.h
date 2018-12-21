// Gdbm.h  -*- C++ -*-
// $Id: Gdbm.h,v 1.6 1997-10-14 21:38:48-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// Gdbm wrapper class

#ifndef INCLUDED_Gdbm
#define INCLUDED_Gdbm

#ifdef __GNUC__
#pragma interface
#endif
#ifdef WIN32
#include "LyraDefs.h"
#else
#include <gdbm.h>
#endif

// gdbm access class

#ifdef WIN32 // need to define datum type, gdbm constants
#define GDBM_READER 0
#define GDBM_WRITER 1
#define GDBM_WRCREATE 2
#define GDBM_NEWDB 3
#define GDBM_FAST 16
#define GDBM_INSERT 0
#define GDBM_REPLACE 1

// registry keys are always SBCS
#define REGISTRY_GDBM_KEY "Software\\Lyra\\ServerGDBM\\"

#endif

class Gdbm {

public:

  Gdbm();
  ~Gdbm();

  int Open(const TCHAR* dirname, const TCHAR* filename, int flags, int mode = 0666);
  int Open(const TCHAR* filename, int flags, int mode = 0666);
  void Close();

  int Store(const TCHAR* key, const TCHAR* value, int flag = GDBM_REPLACE);
  int Store(const TCHAR* key, int value, int flag = GDBM_REPLACE);
  int Fetch(const TCHAR* key, TCHAR* value, int value_size) const;
  int Fetch(const TCHAR* key, int* value) const;
  bool Exists(const TCHAR* key) const;
  int Delete(const TCHAR* key);

  int Reorganize();
  void Sync();

  int GdbmError() const;
  const TCHAR* GdbmStringError() const;

  int SetOption(int option, int* value, int size);

private:

  // methods not implemented
  Gdbm(const Gdbm&);
  ////operator=(const Gdbm&);

#ifdef WIN32
  HKEY regkey_;
#else
  GDBM_FILE dbf_;
#endif

};

#ifdef WIN32
// wide character registry wrappers for Win32; use these when writing
// string values to the registry. DO NOT use them for binary data!!!
LONG LyraRegSetValueEx(
  HKEY hKey,           // handle to key
  LPCTSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  const TCHAR* lpData,  // value data
  DWORD cbData);

LONG LyraRegQueryValueEx(

  HKEY hKey,            // handle to key
  LPCTSTR lpValueName,  // value name
  LPDWORD lpReserved,   // reserved
  LPDWORD lpType,       // type buffer
  TCHAR* lpData,        // data buffer
  LPDWORD lpcbData);      // size of data buffer

#endif // WIN32


#endif /* INCLUDED_Gdbm */
