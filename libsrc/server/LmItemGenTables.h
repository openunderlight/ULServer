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
    30, 40,
     25, 35
  },

  // class 2
  { 2,
    30, 50,
    10, 45
  },

  // class 3
  { 3,
    50, 90,
    10, 55
  },

  // class 4
  { 4,
    75, 99,
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
    95,
    { 15, 15, 16, 17, 18, 20, 23, 31, 31, 32, 33, 41, 42, 48, 49, 53 },
    { 15, 15, 15, 15, 17, 17, 17, 17, 18, 18, 18, 18, 20, 20, 20, 20 }
  },

  // class 2
  { 2,
    90,
    { 21, 21, 21, 21, 21, 34, 34, 34, 46, 46, 51, 56, 59, 59, 60, 60 },
    { 19, 19, 19, 19, 21, 21, 21, 21, 26, 26, 26, 26, 34, 34, 34, 34 }
  },

  // class 3
  { 3,
    95,
    { 33, 33, 33, 33, 33, 38, 38, 38, 60, 60, 60, 60,  8,  7,  6,  6 },
    { 19, 19, 19, 19, 21, 21, 21, 21, 26, 26, 26, 26, 34, 34, 34, 34 }
  },

  // class 4
  { 4,
    95,
    { 29, 29, 29, 29, 29, 60, 60, 60, 60, 39, 39, 39, 44, 44, 38, 6 },
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
  int damage[32];
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
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
  },

  // class 1
  { 1,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    1, 3,
    10,
    0,
    0, 0,
    {0,0,0,1,12,13,13,14,14,14,14,2,2,16,16,16,30,30,30,19,19,19,40,40,40,15,15,15,17,17,32,32 }
  },

  // class 2
  { 2,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    2, 3,
    0,
    0,
    0, 0,


  {30,40,22,41,47,24,31,20,52,18,48,54,34,23,56,3,21,26,53,25,36,49,57,43,55,37,58,33,35,50,59,27 }
  },

  // class 3
  { 3,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    3, 5,
    50,
    25,
    LyraEffect::MIN_BAD_EFFECT, LyraEffect::PLAYER_CURSED,
  {21,42,36,33,33,59,59,4,27,27,44,44,45,45,51,51,46,46,46,46,5,28,28,60,60,39,39,39,39,38,38,38 }
  },

  // class 4
  { 4,
    10, 50,
    LyraBitmap::TALISMAN3,
    LyraBitmap::FIREBALL_MISSILE,
    4, 7,
    100,
    50,
    LyraEffect::MIN_BAD_EFFECT, LyraEffect::MAX_BAD_EFFECT,
  { 50,4,4,27,27,44,45,51,51,51,46,46,46,5,28,28,60,60,60,60,39,39,39,38,38,38,6,29,29,29,29,7 }
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
    {  42, 42, 43, 43, 44, 44, 11, 12, 39, 41, 41, 41, 41, 41, 41, 41 },
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
  },

  // class 2
  { 2,
    80,
    10, 25,
    {  51, 42,  42, 42, 43, 44, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41 },
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

  // class 3
  { 3,
    70,
    10, 25,
    { 51, 51, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44 },
    {  7,  8,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

  // class 4
  { 4,
    70,
    10, 25,
    { 55, 55, 55, 55, 54, 53, 53, 52, 52, 52, 51, 50, 49, 47, 46, 44 },
    {  51,  41,  9, 10, 11, 12, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39 },
  },

};

#endif /* INCLUDED_LmItemGenTables */
