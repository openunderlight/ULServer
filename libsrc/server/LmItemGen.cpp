// LmItemGen.cpp  -*- C++ -*-
// $Id: LmItemGen.cpp,v 1.12 1998-02-20 20:26:19-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#include <string.h>

#include "LmItemGen.h"
#include "LyraDefs.h"
#include "LmRand.h"
#include "LmItem.h"
#include "LmItemHdr.h"
#include "LmItemDefs.h"
#include "SharedConstants.h"
#include "LmItemGenTables.h"
#include "LmItemGenNames.h"

#ifndef USE_INLINE
#include "LmItemGen.i"
#endif

#define ITEMGENCOLORS
#include "LmItemGenColors.cpp"

// file-local functions
static void gen_item_name(int bitmap, int color1, int color2, TCHAR* name);

static void gen_changestat_item_state(int gen_type, int& modifier_index, lyra_item_change_stat_t& state);
static void gen_changestat_item(int gen_type, LmItem& item);
static void gen_armor_item_state(int gen_type, lyra_item_armor_t& state);
static void gen_armor_item(int gen_type, LmItem& item);
static void gen_effect_player_item_state(int gen_type, int& dur_index, int& pos_effect, lyra_item_effect_player_t& state);
static void gen_effect_player_item(int gen_type, LmItem& item);
static void gen_missile_item_state(int gen_type, int& damage_index, lyra_item_missile_t& state);
static void gen_missile_item(int gen_type, LmItem& item);
static void gen_codex(LmItem& item);

// macros
#define CHECK_GENTYPE(gt) if (((gt) < LmItemGen::MIN_GENTYPE) || ((gt) > LmItemGen::MAX_GENTYPE)) \
                            (gt) = LmItemGen::MIN_GENTYPE;


////
// GenerateItem
////

void LmItemGen::GenerateItem(int gen_type, int item_type, LmItem& item)
{
  // if item_type is any, randomly choose
	if (item_type == ITEM_ANY) {

		int token_chk;

		// generate the odds of creating a token...these are seperate from the general item calculations
		switch (gen_type) {
		case 4:
			// forcing level 4s to spit these out for testing purposes
			token_chk = 1;//LmRand::Generate(0, 2500);
			break;
		case 3:
			token_chk = LmRand::Generate(0, 5000);
			break;
		case 2:
			token_chk = LmRand::Generate(0, 7500);
			break;
		case 1:
		default:
			token_chk = LmRand::Generate(0, 10000);
		}

		if (token_chk == 0)
			item_type = ITEM_TOKEN;
		else
			item_type = LmRand::Generate(ITEM_CHANGESTAT, ITEM_MISSILE);
	}

  // codex generator?
  if (gen_type == CODEX_GENERATOR) {
    item_type = ITEM_CODEX;
  }
  // switch on item type
  switch (item_type) {
  default:
  case ITEM_CHANGESTAT:
    gen_changestat_item(gen_type, item);
    break;
  case ITEM_ARMOR:
    gen_armor_item(gen_type, item);
    break;
  case ITEM_EFFECTPLAYER:
    gen_effect_player_item(gen_type, item);
    break;
  case ITEM_MISSILE:
    gen_missile_item(gen_type, item);
    break;
  case ITEM_TOKEN:
	  gen_token_item(gen_type, item);
	  break;
  case ITEM_CODEX:
    gen_codex(item);
    break;
  }
}


////
// gen_item_name - assign item name based on bitmap and colors
////

// table of color names

