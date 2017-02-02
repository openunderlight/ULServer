// gen_item.cpp  -*- C++ -*-
// $Id: gen_item.cpp,v 1.4 1997-12-12 17:03:02-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// test item generators

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmRand.h"
#include "LmItem.h"
#include "LmItemHdr.h"
#include "LmItemDefs.h"
#include "SharedConstants.h"
#include "LmItemGen.h"

int _tmain()
{
  pth_init();
  LmItem item;

  LmRand::InitSeed();

  for (int gt = LmItemGen::MIN_GENTYPE; gt <= LmItemGen::MAX_GENTYPE; ++gt) {
   _tprintf(_T("*** Generator Type %d ***\n\n"), gt);
    // print out N items of each type, for each generator type
    int N = 100;
    int i;

#if 0
   _tprintf(_T("Change Stat:\n\n"));
    for (i = 0; i < N; ++i) {
      LmItemGen::GenerateItem(gt, LmItemGen::ITEM_CHANGESTAT, item);
      item.Dump(stdout);
     _tprintf(_T("\n"));
    }

   _tprintf(_T("Armor:\n\n"));
    for (i = 0; i < N; ++i) {
      LmItemGen::GenerateItem(gt, LmItemGen::ITEM_ARMOR, item);
      item.Dump(stdout);
     _tprintf(_T("\n"));
    }

   _tprintf(_T("Effect Player:\n\n"));
    for (i = 0; i < N; ++i) {
      LmItemGen::GenerateItem(gt, LmItemGen::ITEM_EFFECTPLAYER, item);
      item.Dump(stdout);
     _tprintf(_T("\n"));
    }
#endif

   _tprintf(_T("Missile:\n\n"));
    for (i = 0; i < N; ++i) {
      LmItemGen::GenerateItem(gt, LmItemGen::ITEM_MISSILE, item);
      item.Dump(stdout);
     _tprintf(_T("\n"));
    }

  }
  pth_kill();
  return 0;
}

