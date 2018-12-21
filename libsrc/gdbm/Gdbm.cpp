// Gdbm.cpp  -*- C++ -*-
// $Id: Gdbm.cpp,v 1.11 1997-10-22 19:21:07-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// Gdbm implementation

// MBCS NOTE: 
// All key names should always be passed in SBCS form, because GDBM
// and the Registry do not have wide character access functions.

#ifdef __GNUC__
#pragma implementation "Gdbm.h"
#endif

#include <gdbm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "Gdbm.h"

#ifndef WIN32
// initialize a datum structure
static void init_datum(datum* d);
static void init_datum(datum* d, const TCHAR* dptr);
#endif

////
// constructor
////

#ifdef WIN32
Gdbm::Gdbm()
  : regkey_(0)
{
  // empty
}
#else
Gdbm::Gdbm()
  : dbf_(0)
{
  // empty
}
#endif

////
// destructor
////

Gdbm::~Gdbm()
{
#ifdef WIN32
	if (regkey_) {
	RegCloseKey(regkey_);
	regkey_ = NULL;
	}
#else
  if (dbf_) {
    gdbm_close(dbf_);
    dbf_ = NULL;
  }
#endif
}

////
// Open: open database, return 0 if successful, -1 otherwise
////

int Gdbm::Open(const TCHAR* dirname, const TCHAR* filename, int flags, int mode)
{
  TCHAR fname[FILENAME_MAX];
  if (_tcslen(dirname) > 0) {
	_stprintf(fname, _T("%s/%s"), dirname, filename);
  }
  else {
	_stprintf(fname, _T("%s"), filename);
  }
  return Open(fname, flags, mode);
}

int Gdbm::Open(const TCHAR* filename, int flags, int mode)
{
  Close();
#ifdef WIN32 // use registry to mimic use of GDBM
#ifdef _UNICODE
	char mbcs_filename[256];
	int len = _tcslen(filename);
	if (wcstombs(mbcs_filename, filename, len) < len)
		return -1;
	char keyname[256];
	DWORD result;
	sprintf(keyname, "%s%s", REGISTRY_GDBM_KEY, mbcs_filename);
	if ((mode & GDBM_NEWDB) || (mode & GDBM_WRCREATE)) {
		result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, NULL,0,
			KEY_ALL_ACCESS, NULL, &regkey_, NULL);
	} else {
		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, 
			KEY_ALL_ACCESS, &regkey_);
	}
	if (result == ERROR_SUCCESS)
		return 0;
	else
		return -1;
#else
	char keyname[256];
	DWORD result;
	sprintf(keyname, "%s%s", REGISTRY_GDBM_KEY, filename);
	if ((mode & GDBM_NEWDB) || (mode & GDBM_WRCREATE)) {
		result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, NULL,0,
			KEY_ALL_ACCESS, NULL, &regkey_, NULL);
	} else {
		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, 
			KEY_ALL_ACCESS, &regkey_);
	}
	if (result == ERROR_SUCCESS)
		return 0;
	else
		return -1;
#endif // _UNICODE
#else // WIN32
#ifdef _UNICODE
  dbf_ = gdbm_open((char*) filename, 1024, flags, mode, NULL);
  if (dbf_ == 0) {
    return -1;
  }
  return 0;
#else
  dbf_ = gdbm_open((char*) filename, 1024, flags, mode, NULL);
  if (dbf_ == 0) {
    return -1;
  }
  return 0;
#endif // _UNICODE
#endif
}

////
// Close: close database
////

void Gdbm::Close()
{
#ifdef WIN32
	if (regkey_) {
	RegCloseKey(regkey_);
	regkey_ = NULL;
	}
#else
  if (dbf_) {
    gdbm_close(dbf_);
    dbf_ = NULL;
  }
#endif
}

////
// Store: store string value in database, return 0 if successful, -1
//   if error, 1 if key was already in database and flags did not
//   specify replace
////

int Gdbm::Store(const TCHAR* key, const TCHAR* value, int flag)
{
#ifdef WIN32
  DWORD result = LyraRegSetValueEx(regkey_, (const char*)key, 0, REG_SZ,  
		(const char*)value, _tcslen(value));
  if (result == ERROR_SUCCESS)
	  return 0;
  else
	  return -1;
#else
  datum keyd, contentd;

  init_datum(&keyd, (const TCHAR*)key);
  init_datum(&contentd, value);
  return gdbm_store(dbf_, keyd, contentd, flag);
#endif
}

////
// Store: store an integer value (as a string, naturally); return values
//   same as above.
////

int Gdbm::Store(const TCHAR* key, int value, int flag)
{
#ifdef WIN32
  DWORD result = LyraRegSetValueEx(regkey_, key, 0, REG_DWORD,  
		(const TCHAR*)&value, sizeof(value));
  if (result == ERROR_SUCCESS)
	  return 0;
  else
	  return -1;
#else
  TCHAR str[80];  // long enough for a big integer
 _stprintf(str, _T("%d"), value);
  return Store((TCHAR*)key, str, flag);
#endif
}

////
// Fetch: get a string value from database.  returns 0 if found, -1 if
//   not.  Silently truncates value if result is too large
///

int Gdbm::Fetch(const TCHAR* key, TCHAR* value, int value_size) const
{
#ifdef WIN32
  DWORD reg_type, size = value_size;
  DWORD result = LyraRegQueryValueEx(regkey_, key, NULL, &reg_type,
		(TCHAR*)value, &size);
  if (result == ERROR_SUCCESS)
	  return 0;
  else
	  return -1;
#else
  datum keyd, resultd;

  init_datum(&resultd);
  init_datum(&keyd, key);
  resultd = gdbm_fetch(dbf_, keyd);
  // find it?
  if (resultd.dptr) {
   _tcsnccpy(value, resultd.dptr, MIN(resultd.dsize, value_size));
    // null terminate
    value[value_size-1] = '\0';
    if (resultd.dsize < value_size) {
      value[resultd.dsize] = '\0';
    }
    free(resultd.dptr);
    return 0;
  }
  // not found
  return -1;
#endif
}