void gen_item_name(int bitmap, int color1, int color2, TCHAR* name)
{
  // initialize
  const TCHAR* bitmap_name = GEN_ARTIFAX;
  const TCHAR* color1_name = GEN_C1;
  const TCHAR* color2_name = GEN_C2;

  // determine bitmap name
  switch (bitmap) {                    //    XXXXXXX
  case LyraBitmap::TALISMAN0: bitmap_name = GEN_ARTIFAX; break;
  case LyraBitmap::TALISMAN1: bitmap_name = GEN_ELEMEN; break;
  case LyraBitmap::TALISMAN2: bitmap_name = GEN_CHARM; break;
  case LyraBitmap::TALISMAN3: bitmap_name = GEN_CHAK; break;
  case LyraBitmap::TALISMAN4: bitmap_name = GEN_ALT; break;
  case LyraBitmap::TALISMAN5: bitmap_name = GEN_SHIELD; break;
  case LyraBitmap::TALISMAN6: bitmap_name = GEN_TALIS; break;
  case LyraBitmap::TALISMAN7: bitmap_name = GEN_ARTIFAX; break;
  case LyraBitmap::TALISMAN8: bitmap_name = GEN_ARTIFAX; break;
  default:                    bitmap_name = GEN_ARTIFAX; break;
  }

  // determine color names
  if (RANGE_OK(color1, 0, 15)) {
    color1_name = colors[color1];
  }
  else {
    color1_name = 0; // doh!
  }
  if (RANGE_OK(color2, 0, 15)) {
    color2_name = colors[color2];
  }
  else {
    color2_name = 0; // doh!
  }

  // if one of the color names is 0, then use a generic name
  if (!color1_name || !color2_name) {
   _stprintf(name, GEN_MULTIHUED, bitmap_name);
  }
  // if colors are different, use both
  else if (color1 != color2) {
   _stprintf(name, _T("%s %s %s"), color1_name, color2_name, bitmap_name);
  }
  // colors are the same, no need to repeat oneself
  else {
   _stprintf(name, _T("%s %s"), color1_name, bitmap_name);
  }
}

////
// gen_armor_item_state - fill in state info for an armor item
////

void gen_armor_item_state(int gen_type, lyra_item_armor_t& state)
{
  // init, set function type: ARMOR_FUNCTION
  state.type = LyraItem::ARMOR_FUNCTION;
  state.curr_durability = 0;
  state.max_durability = 0;
  state.absorption = 0;

  CHECK_GENTYPE(gen_type);
  const armor_gen_table_entry& entry = armor_gen_table[gen_type];

  // choose durability (max == curr)
  state.max_durability = LmRand::Generate(entry.min_durability, entry.max_durability);
  // set curr_durability to max
  state.curr_durability = state.max_durability;
  // choose absorption
  state.absorption = LmRand::Generate(entry.min_absorption, entry.max_absorption);
}

////
// gen_armor_item - generate a single function armor item
////

void gen_armor_item(int gen_type, LmItem& item)
{
  LmItemHdr hdr;
  hdr.Init(0, 0);

  CHECK_GENTYPE(gen_type);
  const armor_gen_table_entry& entry = armor_gen_table[gen_type];

  // fill in state info
  lyra_item_armor_t state;
  gen_armor_item_state(gen_type, state);

  // bitmap to use: talisman bitmap 5
  hdr.SetGraphic(LyraBitmap::TALISMAN5);
  // flags to set: none (state must be mutable)
  hdr.SetFlags(0);
  // state descriptor to use: 2 (single use, 4 bytes of state)
  hdr.SetStateFormat(LyraItem::FormatType(sizeof(state), 0, 0));

  // set color1 - absorption range: 2-75; divice by 5 -> 0..15
  hdr.SetColor1(state.absorption / 5);
  // set color2 - durability range: 10-99; divide by 6, subtract 1 -> 0..15
  hdr.SetColor2((state.max_durability / 6) - 1);

  // make a generic name
  TCHAR name[32];
  //_stprintf(name, _T("A %d:%d"), state.max_durability, state.absorption);
  gen_item_name(hdr.Graphic(), hdr.Color1(), hdr.Color2(), name);

  // initialize item
  item.Init(hdr, name);
  item.SetCharges(1); // ?
  item.SetStateField(0, &state, sizeof(state));

  // TODO: determine what to name them for real
  // TODO: use different colors?
}

////
// gen_changestat_item_state - fill in state information for a changestat item
////

