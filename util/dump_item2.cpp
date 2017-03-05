// dump_item2.cpp  -*- C++ -*-
// $Id: dump_item2.cpp,v 1.1 1998-02-27 17:40:14-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// given an item's state, dump its contents (as best as we can)

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
  pth_init();
  // if there are 5 arguments, treat them as item state
  int h1, h2, s1, s2, s3;
  h1 = h2 = s1 = s2 = s3 = 0;
  if (argc == 6) {
    h1 = _ttoi(argv[1]);
    h2 = _ttoi(argv[2]);
    s1 = _ttoi(argv[3]);
    s2 = _ttoi(argv[4]);
    s3 = _ttoi(argv[5]);
  }
  else {
   _tprintf(_T("Enter the item header and serial integers, separated by spaces: "));
   _tscanf(_T("%d %d"), &h1, &h2);
   _tprintf(_T("Enter the three item state integers, separated by spaces: "));
   _tscanf(_T("%d %d %d"), &s1, &s2, &s3);
  }

  LmItem item;
  item.Init(h2, h1, _T("ItemName"), s1, s2, s3);
 _tprintf(_T("Item Dump:\n\n"));
  item.Dump(stdout);

  pth_kill();
 
  return 0;
}

