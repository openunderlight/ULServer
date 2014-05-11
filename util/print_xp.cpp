// print_xp.cpp  -*- C++ -*-
// $Id: print_xp.cpp,v 1.3 1998-02-12 15:33:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// print xp table

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LyraDefs.h"
#include "LmStats.h"

struct {
  int sphere;
  int gain_xp;
} xp_table[] = {
  // sphere    XP for next orbit
  {  0,           1000 },  // 10k        10k
  {  1,           5000 },  // 50k        60k
  {  2,          10000 },  // 100k      160k
  {  3,          50000 },  // 500k      660k
  {  4,         100000 },  // 1000k    1660k
  {  5,         250000 },  // 2500k    4160k
  {  6,         500000 },  // 5000k    9160k
  {  7,        1000000 },  // 10m     19160k
  {  8,        2500000 },  // 25m     44160k
  {  9,        5000000 },  // 50m     94160k
};

int _tmain(int argc, TCHAR** argv)
{
  // print header
 _tprintf(_T("// LmXPTable.h - automatically generated\n")
	 "\n"
	 "#ifndef INCLUDED_LMXPTABLE\n"
	 "#define INCLUDED_LMXPTABLE\n"
	 "\n"
	 "struct { int orbit; int xp_base; int xp_gain; } lyra_xp_table[] = {\n");
  int xp_base = 0;
  for (int orbit = 0; orbit < 100; ++orbit) {
    int sphere = orbit / 10;
   _tprintf(_T("  { %2d, %8d, %7d },\n"), orbit, xp_base, xp_table[sphere].gain_xp);
    xp_base += xp_table[sphere].gain_xp;
  }
 _tprintf(_T("};\n")
	 "\n"
	 "#endif /* INCLUDED_LMXPTABLE */\n");
  return 0;
}

