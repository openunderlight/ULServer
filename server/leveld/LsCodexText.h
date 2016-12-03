// LsCodexText.h  -*- C++ -*-
// $Id: LsCodexText.h,v 1.1 1998-02-20 20:26:54-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// codex generator text manager

#ifndef INCLUDED_LsCodexText
#define INCLUDED_LsCodexText

#include <stdio.h>

#include "LyraDefs.h"
#include "PThMutex.h"

// class forward declarations

class LsMain;
class LsCodexTextImp;

// class declarations

class LsCodexText {

public:

  enum {
    MAX_LINES = 128  // max number of text possibilities
  };

public:

  LsCodexText(LsMain* lsmain);
  ~LsCodexText();

  void Load();
  void CopyDescription(TCHAR* desc, int length);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsCodexText(const LsCodexText&);
  //operator=(const LsCodexText&);

  LsMain* main_;

  // lock
  PThMutex lock_;

  // implementation pointer
  LsCodexTextImp* imp_;

};

#ifdef USE_INLINE
#include "LsCodexText.i"
#endif

#endif /* INCLUDED_LsCodexText */
