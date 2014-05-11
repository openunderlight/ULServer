// LmItemGenTables.h  -*- C++ -*-
// $Id: LmItemGenTables.h,v 1.3 1997-11-06 18:21:27-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// item generator tables

#ifndef INCLUDED_LmItemGenTables
#define INCLUDED_LmItemGenTables

///
/// Armor
///

struct armor_gen_table_entry {
  int gen_type;
  int min_durability;
  int max_durability;
  int min_absorption;
  int max_absorption;
};

static armor_gen_table_entry armor_gen_table[] = {

  // class 0 (unused)
  { 0,
    0, 0,
    0, 0
  },

  // class 1
  { 1,
    10, 30,
     2, 10
  },

  // class 2
  { 2,
    10, 50,
    10, 10
  },

  // class 3
  { 3,
    30, 60,
    10, 50
  },

  // class 4
  { 4,
    30, 99,
    10, 75
  },

};

///
/// Change Stat
///

struct changestat_gen_table_entry {
  int gen_type;
  int pos_chance;
  int pos_modifiers[16];
  int neg_modifiers[16];
};

static changestat_gen_table_entry changestat_gen_table[] = {

  // class 0 (unused)
  { 0,
    80,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  },

  // class 1
  { 1,
    80,
    { 13, 14, 15, 16, 17, 18, 20, 23, 31, 32, 33, 41, 42, 48, 49, 53 },
    { 15, 15, 15, 15, 17, 17, 17, 17, 18, 18, 18, 18, 20, 20, 20, 20 }
  },

  // class 2
  { 2,
    80,
    { 19, 21, 22, 26, 27, 34, 36, 37, 44, 45, 46, 51, 52, 56, 59, 60 },
    { 19, 19, 19, 19, 21, 21, 21, 21, 26, 26, 26, 26, 34, 34, 34, 34 }
  },

  // class 3
  { 3,
    80,
    { 27, 27, 28, 28, 36, 36, 38, 38, 47, 47, 60, 60,  8,  7,  6,  6 },
    { 19, 19, 19, 19, 21, 21, 21, 21, 26, 26, 26, 26, 34, 34, 34, 34 }
  },

  // class 4
  { 4,
    80,
    { 29, 29, 29, 29, 30, 30, 30, 39, 39, 39, 40, 40, 40, 47, 47, 47 },
    { 19, 19, 19, 19, 21, 21, 21, 21, 26, 26, 26, 26, 34, 34, 34, 34 }
  },

};

///
/// Missile
///

struct missile_gen_table_entry {
  int gen_type;
  int min_charges;
  int max_charges;
  int talisman_bitmap;
  int missile_bitmap;
  int min_velocity;
  int max_velocity;
  int bounce_chance;
  int effect_chance;
  int min_effect;
  int max_effect;
  int damage[16];
};

static missile_gen_table_entry missile_gen_table[] = {

  // class 0 (unused)
  { 0,
    0, 0,
    0,
    0,
    0, 0,
    0,
    0,
    0, 0,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  },

  // class 1 - friendly wands
  { 1,
    10, 50,
    LyraBitmap::TALISMAN2,
    LyraBitmap::FIREBALL_MISSILE,
    1, 1,
    0,
    100,
    LyraEffect::MIN_GOOD_EFFECT, LyraEffect::MAX_GOOD_EFFECT,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  },

  // class 2
  { 2,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    1, 2,
    0,
    0,
    0, 0,


  {14,15,16,17,18,23,27,40,43,44,46,50,51,53,56,57}
   // { 19, 21, 22, 26, 27, 34, 36, 37, 44, 45, 46, 51, 52, 56, 59, 60 }
  },

  // class 3
  { 3,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    2, 4,
    0,
    0,
    0, 0,
  {17,18,23,27,28,38,39,43,44,46,50,51,53,56,57,60}
    //{ 19, 21, 22, 26, 27, 34, 36, 37, 44, 45, 46, 51, 52, 56, 59, 60 }
  },

  // class 4
  { 4,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    1, 7,
    100,
    50,
    LyraEffect::MIN_BAD_EFFECT, LyraEffect::MAX_BAD_EFFECT,
  { 3,4,27,28,29,38,39,43,44,46,50,51,53,57,59,60 }
	
    //{ 17, 18, 19, 20, 21, 23, 29, 30, 39, 40, 44, 45, 46, 47, 51, 52 }
  },

};

///
/// Player Effect
///

struct playereffect_gen_table_entry { 
  int gen_type;
  int pos_chance;
  int min_charges;
  int max_charges;
  int pos_duration[16];
  int neg_duration[16];
};

static playereffect_gen_table_entry playereffect_gen_table[] = {

  // class 0 (unused)
  { 0,
    0,
    0, 0,
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  },

  // class 1
  { 1,
    100,
    10, 25,
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
  },

  // class 2
  { 2,
    80,
    10, 25,
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

  // class 3
  { 3,
    50,
    10, 50,
    { 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44 },
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

  // class 4
  { 4,
    50,
    10, 25,
    { 55, 54, 53, 52, 51, 50, 49, 47, 46, 45, 44, 43, 42, 41, 40, 33 },
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

};

#endif /* INCLUDED_LmItemGenTables */
