// LmPlayerNameMap.cpp  -*- C++ -*-
// $Id: LmPlayerNameMap.cpp,v 1.9 1997-11-06 18:20:59-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <string.h>
#include <ctype.h>

// hash maps do not work properly with wide characters, so if
// UNICODE is defined, use a normal map instead
#ifdef _UNICODE
#include <map>
#else
//#include <ext/hash_map>
#include <unordered_map>
#include <tr1/functional>
#endif

#include "LmPlayerNameMap.h"
#include "LmLocker.h"
#include "LmPlayerDBC.h"
#ifndef USE_INLINE
#include "LmPlayerNameMap.i"
#endif


DECLARE_TheFileName;

////
// LmPlayerNameMapImp definition
////

struct string_equal {
  bool operator()(const TCHAR* s1, const TCHAR* s2) const {
#ifdef WIN32
	  return _tcsicmp(s1, s2) == 0;
#else
    return strcasecmp(s1, s2) == 0; // case-insensitive version
#endif
  };
};

#ifdef _UNICODE // use regular map
// map from playerid to name
typedef std::map<int, const TCHAR*, equal_to<int>> id2name_t;
// map from name to playerid
typedef std::map<const TCHAR*, int, string_equal> name2id_t;
#else
// map from playerid to name
typedef std::unordered_map<int, const TCHAR*, std::tr1::hash<int>, std::equal_to<int>> id2name_t;
// map from name to playerid
typedef std::unordered_map<const TCHAR*, int, std::tr1::hash<const TCHAR*>, string_equal> name2id_t;
#endif

struct LmPlayerNameMapImp {
  id2name_t id2name;
  name2id_t name2id;
};


////
// Constructor
////

LmPlayerNameMap::LmPlayerNameMap(LmPlayerDBC* playerdb)
  : playerdb_(playerdb),
    imp_(LmNEW(LmPlayerNameMapImp()))
{
  DECLARE_TheLineNum;
  lock_.Init();
}


////
// Destructor
////

LmPlayerNameMap::~LmPlayerNameMap()
{
  DECLARE_TheLineNum;

  // delete each name from each map
  for (name2id_t::iterator i = imp_->name2id.begin(); !(i == imp_->name2id.end()); ++i) {
    TCHAR* pname = (TCHAR*) (*i).first;
    LmDELETEARRAY(pname);
  }
  
  for (id2name_t::iterator j = imp_->id2name.begin(); !(j == imp_->id2name.end()); ++j) {
    TCHAR* pname = (TCHAR*) (*j).second;
    LmDELETEARRAY(pname);
  }
 
  // delete the imp object
  LmDELETE(imp_);
}


////
// PlayerID
////

lyra_id_t LmPlayerNameMap::PlayerID(const TCHAR* playername)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock for method duration
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  // copy name into writable storage
  TCHAR pname[Lyra::PLAYERNAME_MAX];
 _tcscpy(pname, playername);
  // uppercase it
  int name_len =_tcslen(pname);
  for (int j = 0; j < name_len; ++j) {
    pname[j] = _totupper(pname[j]);
  }
  // look in name2id map
  
  name2id_t::iterator i = imp_->name2id.find((const TCHAR*) pname);
  if (i == imp_->name2id.end()) {
    // not found, look up in database
    playerid = lookup_playerid(pname);
    if (playerid != Lyra::ID_UNKNOWN) {
      add_mapping(playerid, pname);
    }
  }
  else {
    playerid = (*i).second;
  }
  
  return playerid;
}

////
// PlayerName
////

const TCHAR* LmPlayerNameMap::PlayerName(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock for method duration
  const TCHAR* playername = 0;
  // quick check: id 0 always unknown
  if (playerid == Lyra::ID_UNKNOWN) {
    return 0;
  }
  
  // look in id2name map
  id2name_t::iterator i = imp_->id2name.find(playerid);
  if (i == imp_->id2name.end()) {
    // player not found, look up in database
    playername = lookup_playername(playerid);
    if (playername) {
      add_mapping(playerid, playername);
    }
  }
  else {
    playername = (*i).second;
  }
  
  return playername;
}

////
// AddMapping
////

void LmPlayerNameMap::AddMapping(lyra_id_t playerid, const TCHAR* playername)
{
  LmLocker mon(lock_); // lock for method duration
  // assumes playername is from the database (ie. case is correct)
  if (imp_->id2name.find(playerid) == imp_->id2name.end()) {
    add_mapping(playerid, playername);
  }
}

////
// Dump
////

void LmPlayerNameMap::Dump(FILE* f, int indent) const
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LmPlayerNameMap[%p,%d]: playerdb=[%p]>\n_T("), this, sizeof(LmPlayerNameMap), playerdb_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("name2id(%d):\n"), imp_->name2id.size());
 
  for (name2id_t::const_iterator i = imp_->name2id.begin(); !(i == imp_->name2id.end()); ++i) {
    INDENT(indent + 2, f);
   _ftprintf(f, _T("%s -> %d\n"), (*i).first, (*i).second);
  }
  INDENT(indent + 1, f);
 _ftprintf(f, _T("id2name(%d):\n"), imp_->id2name.size());
  for (id2name_t::const_iterator j = imp_->id2name.begin(); !(j == imp_->id2name.end()); ++j) {
    INDENT(indent + 2, f);
   _ftprintf(f, _T("%d -> %s\n"), (*j).first, (*j).second);
  }
  
}

////
// lookup_playerid - assumes lock is held
////

lyra_id_t LmPlayerNameMap::lookup_playerid(TCHAR* playername)
{
  DECLARE_TheLineNum;
  // null player?
  if (!playername || (_tcslen(playername) == 0)) {
    return Lyra::ID_UNKNOWN;
  }
  // get playerid from database
  lyra_id_t playerid = Lyra::ID_UNKNOWN;
  int rc = playerdb_->GetPlayerID(playername, playername); // copy into input arg
  if (rc <= 0) { // not found, or error
    playerid = Lyra::ID_UNKNOWN;
  }
  else {
    playerid = rc;
  }
  return playerid;
}

////
// lookup_playername - assumes lock is held
////

const TCHAR* LmPlayerNameMap::lookup_playername(lyra_id_t playerid)
{
  DECLARE_TheLineNum;
  // get name from database
  int rc = playerdb_->GetPlayerName(playerid, tmp);
  int sc = playerdb_->LastSQLCode();
  if (rc < 0) {
    return 0;
  }
  else if (_tcscmp(tmp, _T("unknown")) == 0) {
    return 0;
  }
  return tmp;
}

////
// add_mapping - assumes lock is held, and that mapping is not in the table
////

void LmPlayerNameMap::add_mapping(lyra_id_t playerid, const TCHAR* playername)
{
  DECLARE_TheLineNum;
  // each map must have its own copy of the name string; the name2id map uses an
  // all uppercase name
  int len =_tcslen(playername) + 1;
  TCHAR* pname1 = LmNEW(TCHAR[len]);
  TCHAR* pname2 = LmNEW(TCHAR[len]);
 _tcscpy(pname1, playername);
 _tcscpy(pname2, playername);
  // convert pname2 to uppercase
  for (int i = 0; i < len; ++i) {
    pname2[i] = _totupper(pname2[i]);
  }
  // add to both maps
  imp_->id2name[playerid] = pname1;
  imp_->name2id[pname2] = playerid;
}
 
  