////
// Fetch: get an int value from database.  returns 0 if found, -1 if
//   not.
///

int Gdbm::Fetch(const TCHAR* key, int* value) const
{
#ifdef WIN32
  DWORD reg_type, size = sizeof(int);
  DWORD result = LyraRegQueryValueEx(regkey_, key, NULL, &reg_type,
		(TCHAR *)value, &size);
  if (result == ERROR_SUCCESS)
	  return 0;
  else
	  return -1;
#else
  TCHAR str[80];  // large enough for a real big integer
  if (Fetch(key, str, sizeof(str)) == 0) {
    *value = _ttoi(str);
    return 0;
  }
  return -1;
#endif
}

////
// Exists: lookup key, and return true if it is in the database, false if not
////

bool Gdbm::Exists(const TCHAR* key) const
{
#ifdef WIN32
  TUCHAR buffer[1024];
  DWORD reg_type, size = 1024;
  DWORD result = LyraRegQueryValueEx(regkey_, key, NULL, &reg_type,
		(TCHAR *)buffer, &size);
  if (result == ERROR_SUCCESS)
	  return true;
  else
	  return false;
#else
  datum keyd;

  init_datum(&keyd, key);
  return gdbm_exists(dbf_, keyd);
#endif
}

////
// Delete: remove key from database; return -1 if error (nonexistent key or
//   caller was a database reader); return 0 if successful.
////

int Gdbm::Delete(const TCHAR* key)
{
#ifdef WIN32
  DWORD result = RegDeleteValue(regkey_, key);
  if (result == ERROR_SUCCESS)
	  return true;
  else
	  return false;
#else
  datum keyd;
  init_datum(&keyd, key);
  return gdbm_delete(dbf_, keyd);
#endif
}

////
// Reorganize: call gdbm_reorganize on the database; return result of the function
////

int Gdbm::Reorganize()
{
#ifdef WIN32
  return 0;
#else
  return gdbm_reorganize(dbf_);
#endif
}

////
// Sync: flush any pending database updates to disk
////

void Gdbm::Sync()
{
#ifdef WIN32
  RegFlushKey(regkey_);
#else
  gdbm_sync(dbf_);
#endif
}

////
// GdbmError: return gdbm equivalent of errno [doesn't really work in MT environ]
////

int Gdbm::GdbmError() const
{
#ifdef WIN32
  return GetLastError();
#else
  return gdbm_errno;
#endif
}

////
// GdbmStringError: return error string
////

const TCHAR* Gdbm::GdbmStringError() const
{
#ifdef WIN32
  static TCHAR buffer[1024];
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
	  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)&buffer, 0, NULL);
  return buffer;
#else
  return gdbm_strerror(gdbm_errno);
#endif
}

////
// SetOption: set gdbm option; return 0 if successful, -1 otherwise
////

int Gdbm::SetOption(int option, int* value, int size)
{
#ifdef WIN32
  return 0;
#else
  return gdbm_setopt(dbf_, option, value, size);
#endif
}

#ifndef WIN32

////
// init_datum: initialize a datum structure
////

void init_datum(datum* d)
{
  d->dptr = 0;
  d->dsize = 0;
}

void init_datum(datum* d, const TCHAR* dptr)
{
  // cast away const-ness... oh well, so much for const safety
  d->dptr = (char*) dptr;
  d->dsize =_tcslen(dptr);
}
#endif

#ifdef WIN32
// Utilities: these wrapper functions are used to access the Win32
// registry API with _UNICODE builds; use these when writing
// string values to the registry. DO NOT use them for binary data!!!

LONG LyraRegSetValueEx(
  HKEY hKey,           // handle to key
  LPCTSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  const TCHAR* lpData,  // value data
  DWORD cbData)         // size of value data
{
#ifdef _UNICODE
	int len = _tcslen(lpData);
	char* mbcs_data = new char[len];
	if (wcstombs(mbcs_data, lpData, len) < len)
		return -1;
	DWORD result = RegSetValueEx(hKey, lpValueName, Reserved, dwType, 
			(const unsigned char*)mbcs_data, cbData);
	delete [] mbcs_data; 
	return result;
#else
	return RegSetValueEx(hKey, lpValueName, Reserved, dwType, 
			(const TUCHAR*)lpData, cbData);
#endif
}

LONG LyraRegQueryValueEx(
  HKEY hKey,            // handle to key
  LPCTSTR lpValueName,  // value name
  LPDWORD lpReserved,   // reserved
  LPDWORD lpType,       // type buffer
  TCHAR* lpData,        // data buffer
  LPDWORD lpcbData)      // size of data buffer
{


#ifdef _UNICODE
	int len = _tcslen(lpData);
	char* mbcs_data = new char[len];
	if (wcstombs(mbcs_data, lpData, len) < len)
		return -1;
	DWORD result = RegQueryValueEx(hKey, lpValueName, lpReserved, lpType,
		(const TUCHAR*)mbcs_data, lpcbData);
	delete [] mbcs_data; 
	return result;
#else
	return RegQueryValueEx(hKey, lpValueName, lpReserved, lpType,
		(TUCHAR*)lpData, lpcbData);
#endif
}

#endif // WIN32

