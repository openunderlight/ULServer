// dump_item.cpp  -*- C++ -*-
// $Id: dump_item.cpp,v 1.2 1998-02-27 17:40:14-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// given an item string, dump its contents (as best as we can)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmItem.h"
#include "LmItemHdr.h"
#include "LmItemDefs.h"
#include "SharedConstants.h"

int _tmain(int argc, TCHAR** argv)
{
  st_init();
  // if there's an argument, it is the item string, otherwise prompt for it
  TCHAR itemstr[80];
  if (argc == 2) {
   _tcsnccpy(itemstr, argv[1], sizeof(itemstr));
  }
  else {
   _tprintf(_T("Enter item string: "));
   _tscanf(_T("%s"), itemstr);
  }

  LmItem item;
  if (item.Parse(itemstr) < 0) {
   _tprintf(_T("Error: item string '%s' did not parse.\n"), itemstr);
  }
  else {
   _tprintf(_T("Item Dump:\n\n"));
    item.Dump(stdout);
  }
  //st_kill();
  return 0;
}

