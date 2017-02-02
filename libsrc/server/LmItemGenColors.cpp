// LmItemGenColors.cpp  -*- C++ -*-
// Copyright 1996-2001 Lyra LLC, All rights reserved. 
//
// item type/state definitions

// item name definitions
// these are done as #define's instead of const's so the compiler won't complain

// we use the #ifdef to only compile ourselves when included
// by LmItemGen.cpp

#ifdef ITEMGENCOLORS

const TCHAR* const colors[16] = {

  _T("Chalk"),
  _T("Blood"),
  _T("Fire"),
  _T("Gold"),

  _T("Jade"),
  _T("Teal"),
  _T("Cyan"),
  _T("Night"),

  _T("Azure"),
  _T("Plum"),
  _T("Berry"),
  _T("Sand"),

  _T("Beige"),
  _T("Tan"),
  _T("Earth"),
  _T("Abyss"),

};

#endif