void gen_changestat_item_state(int gen_type, int& modifier_index, lyra_item_change_stat_t& state)
{
  // init, set function type: CHANGE_STAT_FUNCTION
  state.type = LyraItem::CHANGE_STAT_FUNCTION;
  state.stat = 0;
  state.modifier = 0;

  CHECK_GENTYPE(gen_type);
  const changestat_gen_table_entry& entry = changestat_gen_table[gen_type];

  // choose stat to change [0-4], giving favoritism to dreamsoul
  state.stat = Stats::DREAMSOUL;
  // this is intentionally generating to the full length to give dreamsoul an extra shot to gen
  int rnd = LmRand::Generate(0, NUM_PLAYER_STATS); 

  if (rnd < NUM_PLAYER_STATS) {	  
	  state.stat = rnd;
  }
  // choose modifier index
  modifier_index = LmRand::Generate(0, 15);
  // choose whether positive/negative, and then choose modifier
  if (LmRand::Generate(1, 100) < entry.pos_chance) { // good
    state.modifier = entry.pos_modifiers[modifier_index];
  }
  else { // bad
    state.modifier = - entry.neg_modifiers[modifier_index];
  }
}

////
// gen_changestat_item - generate a single-function changestat item
////

void gen_changestat_item(int gen_type, LmItem& item)
{
  LmItemHdr hdr;
  hdr.Init(0, 0);

  CHECK_GENTYPE(gen_type);
  const changestat_gen_table_entry& entry = changestat_gen_table[gen_type];

  // fill in state info
  lyra_item_change_stat_t state;
  int modifier_index;
  gen_changestat_item_state(gen_type, modifier_index, state);

  // bitmap to use: talisman bitmap 1
  hdr.SetGraphic(LyraBitmap::TALISMAN1);
  // flags to set: FLAG_CHANGE_CHARGES (immutable except for # of charges)
  hdr.SetFlags(LyraItem::FLAG_CHANGE_CHARGES);
  // state descriptor to use: 1 (single use, 3 bytes of state)
  hdr.SetStateFormat(LyraItem::FormatType(sizeof(state), 0, 0));

  // choose number of charges
  int num_charges = LmRand::Generate(10, 25);

  // set color1
  // case on gen_type:
  switch (gen_type) {
  default: // unknown, use least powerful
  case 1:
  case 2:
    hdr.SetColor1(state.stat * 3);
    break;
  case 3:
    hdr.SetColor1((state.stat * 3) + 1);
    break;
  case 4:
    hdr.SetColor1((state.stat * 3) + 2);
    break;
  } // end switch

  // set color2
  hdr.SetColor2(modifier_index);

  // make a generic name
  TCHAR name[32];
  //_stprintf(name, _T("CS %d:%d"), state.stat, state.modifier);
  gen_item_name(hdr.Graphic(), hdr.Color1(), hdr.Color2(), name);

  // initialize item
  item.Init(hdr, name);
  item.SetCharges(num_charges);
  item.SetStateField(0, &state, sizeof(state));

  // TODO: determine what to name them for real
  // TODO: set correct colors
}

////
// gen_missile_item_state - fill in missile item state
////

void gen_missile_item_state(int gen_type, int& damage_index, lyra_item_missile_t& state)
{
  // init, set function type: MISSILE_FUNCTION
  state.type = LyraItem::MISSILE_FUNCTION;
  state.velocity = 0;
  state.effect = LyraEffect::NONE;
  state.damage = 0;
  state.bitmap_id = 0;

  CHECK_GENTYPE(gen_type);
  const missile_gen_table_entry& entry = missile_gen_table[gen_type];

  // choose bitmap
  state.bitmap_id = entry.missile_bitmap;
  // choose velocity (bouncing as well)
  state.velocity = LmRand::Generate(entry.min_velocity, entry.max_velocity);
  if (LmRand::Generate(1, 100) < entry.bounce_chance) {
    state.velocity = - state.velocity; // negated: bouncing
  }
  // choose damage
  damage_index = LmRand::Generate(0, 15);
  state.damage = entry.damage[damage_index];
  // determine if it should have an effect, and choose effect
  if (LmRand::Generate(1, 100) < entry.effect_chance) {
    state.effect = LmRand::Generate(entry.min_effect, entry.max_effect);
    if (state.effect == LyraEffect::PLAYER_INVISIBLE) {
      state.effect = LyraEffect::PLAYER_CHAMELED;
    }
  }
}

