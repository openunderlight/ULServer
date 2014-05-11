// LsCodexText.cpp  -*- C++ -*-
// $Id: LsCodexText.cpp,v 1.2 1998-02-24 22:35:00-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <stdio.h>
#include <string.h>

#include <vector>
#include <algo.h>

#include "LsCodexText.h"
#include "LmLocker.h"
#include "LsMain.h"
#include "Gdbm.h"
#include "LmGlobalDB.h"
#include "LmRand.h"
#include "LmLog.h"

#ifndef USE_INLINE
#include "LsCodexText.i"
#endif

// LsCodexTextImp definition
class LsCodexTextImp : public std::vector<TCHAR*> {
public:
  // erase all strings pointed to
  void Erase() {
    for (LsCodexTextImp::iterator i = begin(); i != end(); ++i) {
      TCHAR* str = *i;
      LmDELETE(str);
    }
    erase(begin(), end());
  }
};


	// *** STRING LITERAL ***

// list of "default" codex texts
static const TCHAR* default_text[] = {
  _T("Today is a good day to dream."),
  _T("I dream, therefore I am."),
  _T("Sleep tight, don't let the nightmares bite."),
  _T("I have not yet begun to dream."),
  _T("I have a dream."),
  _T("Give me dreaming, or give me death!"),
};
static const int num_default_text = sizeof(default_text) / sizeof(TCHAR*);

////
// Constructor
////

LsCodexText::LsCodexText(LsMain* main)
  : main_(main)
{
  lock_.Init();
  imp_ = LmNEW(LsCodexTextImp());
}

////
// Destructor
////

LsCodexText::~LsCodexText()
{
  imp_->Erase();
  LmDELETE(imp_);
}

////
// Load - load description text from gdbm database
////

void LsCodexText::Load()
{
  DEFMETHOD(LsCodexText, Load);
  LmLocker mon(lock_); // lock object for method duration

  // get database file (it's in the text directory)
  TCHAR dbname[FILENAME_MAX];
  main_->GlobalDB()->GetTextFile(dbname, _T("codex.db"));
  // open it up
  Gdbm db;
  if (db.Open(dbname, GDBM_READER) < 0) {
    main_->Log()->Error(_T("%s: could not open codex database '%s'"), method, dbname);
    return;
  }
  // get number of lines in db
  int num_lines = 0;
  db.Fetch("NumLines", &num_lines);
  if (num_lines < 0) {
    main_->Log()->Error(_T("%s: numlines = %d?"), method, num_lines);
    db.Close();
    return;
  }
  if (num_lines > MAX_LINES) {
    num_lines = MAX_LINES; // truncate
  }
  // get rid of existing text
  imp_->Erase();
  // load from db
  for (int i = 0; i < num_lines; ++i) {
    TCHAR desc[Lyra::MAX_ITEMDESC];
    char key[20];
   _stprintf(key, _T("Text_%d"), i);
    if (db.Fetch(key, desc, sizeof(desc)) == 0) {
      int len =_tcslen(desc) + 1;
      TCHAR* newdesc = LmNEW(TCHAR[len]);
      memset(newdesc, 0, len);
     _tcscpy(newdesc, desc);
      imp_->push_back(newdesc);
    }
  }
  // done
  db.Close();
}

////
// CopyDescription - copy a random description into given string
////

void LsCodexText::CopyDescription(TCHAR* desc, int len)
{
  LmLocker mon(lock_); // lock object for method duration
  const TCHAR* chosen = _T("I have not yet begun to dream.");
  // if there isn't anything there, use one of the hardcoded ones
  if (imp_->size() == 0) {
    chosen = default_text[LmRand::Generate(0, num_default_text - 1)];
  }
  else {
    // shuffle the list
    random_shuffle(imp_->begin(), imp_->end());
    // pick the first one
    LsCodexTextImp::iterator i = imp_->begin();
    chosen = *i;
  }
  // copy it
 _tcsnccpy(desc, chosen, len);
}

////
// Dump
////

void LsCodexText::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object for method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<LsCodexText[%p,%d]: main=[%p] imp=[%p] size=%d>\n"), this, sizeof(LsCodexText),
	  main_, imp_, imp_->size());
  // print out items
  for (LsCodexTextImp::const_iterator i = imp_->begin(); i != imp_->end(); ++i) {
    INDENT(indent + 1, f);
   _ftprintf(f, _T("text: %s\n"), *i);
  }
}