////
// gen_missile_item - generate a single function missile item
////

void gen_missile_item(int gen_type, LmItem& item)
{
  LmItemHdr hdr;
  hdr.Init(0, 0);

  CHECK_GENTYPE(gen_type);
  const missile_gen_table_entry& entry = missile_gen_table[gen_type];

  // fill in state info
  lyra_item_missile_t state;
  int damage_index;
  gen_missile_item_state(gen_type, damage_index, state);

  // bitmap to use
  hdr.SetGraphic(entry.talisman_bitmap);
  // flags to set: FLAG_CHANGE_CHARGES (immutable except for # of charges)
  hdr.SetFlags(LyraItem::FLAG_CHANGE_CHARGES);
  // state descriptor to use: 4 (single use, 6 bytes of state)
  hdr.SetStateFormat(LyraItem::FormatType(sizeof(state), 0, 0));
  // choose number of charges
  int num_charges = LmRand::Generate(entry.min_charges, entry.max_charges);

  // set color1 (based on generator type, and a random focus stat)
  int focus = LmRand::Generate(0, 3);
  hdr.SetColor1(focus * 4 + (gen_type - 1));
  // hdr.SetColor1(((gen_type - 1) * 4) + LmRand::Generate(0, 3));

  // set color2
  hdr.SetColor2(state.effect);

  // make a generic name
  TCHAR name[32];
  //_stprintf(name, _T("M %d:%d:%d:%d"), state.velocity, state.effect, state.damage, state.bitmap_id);
  gen_item_name(hdr.Graphic(), hdr.Color1(), hdr.Color2(), name);

  // initialize item
  item.Init(hdr, name);
  item.SetCharges(num_charges);
  item.SetStateField(0, &state, sizeof(state));

  // TODO: determine what to name them for real
  // TODO: use correct bitmap_id
  // TODO: use correct colors?
}

void gen_token_item(int gen_type, LmItem& item)
{
	LmItemHdr hdr;
	hdr.Init(0, 0);

	CHECK_GENTYPE(gen_type);

	// create the item state
	lyra_item_support_t state = { LyraItem::SUPPORT_FUNCTION, 0, 0, 0 };
	state.type = LyraItem::SUPPORT_FUNCTION;
	state.set_guild_token(Guild::NO_GUILD, Tokens::POWER_TOKEN);
	/// ARGH - we need target ID!!!
	state.set_target_id(0);
	state.set_creator_id(0);

	// create the header
	hdr.SetFlags(LyraItem::FLAG_SENDSTATE | LyraItem::FLAG_IMMUTABLE);
	hdr.SetGraphic(LyraBitmap::SOUL_ESSENCE);
	hdr.SetColor1(0); header.SetColor2(0);
	hdr.SetStateFormat(LyraItem::FormatType(LyraItem::FunctionSize(LyraItem::SUPPORT_FUNCTION), 0, 0));

	// FIXME This should be more variable with more odds to the low end and top of the range being quite rare
	int num_charges = LmRand::Generate(1, gen_type+1);

	item.Init(hdr, "Energy Token", 0, 0, 0);
	item.SetCharges(num_charges);
	item.SetStateField(0, &state, sizeof(state));
}

////
// gen_effect_player_item_state - fill in effect_player item state
////

void gen_effect_player_item_state(int gen_type, int& dur_index, int& pos_effect, lyra_item_effect_player_t& state)
{
  // init, set function type: EFFECT_PLAYER_FUNCTION
  state.type = LyraItem::EFFECT_PLAYER_FUNCTION;
  state.effect = 0;
  state.duration = 0;

  CHECK_GENTYPE(gen_type);
  const playereffect_gen_table_entry& entry = playereffect_gen_table[gen_type];

  // choose duration index
  dur_index = LmRand::Generate(0, 15);
  // choose positive effect
  pos_effect = LmRand::Generate(LyraEffect::MIN_GOOD_EFFECT, LyraEffect::MAX_GOOD_EFFECT);
  if (pos_effect == LyraEffect::PLAYER_INVISIBLE) {
    pos_effect = LyraEffect::PLAYER_CHAMELED;
 }

  // determine whether positive or negative effect
  //   choose effect (based on positive/negative)
  //   choose duration (based on positive/negative)
  if (LmRand::Generate(1, 100) < entry.pos_chance ) { // good
    state.effect = pos_effect;
    state.duration = entry.pos_duration[dur_index];
  }
  else { // bad
    state.effect = LmRand::Generate(LyraEffect::MIN_BAD_EFFECT, LyraEffect::MAX_BAD_EFFECT);
    state.duration = entry.neg_duration[dur_index];
  }
}

////
// gen_effect_player_item - generate single function effect_player item
////

void gen_effect_player_item(int gen_type, LmItem& item)
{
  LmItemHdr hdr;
  hdr.Init(0, 0);

  CHECK_GENTYPE(gen_type);
  const playereffect_gen_table_entry& entry = playereffect_gen_table[gen_type];

  // fill in state info
  lyra_item_effect_player_t state;
  int dur_index;
  int pos_effect;
  gen_effect_player_item_state(gen_type, dur_index, pos_effect, state);

  // bitmap to use: talisman bitmap 4
  hdr.SetGraphic(LyraBitmap::TALISMAN4);
  // flags to set: FLAG_CHANGE_CHARGES (immutable except for # of charges)
  hdr.SetFlags(LyraItem::FLAG_CHANGE_CHARGES);
  // state descriptor to use: 1 (single use, 3 bytes of state)
  hdr.SetStateFormat(LyraItem::FormatType(sizeof(state), 0, 0));

  // choose number of charges
  int num_charges = LmRand::Generate(entry.min_charges, entry.max_charges);

  // set color1
  hdr.SetColor1(((gen_type - 1) * 4) + (dur_index / 4));

  // set color2
  hdr.SetColor2(pos_effect); // even if bad, it looks good

  // make a generic name
  TCHAR name[32];
  //_stprintf(name, _T("EP %d:%d"), state.effect, state.duration);
  gen_item_name(hdr.Graphic(), hdr.Color1(), hdr.Color2(), name);

  // initialize item
  item.Init(hdr, name);
  item.SetCharges(num_charges);
  item.SetStateField(0, &state, sizeof(state));

  // TODO: determine what to name them for real
  // TODO: use correct colors?
}

////
// gen_codex - generate (partially) a codex item (description chosen elsewhere)
////

void gen_codex(LmItem& item)
{
  LmItemHdr hdr;
  hdr.Init(0, 0);

  // fill in state info
  lyra_item_scroll_t state;
  memset(&state, 0, sizeof(state));
  state.type = LyraItem::SCROLL_FUNCTION;
  state.set_creatorid(0);

  // bitmap to use: codex
  hdr.SetGraphic(LyraBitmap::CODEX);
  // flags to set: FLAG_IMMUTABLE (readable forever), FLAG_HASDESCRIPTION (duh)
  hdr.SetFlags(LyraItem::FLAG_IMMUTABLE | LyraItem::FLAG_HASDESCRIPTION);
  // state descriptor to use
  hdr.SetStateFormat(LyraItem::FormatType(sizeof(state), 0, 0));

  // number of charges: 255 = "infinite"
  int num_charges = 254;

  // set colors
  hdr.SetColor1(3);  // gold
  hdr.SetColor2(3);  // gold

  // make a generic name
  TCHAR name[32];
  //_stprintf(name, _T("EP %d:%d"), state.effect, state.duration);
  // gen_item_name(hdr.Graphic(), hdr.Color1(), hdr.Color2(), name);
 _stprintf(name, _T("Codex of Learning"));

  // initialize item
  item.Init(hdr, name);
  item.SetCharges(num_charges);
  item.SetStateField(0, &state, sizeof(state));

  // caller must choose description text, and create item in database
}
