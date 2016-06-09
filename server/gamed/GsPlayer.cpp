// GsPlayer.cpp  -*- C++ -*-
// $Id: GsPlayer.cpp,v 1.72 1998/06/18 00:56:16 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "GsPlayer.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "GsPlayer.h"
#include "GsPlayerThread.h"
#include "GMsg_All.h"
#include "PThMutex.h"
#include "GsNetworkInput.h"
#include "GsMain.h"
#include "LmPlayerDB.h"
#include "LmRoomDB.h"
#include "LmDatabase.h"
#include "LmGlobalDB.h"
#include "LmSocket.h"
#include "LmConnection.h"
#include "LmSockType.h"
#include "LmSockAddrInet.h"
#include "LmLogFile.h"
#include "LmBillingDBC.h"
#include "LmLevelDBC.h"
#include "GMsg_All.h"
#include "LmLocker.h"
#include "LmGoalInfo.h"
#include "RMsg_PlayerUpdate.h"
#include "LmItemDBC.h"
#include "LmUtil.h"
#include "LmPlayerDBF.h"
#include "LmPlayerDBC.h"
#include "LmServerDBC.h"
#include "GsUtil.h"
#include "LmTimer.h"
#include "resource.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef USE_INLINE
#include "GsPlayer.i"
#endif

art_t art_info[NUM_ARTS] = // 		  			    Evoke
{// CKS  Name				Stat			    Orb Drn Dur Time PP 	Flags
{IDS_JP,					Stats::NO_STAT,		0,  0,  0,	0,	-1, SANCT|NEIGH|LEARN},
{IDS_GK, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, FOCUS},
{IDS_DREAMSEER,				Stats::NO_STAT,		0,  0,  0,	0,	-1, FOCUS},
{IDS_SM, 					Stats::NO_STAT,		0,  0,  0,	0,	-1, FOCUS},
{IDS_FS, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, FOCUS},
{IDS_RANDOM,				Stats::NO_STAT,		0,  0,  0,	2, 	-1, SANCT|LEARN},
{IDS_MEDITATION, 			Stats::WILLPOWER,	0,  2,  15, 1,	1, SANCT|LEARN},
{IDS_RF,					Stats::WILLPOWER,	10, 5,  13, 2, 	2, SANCT|LEARN},
{IDS_PROTECTION, 			Stats::WILLPOWER,	15, 5,  13, 2, 	2, SANCT|LEARN},
{IDS_FA,					Stats::WILLPOWER,	5,  5,  13, 2, 	2, SANCT|LEARN},
{IDS_WARD, 					Stats::WILLPOWER,	20, 20, 0,	5, 	3, MAKE_ITEM|FOCUS|LEARN},
{IDS_AMULET,				Stats::WILLPOWER,	20, 1,  0,	2, 	1, SANCT|MAKE_ITEM|FOCUS|LEARN},
{IDS_SHATTER, 				Stats::DREAMSOUL,	0, 15, 0,	8, 	4, SANCT|LEARN},
{IDS_RETURN,				Stats::WILLPOWER, 50, 20, 23,   2,  2, SANCT|FOCUS|LEARN},
{IDS_KNOW, 					Stats::INSIGHT,		0,  0,  0,	1, 	-1, SANCT|LEARN},
{IDS_JUDGEMENT_ART_NAME,	Stats::INSIGHT,		10, 2,  0,	1, 	1, SANCT|NEIGH|LEARN},
{IDS_ID,					Stats::INSIGHT,		15, 6,  0,	5, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_ID_CURSE,				Stats::INSIGHT,		20, 2,  0,	1, 	1, SANCT|NEIGH|LEARN},
{IDS_CHAMELE, 				Stats::INSIGHT,		20, 20, 16, 5, 	2, SANCT|FOCUS|LEARN},
{IDS_VISION,				Stats::INSIGHT,		20, 5,  13, 2, 	2, SANCT|FOCUS|LEARN},
{IDS_BLAST,					Stats::INSIGHT,		30, 2,  0,	1, 	2, NEIGH|FOCUS|LEARN},
{IDS_BLEND,					Stats::INSIGHT,		50, 30, 6,	5, 	3, SANCT|FOCUS|LEARN},
{IDS_FORGE,					Stats::DREAMSOUL,	50, 50, 0,	8, 	-1, SANCT|MAKE_ITEM|LEARN},
{IDS_RECHARGE,				Stats::INSIGHT,		40, 15, 0,	8, 	2, SANCT|FOCUS|LEARN|NEED_ITEM},
{IDS_RESTORE, 				Stats::RESILIENCE,	10, 5,  0,	1, 	1, SANCT|LEARN},
{IDS_REWEAVE, 				Stats::RESILIENCE,	15, 10, 0,	4, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_PURIFY,				Stats::RESILIENCE,	5,  15, 0,	2, 	1, SANCT|LEARN},
{IDS_DRAIN_SELF, 			Stats::RESILIENCE,	20, 5,  0,	2, 	2, SANCT|NEIGH|FOCUS|LEARN},
{IDS_ABJURE,				Stats::RESILIENCE,	50, 30, 0,	3, 	4, FOCUS|LEARN},
{IDS_POISON,				Stats::RESILIENCE,	30, 15, 13, 3, 	2, NEIGH|FOCUS|LEARN},
{IDS_ANTIDOTE,				Stats::RESILIENCE,	30, 10, 0,	2, 	3, SANCT|FOCUS|LEARN},
{IDS_CURSE,					Stats::RESILIENCE,	40, 10, 13, 3, 	3, NEIGH|FOCUS|LEARN},
{IDS_DRAIN_ESSENCE,			Stats::RESILIENCE,	0,  0,  0,	1, 	1, SANCT|NEED_ITEM|LEARN},
{IDS_BANISH_MARE,			Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_IMPRISON_MARE,			Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_TRAP_MARE,				Stats::RESILIENCE,	50, 10, 0,	3, 	3, SANCT|NEIGH|FOCUS|LEARN},
{IDS_DREAMBLADE, 			Stats::INSIGHT,		0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_TRAIL,					Stats::LUCIDITY,	0,  2,  25, 1, 	-1, SANCT|LEARN},
{IDS_SCARE,					Stats::LUCIDITY,	10, 5,  4,	2, 	2, NEIGH|LEARN},
{IDS_STAGGER, 				Stats::LUCIDITY,	40, 10, 3,	2, 	3, NEIGH|FOCUS|LEARN},
{IDS_DEAFEN,				Stats::LUCIDITY,	5,  15, 4,	2, 	2, NEIGH|LEARN},
{IDS_BLIND,					Stats::LUCIDITY,	45, 15, 3,	3, 	3, NEIGH|FOCUS|LEARN},
{IDS_DARKNESS_ART_NAME,		Stats::LUCIDITY,	50, 25, 4,	5, 	3, NEIGH|FOCUS|LEARN},
{IDS_PARALYZE,				Stats::LUCIDITY,	30, 20, 2,	3, 	3, NEIGH|FOCUS|LEARN},
{IDS_FIRESTORM,				Stats::LUCIDITY,	50, 25, 0,	7, 	3, FOCUS|LEARN},
{IDS_RAZORWIND,				Stats::LUCIDITY,	70, 40, 6,	9, 	4, FOCUS|LEARN},
{IDS_RECALL_ART_NAME,		Stats::DREAMSOUL,	25, 1,  25, 1, 	1, SANCT|LEARN},
{IDS_PUSH, 					Stats::DREAMSOUL,	0,  0,  0,	1, 	1, NEIGH|LEARN},
{IDS_SOUL_EVOKE, 			Stats::DREAMSOUL,	15, 1,  23, 1, 	1, SANCT|LEARN},
{IDS_DREAM_STRIKE,			Stats::DREAMSOUL,	10, 30,  0,	5, 	-1, NEIGH},
{IDS_NMF,					Stats::DREAMSOUL,	40, 10, 13, 5, 	1, SANCT|LEARN},
{IDS_LOCATE,				Stats::DREAMSOUL,	0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_TRAIN,					Stats::NO_STAT,		10, 0,  0,	2, 	-1, SANCT|NEIGH},
{IDS_INITIATE,				Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_KNIGHT,				Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH},
{IDS_SUPPORT_ASCENSION,		Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_ASCEND_RULER,			Stats::NO_STAT,		0,  0,  0,	10,	-1, SANCT|NEED_ITEM},
{IDS_INSTA_COLLAPSE,		Stats::NO_STAT,		0,  0,  0,	1, 	-1, SANCT|NEIGH},		// gm only
{IDS_GRANT_XP_ART_NAME,		Stats::NO_STAT,		0,  0,  0,	1, 	-1, SANCT},				// gm only
{IDS_TERMINATE,				Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT},
{IDS_SPHERE,				Stats::NO_STAT,		20, 0,  0,	1, 	-1, SANCT|NEIGH},
{IDS_SUPPORT_DEMOTION,		Stats::NO_STAT,		0,  0,  0,	3,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_DEMOTE,				Stats::NO_STAT,		0,  0,  0,	3,	-1, SANCT},
{IDS_INVISIBILITY,			Stats::INSIGHT,		40, 20, 6,	3, 	3, SANCT|FOCUS|LEARN},
{IDS_GIVE, 					Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT|NEIGH|NEED_ITEM|LEARN},
{IDS_GATESMASHER,			Stats::WILLPOWER,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_FATESLAYER, 			Stats::LUCIDITY,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_SOULREAPER, 			Stats::RESILIENCE,	0,  5,  23, 1, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_FLAMESHAFT, 			Stats::WILLPOWER,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_TRANCEFLAME,			Stats::INSIGHT,		1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_FLAMESEAR,				Stats::RESILIENCE,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_FLAMERUIN,				Stats::LUCIDITY,	1,  2,  0,	1, 	2, FOCUS|LEARN},
{IDS_INSCRIBE,				Stats::DREAMSOUL,	20, 2,  0,	1, 	1, SANCT|MAKE_ITEM|LEARN},
{IDS_DT,					Stats::DREAMSOUL,	35, 5,	0,	2,	1, SANCT|NEED_ITEM|LEARN},
{IDS_MB,					Stats::DREAMSOUL,	30, 5, 18,	5,	2, SANCT|LEARN},
{IDS_SHOW,					Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH|NEED_ITEM|LEARN},
{IDS_AWAKEN,				Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH},		// gm only
{IDS_UNTRAIN_ART_NAME, 		Stats::NO_STAT, 	0,	0,	0,	2,	-1, SANCT|NEIGH},		// gm only
{IDS_GRANT_RP_XP, 			Stats::NO_STAT, 	0,	0,	0,	0,	-1, SANCT|NEIGH},
{IDS_DREAMQUAKE,			Stats::WILLPOWER,	60, 40, 4,	7,	3, FOCUS|LEARN},
{IDS_HYPNOTIC_WEAVE_ART_NAME,		Stats::LUCIDITY,	60, 40, 4,	7,	4, FOCUS|LEARN},
{IDS_VAMPRIC_DRAW,					Stats::RESILIENCE,	60, 5,	0,	5,	3, FOCUS|LEARN|NEIGH|NEED_ITEM},
{IDS_TERROR_ART_NAME,				Stats::LUCIDITY,	60, 40, 4,	1,	3, FOCUS|LEARN},
{IDS_HEAL_AURA,						Stats::RESILIENCE,	60, 40, 0,	7, 	-1, FOCUS|NEIGH|SANCT|LEARN},
{IDS_TELEPATHY,						Stats::NO_STAT,		75, 0,  0,	0, 	-1, SANCT|NEIGH},	// roger wilco voice
{IDS_DREAMSMITH_MARK_ART_NAME, 		Stats::NO_STAT,		50, 0,  0,	1, 	-1, SANCT},
{IDS_SUPPORT_TRAIN,					Stats::NO_STAT,		10, 0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_SUPPORT_SPHERE,				Stats::NO_STAT,		20, 0,  0,	10,	-1, SANCT|NEIGH|MAKE_ITEM},
{IDS_TRAIN_SELF,					Stats::NO_STAT,		40, 0,  0,	2, 	-1, SANCT},
{IDS_SOUL_SHIELD,					Stats::DREAMSOUL,	40, 20, 15, 5,  2, SANCT|LEARN},
{IDS_SUMMON_ART_NAME,				Stats::NO_STAT,		0,  0,  0,  0,  -1, SANCT},
{IDS_SUSPEND,						Stats::NO_STAT,		0,  0,  0,	0, 	-1, SANCT},
{IDS_REFLECT_ART_NAME,				Stats::WILLPOWER,   65, 40, 9,  3,	-1, SANCT|FOCUS|LEARN},
{IDS_SACRIFICE,						Stats::RESILIENCE,	10, 5,  0,	1, 	-1, SANCT|NEED_ITEM},
{IDS_CLEANSE_MARE,					Stats::RESILIENCE,	50, 5,  0,	1, 	1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_CREATE_ID_TOKEN,				Stats::DREAMSOUL,	0,  20, 0,	1, 	-1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_SENSE,							Stats::DREAMSOUL,	0,  0,  0,	1,  -1, SANCT|LEARN},
{IDS_EXPEL_ART_NAME,				Stats::DREAMSOUL,	20, 0,  0,	1,  -1, SANCT|NEED_ITEM|NEIGH},
{IDS_NEWLY_AWAKENED,				Stats::INSIGHT,	    0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_COMBINE,						Stats::INSIGHT,	    60, 40, 0,	2, 	-1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS|LEARN},
{IDS_POWER_TOKEN,					Stats::DREAMSOUL,	10,  0, 0,	10, -1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_SHOW_GRATITUDE,				Stats::NO_STAT,		0,   0, 0,	10, -1, SANCT|NEED_ITEM|NEIGH},
{IDS_QUEST,							Stats::NO_STAT,		0,   0, 0,	3, -1, SANCT|NEIGH|MAKE_ITEM},
{IDS_BEQUEATH,						Stats::NO_STAT,		0,   0, 0,	10, -1, SANCT|NEED_ITEM|NEIGH},
{IDS_RADIANT_BLAZE,					Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_POISON_CLOUD,					Stats::DREAMSOUL,	20, 10,15,	5,  -1, NEED_ITEM|NEIGH},
{IDS_BREAK_COVENANT,				Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_PEACE_AURA_ART_NAME,			Stats::DREAMSOUL,	20, 10, 6,	5,  -1, NEED_ITEM|SANCT},
{IDS_SABLE_SHIELD,					Stats::DREAMSOUL,	20, 10,16,	5,  -1, NEED_ITEM|SANCT},
{IDS_ENTRANCEMENT,					Stats::DREAMSOUL,	20, 10,13,	5,  -1, NEED_ITEM|SANCT},
{IDS_SHADOW_STEP,					Stats::DREAMSOUL,	20, 10,10,	5,  -1, NEED_ITEM|SANCT},
{IDS_DAZZLE,						Stats::DREAMSOUL,	20, 10, 9,	5,  -1, NEED_ITEM|NEIGH},
{IDS_GUILD_HOUSE,					Stats::NO_STAT	,	50,  0, 0,  13, -1, SANCT},
{IDS_CORRUPT_ESSENCE,				Stats::RESILIENCE,	10,  5, 0,  1,  -1, NEED_ITEM|SANCT|MAKE_ITEM},
{IDS_TEHTHUS_OBLIVION,				Stats::DREAMSOUL,	10, 10, 0,  5,  -1, NEED_ITEM|SANCT},
{IDS_CHAOS_PURGE_ART_NAME,			Stats::DREAMSOUL,	 0, 20, 0,  5,  -1, NEIGH},
{IDS_WORDSMITH_MARK_ART_NAME, 		Stats::NO_STAT,		10,  0, 0,	2, 	-1, SANCT|NEIGH},
{IDS_CUP_SUMMONS_ART_NAME,			Stats::NO_STAT,		 0,  0, 0,  0,  -1, SANCT},
{IDS_HOUSE_MEMBERS,					Stats::DREAMSOUL,	 0,  1, 0,	1, 	-1, SANCT},
{IDS_FREESOUL_BLADE,				Stats::DREAMSOUL,	 0,  0, 0,	1, 	-1, SANCT|MAKE_ITEM|NEED_ITEM|NEIGH},
{IDS_ILLUMINATED_BLADE,				Stats::DREAMSOUL,	 0,  0, 0,	1, 	-1, SANCT|MAKE_ITEM|NEED_ITEM|NEIGH},
{IDS_SUMMON_PRIME,				    Stats::DREAMSOUL,	 0, 25, 0,	10,	-1, SANCT|NEED_ITEM|MAKE_ITEM},
{IDS_GRANT_PPOINT,				    Stats::NO_STAT,		 0, 0, 0,	0,	-1, SANCT|NEIGH},
{IDS_SCAN,							Stats::INSIGHT,		10, 2,  0,	1, 	-1, SANCT|NEIGH},
{IDS_PASSLOCK,						Stats::INSIGHT,		50, 30, 6,	5, 	-1, SANCT|FOCUS},
{IDS_HEAL, 							Stats::RESILIENCE,	10, 5,  0,	1, 	-1, SANCT},
{IDS_SANCTIFY, 						Stats::WILLPOWER,	15, 5,  13, 2, 	-1, SANCT},
{IDS_LOCK, 							Stats::WILLPOWER,	20, 20, 0,	5, 	-1, MAKE_ITEM|FOCUS},
{IDS_KEY,							Stats::WILLPOWER,	20, 1,  0,	2, 	-1, SANCT|MAKE_ITEM|FOCUS},
{IDS_BREAK_LOCK, 					Stats::WILLPOWER,	40, 40, 0,	8, 	-1, SANCT|FOCUS},
{IDS_REPAIR, 						Stats::RESILIENCE,	15, 10, 0,	4, 	-1, SANCT|NEED_ITEM},
{IDS_REMOVE_CURSE,					Stats::RESILIENCE,	5,  15, 0,	2, 	-1, SANCT},
{IDS_HOLD_AVATAR,					Stats::LUCIDITY,	30, 20, 1,	3, 	-1, NEIGH|FOCUS},
{IDS_SANCTUARY,						Stats::DREAMSOUL,	25, 1,  25, 1, 	-1, SANCT},
{IDS_SHOVE, 						Stats::DREAMSOUL,	0,  0,  0,	1, 	-1, NEIGH},
{IDS_INSCRIBE,						Stats::DREAMSOUL,	20, 2,  0,	1, 	-1, SANCT|MAKE_ITEM},
{IDS_FORGE_MASTER,		 			Stats::NO_STAT,		50, 0,  0,	1, 	-1, SANCT},
{IDS_MERGE_TALISMAN,				Stats::INSIGHT,	    60, 40, 0,	2, 	-1, SANCT|NEED_ITEM|MAKE_ITEM|FOCUS},
{IDS_NP_SYMBOL_ART_NAME, 			Stats::NO_STAT,		10,  0, 0,	2, 	-1, SANCT},
{IDS_LOCATE_MARES,					Stats::INSIGHT,	    0,  1,  0,	1, 	-1, SANCT|LEARN},
{IDS_TEMPEST,				        Stats::LUCIDITY,	60, 40, 0,	7, 	-1, FOCUS|LEARN},
{IDS_KINESIS, 						Stats::WILLPOWER,	30, 5,  0,	1, 	-1, FOCUS|LEARN|NEIGH},
{IDS_MISDIRECTION,					Stats::DREAMSOUL,   60, 30, 0,  5,  -1, LEARN|NEIGH},
{IDS_CHAOTIC_VORTEX,				Stats::DREAMSOUL,   70, 40, 4,  5,  -1, NEIGH|NEED_ITEM},
{IDS_CHAOS_WELL,					Stats::DREAMSOUL,   30, 5,  0,  5,  -1, SANCT|MAKE_ITEM|LEARN},
{IDS_RALLY,							Stats::WILLPOWER,	60, 30, 0,  5,   4, SANCT|NEIGH|FOCUS},
};





int GsPlayer::MinOrbit(int art)
{
	return art_info[art].min_orbit;
}

int GsPlayer::FocusStat(int art)
{
	return art_info[art].stat;
}

bool GsPlayer::FocusOnly(int art)
{
	return art_info[art].restricted();
}
  
bool GsPlayer::Learnable(int art)
{
	return art_info[art].display_learnable();
}


short GsPlayer::PPMultiplier(int art_id)
{
	return art_info[art_id].pp_multiplier;;
}


////
// Constructor
////

GsPlayer::GsPlayer()
  : main_(0)
{
  lock_.Init();
  u_lock_.Init();
  n_lock_.Init();
  clear_information();
  is_mare_ = false;
  pp_evoking_ = Arts::NONE;
  pp_skill_ = 0;
}

////
// Destructor
////

GsPlayer::~GsPlayer()
{
  clear_information();
}

////
// Login - load from database; return 0 if ok, -1 otherwise
////

int GsPlayer::Login(lyra_id_t playerid, int pmare_type)
{
  DEFMETHOD(GsPlayer, Login);
  // load the player info from the player database
  int rc = main_->PlayerDBC()->LoadPlayer(playerid, db_, pmare_type);
  int sc = main_->PlayerDBC()->LastSQLCode();
  int lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::LoadPlayer took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not load player %u; rc=%d, sqlcode=%d"), method, playerid, rc, sc);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    return -1;
  }

//  db_.SetTCPOnly(tcp_only);

  // if not resuming a pmare session, billing type = mare avatar type
  if (pmare_type != Avatars::PMARE_RESUME) {
    db_.SetPMareBilling(pmare_type);
  }
    
  pmare_type_ = pmare_type;
  
  // load the inventory from the item database
  db_.Inventory().RemoveAll();
  rc = main_->ItemDBC()->GetPlayerInventory(db_.PlayerID(), db_.Inventory());
  sc = main_->ItemDBC()->LastSQLCode();
  lt = main_->ItemDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmItemDBC::GetPlayerInventory took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not load player %u inventory; rc=%d, sqlcode=%d"), method, playerid, rc, sc);
    GsUtil::HandleItemError(main_, method, rc, sc);
    return -1;
  }
  // check the avatar
  int av_rc = db_.FixAvatar(true);
  if (av_rc != 0) {
    main_->Log()->Warning(_T("%s: illegal player %u avatar; reason=%d"), method, playerid, av_rc);
  }

  // TEMP: destroy items that have obsolete bitmaps/missiles
  {
	  std::list<LmItem> to_destroy;
    for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
      LmItem item = db_.Inventory().ItemByIndex(i);
      // bitmap 240-244 are gone
      if ((item.Graphic() >= 240) && (item.Graphic() <= 244)) {
	to_destroy.push_back(item);
	continue;
      }
    }

    if (to_destroy.size() > 0) {
      //      main_->Log()->Debug(_T("%s: items to be destroyed: %d"), method, to_destroy.size());
		for (std::list<LmItem>::iterator pitem = to_destroy.begin(); (bool)!(pitem == to_destroy.end()); ++pitem) {
	LmItem item = *pitem;
	main_->ItemDBC()->DeleteItem(item.Serial());
	db_.Inventory().RemoveItem(item.Header());
      }
    }
  }


  // log player in
  rc = main_->PlayerDBC()->Login(playerid, pmare_type, db_.PMareBilling(), 
	  main_->HostIP(), main_->ServerPort());
  sc = main_->PlayerDBC()->LastSQLCode();
  lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::Login took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not log player %u in; rc=%d, sqlcode=%d"), method, playerid, rc, sc);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    return -1;
  }
  // update login time
  db_.SetLastLogin(time(NULL));
  // set current avatar from database
  avatar_ = db_.Avatar();

  // set login time
  time(&login_time_);
  
  return 0;
}

////
// Logout
////

void GsPlayer::Logout(bool save)
{
  DEFMETHOD(GsPlayer, Logout);
  LmLocker mon(lock_); // lock object during method duration

  // pmares: count time online, generate bill!
  time_t logout_time;
  time(&logout_time);

  unsigned int num_seconds_online = (unsigned int)(logout_time - login_time_);
  int rc, sc = 0, lt;

  if (db_.AccountType() == LmPlayerDB::ACCT_PMARE) {
    rc = main_->BillingDBC()->LogoutPMare(db_.PlayerID(), num_seconds_online, db_.PMareBilling(), db_.BillingID());
    if (rc < 1) {
      main_->Log()->Error(_T("%s: could not properly logout pmare %u, %u seconds online; rc=%d"), method, db_.PlayerID(), rc, num_seconds_online);
	  GsUtil::HandlePlayerError(main_, method, rc, sc, false);
	  main_->Log()->Debug(_T("%s: PMare %s logged out; %u seconds online"), method, db_.PlayerName(), num_seconds_online);
	}
  }

  int online = 0;
  // save?
  if (save) {
    online = LmUtil::TimeSince(db_.LastLogin());

    // save to database
    if (save_to_db(true) < 0) {
      main_->Log()->Error(_T("%s: could not store player information to database"), method);
	  // save to local file instead
	  if (save_to_file() < 0) {
	    main_->Log()->Error(_T("%s: could not store player information to local file"), method);
	  }
	}
  }
  // log player out
  rc = main_->PlayerDBC()->Logout(db_.PlayerID(), online);
  sc = main_->PlayerDBC()->LastSQLCode();
  lt = main_->PlayerDBC()->LastCallTime();
  //  main_->Log()->Debug(_T("%s: LmPlayerDBC::Logout for player %u took %d ms"), method, db_.PlayerID(), lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not log player out; rc=%d, sqlcode=%d"), method, rc, sc);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
  }

  //  main_->Log()->Debug(_T("%s: Database updated, local file removed for player %u"), method, db_.PlayerID());
  clear_information();
  //  main_->Log()->Debug(_T("%s: Cleared information"), method);
}

////
// SaveToFile
////

int GsPlayer::SaveToFile() const
{
  LmLocker mon(lock_); // lock object during method duration
  return save_to_file(); // call non-locking method that does the work
}

////
// SaveToDB
////

int GsPlayer::SaveToDB(bool force)
{
  LmLocker mon(lock_); // lock object during method duration
  //return save_to_db(foce); // call non-locking method that does the work
  return save_to_db(force); // call non-locking method that does the work
}

////
// Init
////

void GsPlayer::Init(LmConnection* conn, int serv_port, LmLog* log, bool firewall, short tcp_only)
{
  LmLocker mon(lock_); // lock object during method duration
  conn_ = conn;
  log_ = log;
  firewall_ = firewall;
  tcp_only_ = tcp_only;
  // determine client IP address, and UDP update address
  if (conn->Socket().SockType() == LmSockType::Inet_Stream()) {
    client_addr_ = conn->Socket().PeerName(); // this is guaranteed to be valid
    update_addr_.Init(client_addr_.IPAddress(), serv_port);
  }
  in_use_ = true;
}


////
// InUse - return "in use" flag
////

bool GsPlayer::InUse() const
{
  LmLocker mon(lock_); // lock object during method duration
  return in_use_;
}

////
// IsHidden
////

bool GsPlayer::IsHidden() const
{
  LmLocker mon(lock_); // lock object during method duration
  return hidden_;
}

////
// SetHidden
////

void GsPlayer::SetHidden(bool hidden)
{
  LmLocker mon(lock_); // lock object during method duration
  hidden_ = hidden;
}

////
// IsBeingSummoned
////

bool GsPlayer::BeingSummoned() const
{
	LmLocker mon(lock_); // lock object during method duration
	return being_summoned_;
}

////
// SetBeingSummoned
////

void GsPlayer::SetBeingSummoned(bool being_summoned)
{
	LmLocker mon(lock_); // lock object during method duration
	being_summoned_ = being_summoned;
}

////
// Online - return seconds online
////

int GsPlayer::Online() const
{
  LmLocker mon(lock_); // lock object during method duration
  return LmUtil::TimeSince(db_.LastLogin());
}

////
// IdleTime - return seconds of idle time
////

int GsPlayer::IdleTime()
{
  DEFMETHOD(GsPlayer, IdleTime);

  LmLocker mon(lock_); // lock object during method duration
  // if in level, timeout based on udp updates and tcp messages
  if (!conn_) { 
    // null connection could mean the player is in login, OR it could
    // mean they're ghosted. if we make an idle time check and the
    // connection is null, set idle_null_connection_ to true if it is false,
    // or return a huge idle value if it's true to force a timeout
    // and a reap of the connection
    //    if (idle_null_connection_) {
    //main_->Log()->Debug(_T("%s: player %u has idle null twice - ghosted?"), method, db_.PlayerID());
    //return (GsNetworkInput::PLAYER_TIMEOUT*10);
    //} else {
    //  main_->Log()->Debug(_T("%s: player %u has idle null once - ghosted?"), method, db_.PlayerID());
    //  idle_null_connection_ = true;
    //}

    main_->Log()->Debug(_T("%s: player %u has idle null - ghosted?"), method, db_.PlayerID());
    return (GsNetworkInput::PLAYER_TIMEOUT*10);
    } 

  if (in_level_) {
    LmLocker umon(u_lock_); // lock update time
    int udp_idle = LmUtil::TimeSince(last_update_);
    //    int tcp_idle = conn_ ? conn_->IdleIn() : 0;
    //return MIN(udp_idle, tcp_idle);
    return udp_idle;
  }
  // if not in level, timeout based on tcp messages
  return conn_->IdleIn();
}

void GsPlayer::SetUpdateAddress(unsigned long clientip, int servport) 
{
  DECLARE_TheLineNum;
  LmLocker mon(lock_); // lock object for method duration
  update_addr_.Init(clientip, servport);
}

////
// CheckAndReceiveUpdate - copy player update, set time; check if player has just attacked, and if so
//   perform detailed weapon check (missile, damage, velocity, effect); if these don't match
//   an item in the player's inventory, then zero out the attack bits in the update;
//   returns distance^2 from last update if ok, -1 if not, -2 if not and no logging is to be done;
//   returns 0 if player is admin/monster
////

int GsPlayer::CheckAndReceiveUpdate(LmPeerUpdate& update)
{
  LmLocker mon(u_lock_); // lock update members during method duration
  num_updates_++; // count it

  // save update, and set last update time to now
  update_ = update;
  last_update_ = time(NULL);

  db_.SetX(update.X());
  db_.SetY(update.Y());

  if (num_updates_%100 == 0) { // every 100th updates - check for timing hacks
	  if (last_mod_100_update_ > 0) {
		  // if we got 100 updates in under 10 seconds, 
		  // there's obviously a hack - it should take 20 seconds!
		  if ((last_update_ - last_mod_100_update_) < 10) {
			num_too_fast_updates_++;
			if (num_too_fast_updates_ == 50) {
				SECLOG(6, _T("Speed hacker! Player %d caused 50 potential cheats to be logged - logging his cheats turned off for the duration of the session!"),
					db_.PlayerID(), (last_update_ - last_mod_100_update_));
			} else if (num_too_fast_updates_ < 50)
				SECLOG(6, _T("Potential speed hack - Player %d sent 100 updates in %d seconds!"),
					db_.PlayerID(), (last_update_ - last_mod_100_update_));
			//if (num_too_fast_updates_ == 1)
				    // then boot the person from the game
			//		GsUtil::FakeLogout(main_, this);
		  }
	  }
	  
	  last_mod_100_update_ = time(NULL);
  }

  // compute distance since last update
  //main_->Log()->Debug(_T("Update for player %u : %d, %d \n"), db_.PlayerID(), update.X(), update_.Y());

  int dist = SQUARE(update.X() - update_.X()) + SQUARE(update.Y() - update_.Y());
  // admin/monster -- no checking or distance computation, no checking for firing or being hit
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || (db_.AccountType() == LmPlayerDB::ACCT_MONSTER)) {
    return 0;
  }
  // has player been hit?
  if (!been_hit_ && (update.HitBits() & 0x01)) {
    been_hit_ = true;
  }
  // if player isn't attacking, no checks need to be made
  if (update.AttackBits() == 0) {
    return dist;
  }
  // player has attacked
  has_fired_ = true;
  // if first attack bit is set, then do in-depth check of update
  if (update.AttackBits() & 0x1) {
    if (!check_update(update)) {
      update.SetAttackBits(0);
      weapon_bad_ = true;
      num_bad_++;
      return -1;
    }
    else {
      // check was ok, weapon is ok
      weapon_bad_ = false;
      return dist;
    }
  }
  // else if last check failed, clear attack bits and return -2
  if (weapon_bad_) {
    update.SetAttackBits(0);
    return -2;
  }
  // otherwise looks ok
  return dist;
}

////
// ReceivedUpdate - copy player update, set time (initializes update info)
////

void GsPlayer::ReceivedUpdate(const LmPeerUpdate& update)
{
  LmLocker mon(u_lock_); // lock update members during method duration
  update_ = update;
  last_update_ = time(NULL);
}

////
// ReceivedServerUpdate - copy neighbors
////

void GsPlayer::ReceivedServerUpdate(const RMsg_PlayerUpdate& player_update)
{
  LmLocker mon(n_lock_); // lock neighbor members during method duration
  num_neighbors_ = player_update.NumPlayers();
  for (int i = 0; i < num_neighbors_; ++i) {
    nb_[i] = player_update.PlayerID();
  }
}

////
// SaveReturnInfo
////

void GsPlayer::SaveReturnInfo()
{
  LmLocker mon(lock_); // lock object during method duration
  r_levelid_ = levelid_;
  r_roomid_ = roomid_;
}

////
// SaveRecallInfo
////

void GsPlayer::SaveRecallInfo()
{
  LmLocker mon(lock_); // lock object during method duration
  rc_levelid_ = levelid_;
  rc_roomid_ = roomid_;
}

////
// SaveGoalReturnInfo
////

void GsPlayer::SaveGoalReturnInfo()
{
  LmLocker mon(lock_); // lock object during method duration
  g_levelid_ = levelid_;
  g_roomid_ = roomid_;
}

////
// LevelID - return player's current level id
////

lyra_id_t GsPlayer::LevelID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return levelid_;
}

////
// LevelDB - return pointer to player's level
////

const LmLevelDBC* GsPlayer::LevelDBC() const
{
  LmLocker mon(lock_); // lock object during method duration
  return ldbc_;
}

////
// RoomID - return player's current room id
////

lyra_id_t GsPlayer::RoomID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return roomid_;
}

////
// ReturnLevelID
////

lyra_id_t GsPlayer::ReturnLevelID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return r_levelid_;
}

////
// ReturnRoomID
////

lyra_id_t GsPlayer::ReturnRoomID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return r_roomid_;
}

////
// GoalReturnLevelID
////

lyra_id_t GsPlayer::GoalReturnLevelID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return g_levelid_;
}

////
// GoalReturnRoomID
////

lyra_id_t GsPlayer::GoalReturnRoomID() const
{
  LmLocker mon(lock_); // lock object during method duration
  return g_roomid_;
}


struct teleport_spot {
	lyra_id_t level_id;
	lyra_id_t room_id;
};


teleport_spot always_legal_teleports[] = { 
	{ 43, 8 }, // anyone can goto the unknown - for summon
	{ 14, 12}, // anyone can go to gatekeeper focus guild
	{ 3,  29}, // anyone can go to dreamseer focus guild
	{ 7,  9}, // anyone can go to soulmaster focus guild
	{ 29, 21}, // anyone can go to lucidity focus guild
    { 39, 1 }, // anyone can go to boggen's lair - for expel
    { 42, 3 }, // anyone can be targetted by Chaos Purge
    { 46, 8 }, // anyone can be targetted by Cup Summons
	{ 46, 17}, // anyone can be auto-recalled in the Cup Arena
};

const int NUM_ALWAYS_LEGAL_TELEPORTS = sizeof(always_legal_teleports)/sizeof(teleport_spot);

////
// CanGotoLevel - return true if player can enter level/room via a return/recall/goalposting point
////

bool GsPlayer::CanGotoLevel(lyra_id_t levelid, lyra_id_t roomid) const
{
  LmLocker mon(lock_); // lock object during method duration
  // admins/monsters/player mares can go anywhere
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || 
      (db_.AccountType() == LmPlayerDB::ACCT_PMARE) ||
      (db_.AccountType() == LmPlayerDB::ACCT_MONSTER)) {
    return true;
  }

#ifdef CHINESE // no restrictions on teleporting due to restart at last location
  return true;
#endif 

  // some teleport spots are always allowed, for various reasons
  for (int i=0; i< NUM_ALWAYS_LEGAL_TELEPORTS; i++) {
	  if ((levelid == always_legal_teleports[i].level_id) &&
		  (roomid == always_legal_teleports[i].room_id))
		  return true;
  }


  // check return points
  if ((levelid == g_levelid_) && (roomid == g_roomid_)) {
    return true; // goalposting return
  }
  if ((levelid == r_levelid_) && (roomid == r_roomid_)) {
    return true; // RETURN point
  }
  if ((levelid == rc_levelid_) && (roomid == rc_roomid_)) {
    return true; // RECALL point
  }
  return false;
}

////
// CanGotoRoom - return true if player can go to room in current level (portal check)
////

bool GsPlayer::CanGotoRoom(lyra_id_t roomid) const
{
  LmLocker mon(lock_); // lock object during method duration
  // if not in level, no go
  if (!ldbc_) {
    return false;
  }
  // admins/monsters can go anywhere
  if ((db_.AccountType() == LmPlayerDB::ACCT_ADMIN) || 
      (db_.AccountType() == LmPlayerDB::ACCT_PMARE) ||
      (db_.AccountType() == LmPlayerDB::ACCT_MONSTER)) {
    return true;
  }


  // some teleport spots are always allowed, for various reasons
  for (int i=0; i< NUM_ALWAYS_LEGAL_TELEPORTS; i++) {
	  if ((levelid_ == always_legal_teleports[i].level_id) &&
		  (roomid == always_legal_teleports[i].room_id))
		  return true;
  }

  // can always go to same room on same level
  if (roomid == roomid_) {
    return true;
  }
  // is room a return/recall/goalpost point?
  if ((roomid == r_roomid_) || (roomid == rc_roomid_) || (roomid == g_roomid_)) {
    return true;
  }
  // is the player currently being rallied or summoned?
  if (being_summoned_) {
	  return true;
  }
  // check for portal to target room
  if (!ldbc_->RoomDB(roomid_).HasPortal(levelid_, roomid)) {
    return false;
  }
  // ok
  return true;
}

////
// SetInUse - set "in use" flag to given value
////

void GsPlayer::SetInUse(bool inuse)
{
  LmLocker mon(lock_); // lock object during method duration
  in_use_ = inuse;
}

////
// InLevel - return true if player is in a level (logged into a level server)
////

bool GsPlayer::InLevel() const
{
  LmLocker mon(lock_); // lock object during method duration
  return in_level_;
}

////
// SetInLevel - set "in level" flag to given value
////

void GsPlayer::SetInLevel(bool in_level)
{
  LmLocker mon(lock_); // lock object during method duration
  in_level_ = in_level;
  if (!in_level_) {
    lsconn_ = 0;
    ldbc_ = 0;
    // don't reset these, as we need to keep track of the last place the player was in, for portal checking
    // levelid_ = Lyra::ID_UNKNOWN;
    // roomid_ = Lyra::ID_UNKNOWN;
  }
}

////
// GotoLevel - update level connection, database and level/room id's when player goes to given level/room
////

void GsPlayer::GotoLevel(LmConnection* lsconn, const LmLevelDBC* leveldb, lyra_id_t roomid)
{
  LmLocker mon(lock_); // lock object during method duration
  ldbc_ = leveldb;
  if (ldbc_) {
    levelid_ = ldbc_->LevelID();
  }
  roomid_ = roomid;
  lsconn_ = lsconn;
  // determine level server IP address
  level_addr_.Init();
  if (lsconn_ && (lsconn_->Socket().SockType() == LmSockType::Inet_Stream())) {
    level_addr_ = lsconn_->Socket().PeerName();
  }
}

////
// GotoRoom
////

void GsPlayer::GotoRoom(lyra_id_t roomid)
{
  LmLocker mon(lock_); // lock object during method duration
  roomid_ = roomid;
}

////
// AddKill
////

void GsPlayer::AddKill(int orbit)
{
  LmLocker mon(lock_); // lock object during method duration
  num_kills_++;
  num_orbit_kills_ += orbit;
}

////
// AddDeath
////

void GsPlayer::AddDeath()
{
  LmLocker mon(lock_); // lock object during method duration
  num_deaths_++;
  num_recent_deaths_++;
}

////
// NumWeaponChecksFailed
////

int GsPlayer::NumWeaponChecksFailed() const
{
  LmLocker mon(lock_); // lock object during method duration
  return num_bad_;
}

////
// SetDescription
////

//void GsPlayer::SetDescription(const TCHAR* desc)
//{
//  LmLocker mon(lock_); // lock object during method duration
// _tcsnccpy(desc_, desc, sizeof(desc_));
//}

////
// Dump
////

void GsPlayer::Dump(FILE* f, int indent) const
{
  LmLocker mon(lock_); // lock object during method duration
  INDENT(indent, f);
 _ftprintf(f, _T("<GsPlayer[%p,%d]: inuse=%d conn=[%p] updates=%d lastupdate=%d (%d)\n"), this, sizeof(GsPlayer),
	  in_use_, conn_, num_updates_, last_update_, LmUtil::TimeSince(last_update_));
  INDENT(indent + 1, f);
 _ftprintf(f, _T("inlevel=%d lsconn=[%p] leveldb=[%p] levelid=%u roomid=%u\n"),
	  in_level_, lsconn_, ldbc_, levelid_, roomid_);
  INDENT(indent + 1, f);
 _ftprintf(f, _T("return=(%u,%u) goalreturn=(%u,%u) neighbors=%d items=(drop:%d pickup:%d take:%d)\n"),
	  r_levelid_, r_roomid_, g_levelid_, g_roomid_, num_neighbors_,
	  d_items_.size(), p_items_.size(), t_items_.size());
  INDENT(indent + 1, f);
 _ftprintf(f, _T("hit=%d fired=%d kills=%d deaths=%d check_failed=%d hidden=%d\n"),
	  been_hit_, has_fired_, num_kills_, num_deaths_, num_bad_, hidden_);
  INDENT(indent + 1, f);
  // _ftprintf(f, _T("desc='%s'\n"), desc_);
  db_.Dump(f, indent + 1);
}

////
// clear_information - initialize to unused state; assumes lock is held
////

void GsPlayer::clear_information()
{
  log_ = 0;

  in_use_ = false;

  db_.Init();
  avatar_.Init(0, 0);
  // _tcsnccpy(desc_, _T("none"), sizeof(desc_));

  num_kills_ = 0;
  num_deaths_ = 0;
  num_orbit_kills_ = 0;
  num_recent_deaths_ = 0;
  has_fired_ = false;
  been_hit_ = false;
  ds_decreased_ = false;
  hidden_ = false;
  being_summoned_ = false;

  in_level_ = false;
  ldbc_ = 0;
  levelid_ = 0;
  roomid_ = 0;

  r_levelid_ = 0;
  r_roomid_ = 0;

  rc_levelid_ = 0;
  rc_roomid_ = 0;

  g_levelid_ = 0;
  g_roomid_ = 0;

  if (d_items_.size() > 0) {
    d_items_.erase(d_items_.begin(), d_items_.end());
  }
  if (p_items_.size() > 0) {
    p_items_.erase(p_items_.begin(), p_items_.end());
  }

  if (t_items_.size() > 0) {
    t_items_.erase(t_items_.begin(), t_items_.end());
  }

  conn_ = 0;
  client_addr_.Init();
  update_addr_.Init();

  lsconn_ = 0;
  level_addr_.Init();

  update_.Init(0, 0, 0, 0, 0, 0);
  last_update_ = 0;
  idle_null_connection_ = false;
  weapon_bad_ = false;
  num_bad_ = 0;
  num_updates_ = 0;
  last_mod_100_update_ = 0;
  num_too_fast_updates_ = 0;

  num_neighbors_ = 0;
}

////
// has_neighbor - return true if given player is in neighbors
////

bool GsPlayer::has_neighbor(lyra_id_t playerid) const
{
  LmLocker mon(n_lock_); // lock neighbor members during method duration
  for (int i = 0; i < num_neighbors_; ++i) {
    if (nb_[i] == playerid) {
      return true;
    }
  }
  return false;
}

////
// neighbor_update - return neighbor update for given player (results undefined if
//   player isn't in neighbor list, will return first one)
// DEPRECATED 8/03 BMP
////

//const LmPeerUpdate& GsPlayer::neighbor_update(lyra_id_t playerid) const
//{
//  LmLocker mon(n_lock_); // lock neighbor members during method duration
//  for (int i = 0; i < num_neighbors_; ++i) {
    //if (nb_[i] == playerid) {
//      return nb_[i];
    //}
  //}
  //return nb_[0];
//}

////
// potential_inventory_size - return size of inventory if all current pickups and takes, succeed,
//   and all current drops fail; assumes lock is held
////

int GsPlayer::potential_inventory_size() const
{
  return db_.Inventory().NumItems() + d_items_.size() + p_items_.size() + t_items_.size();
}

////
// check_update - return true if peer update is allowed, false if not
//   - check that the weapon being used is valid
////

static const int dreamblade_damage[10] = {
  14,
  30,
  52,
  18,
  25,
  44,
  46, 
  60,
  39,
  38
};

bool GsPlayer::check_update(LmPeerUpdate& update)
{
  DEFMETHOD(GsPlayer, check_update);
  // Note: this is only called if player is a regular player, so no admin/monster checking done
  // get relevant fields from update
  int bitmap = update.WeaponBitmap();
  int velocity = update.WeaponVelocity();
  int damage = update.WeaponDamage();
  int effect = update.WeaponEffect();

#ifdef Ul3D 
  return true;
#else

  // check for dreamblade
  if (bitmap == LyraBitmap::DREAMBLADE_MISSILE) {
    // check dreamblade velocity, effect
//    if ((velocity != MELEE_VELOCITY) || (effect != LyraEffect::NONE)) {
    if (velocity != MELEE_VELOCITY) {
      PDEBUG((_T("%s: illegal dreamblade; vel=%d dmg=%d fx=%d"), method, velocity, damage, effect));
      return false;
    }

	return true; 

	// this code is deprecated because the DREAMBLADE_MISSILE bitmap
	// is also used for forged and gen'd blades

#if 0
    // get their skill in their focus-specific melee art
    int melee_skill = 0;
    switch (db_.Stats().FocusStat()) {
    case Stats::WILLPOWER:   melee_skill = db_.Arts().Skill(Arts::GATESMASHER);  break;
    case Stats::INSIGHT:     melee_skill = db_.Arts().Skill(Arts::DREAMBLADE);   break;
    case Stats::RESILIENCE:  melee_skill = db_.Arts().Skill(Arts::SOULREAPER);   break;
    case Stats::LUCIDITY:    melee_skill = db_.Arts().Skill(Arts::FATESLAYER);   break;
    default:
      PDEBUG((_T("%s: no focus stat?"), method));
      return false;
    }
    // check damage
    if (damage != (dreamblade_damage[melee_skill / 10])) {
      PDEBUG((_T("%s: illegal dreamblade; vel=%d dmg=%d (skill=%d) fx=%d"), method, velocity, damage, melee_skill, effect));
      return false;
    }
    // looks ok
    return true;
#endif
  }


  // check for player in nightmare form, or as pmares
  switch (avatar_.AvatarType()) {
  case Avatars::EMPHANT:
    if ((velocity != MELEE_VELOCITY) || (damage != EMPHANT_DAMAGE) ||
	(effect != LyraEffect::NONE) || (bitmap != LyraBitmap::MARE_MELEE_MISSILE)) {
      PDEBUG((_T("%s: illegal emphant weapon; vel=%d dmg=%d fx=%d bmp=%d"), method, velocity, damage, effect, bitmap));
      return false;
    }
    return true;
  case Avatars::BOGROM:
    if ((velocity != MELEE_VELOCITY) || (damage != BOGROM_DAMAGE) ||
	(effect != LyraEffect::NONE) || (bitmap != LyraBitmap::MARE_MELEE_MISSILE)) {
      PDEBUG((_T("%s: illegal bogrom weapon; vel=%d dmg=%d fx=%d bmp=%d"), method, velocity, damage, effect, bitmap));
      return false;
    }
    return true;
  case Avatars::AGOKNIGHT:
    if ((velocity != MELEE_VELOCITY) || (damage != AGOKNIGHT_DAMAGE) ||
	(effect != LyraEffect::NONE) || (bitmap != LyraBitmap::MARE_MELEE_MISSILE)) {
      PDEBUG((_T("%s: illegal agoknight weapon; vel=%d dmg=%d fx=%d bmp=%d"), method, velocity, damage, effect, bitmap));
      return false;
    }
    return true;
  case Avatars::SHAMBLIX:
    if ((velocity != 3) || (damage != SHAMBLIX_DAMAGE) ||
	(effect != LyraEffect::PLAYER_PARALYZED) || (bitmap != LyraBitmap::FIREBALL_MISSILE)) {
      PDEBUG((_T("%s: illegal shamblix weapon; vel=%d dmg=%d fx=%d bmp=%d"), method, velocity, damage, effect, bitmap));
      return false;
    }
    return true;
  case Avatars::HORRON:
    if ((velocity != -5) || (damage != HORRON_DAMAGE) ||
	(effect != LyraEffect::PLAYER_BLIND) || (bitmap != LyraBitmap::FIREBALL_MISSILE)) {
      PDEBUG((_T("%s: illegal horron weapon; vel=%d dmg=%d fx=%d bmp=%d"), method, velocity, damage, effect, bitmap));
      return false;
    }
    return true;
  default:
    break;
  }
  
  // check for push
  if (bitmap == LyraBitmap::PUSH_MISSILE) {
    if ((velocity != MELEE_VELOCITY) || (damage != 0) || (effect != LyraEffect::NONE)) {
      PDEBUG((_T("%s: illegal push; vel=%d dmg=%d fx=%d"), method, velocity, damage, effect));
      return false;
    }
    // looks ok
    return true;
  }

  // check for focus-specific missile weapon
  if ((bitmap == LyraBitmap::FIREBALL_MISSILE) && (velocity == ART_MISSILE_VELOCITY) && (effect == LyraEffect::NONE)) {
    // might be a builtin missile attack; get skill level of relevant art
    int missile_skill = 0;
    switch (db_.Stats().FocusStat()) {
    case Stats::WILLPOWER:   missile_skill = db_.Arts().Skill(Arts::FLAMESHAFT);   break;
    case Stats::INSIGHT:     missile_skill = db_.Arts().Skill(Arts::TRANCEFLAME);  break;
    case Stats::RESILIENCE:  missile_skill = db_.Arts().Skill(Arts::FLAMESEAR);    break;
    case Stats::LUCIDITY:    missile_skill = db_.Arts().Skill(Arts::FLAMERUIN);    break;
    default:
      PDEBUG((_T("%s: no focus stat?"), method));
      return false;
    }


    int evoke_id = this->PPEvoking();
	if ((evoke_id == Arts::FLAMESHAFT) || (evoke_id == Arts::TRANCEFLAME) ||
		(evoke_id == Arts::FLAMESEAR) || (evoke_id == Arts::FLAMERUIN)) {
		 // we spent pp's to evoke this art once
			missile_skill = pp_skill_;
			pp_evoking_ = Arts::NONE;
			pp_skill_ = 0;
		}
		

    // check if damage matches
    if (damage == (dreamblade_damage[missile_skill / 10])) {
      return true; // found it
    }
    // NOTE: we don't return false, since this attack could be a missile item
  }

  // check if player has a weapon that has the given attributes
  if (db_.Inventory().HasWeapon(bitmap, damage, velocity, effect)) {
    return true;
  }

  // didn't find one
  PDEBUG((_T("%s: missile weapon not found for: bm=%d vel=%d dmg=%d fx=%d"), method, bitmap, velocity, damage, effect));
  return false;

#endif // Ul3D
}




////
// max_skill - return max skill level player can have in the given art
//   (checks based on orbit, focus stat)
////

int GsPlayer::max_skill(int art_num) const
{
  // find entry for art
  //int i = //find_art_entry(art_num);
	int i = art_num;
  if (i == -1) {
    return 0; // not found, can't learn it
  }
  // initial max is same as player's orbit
  int the_max = db_.Stats().Orbit();
  // if orbit is < 20, limit is 19, otherwise it is the same as the player's orbit
  if (db_.Stats().Orbit() < 20) {
   the_max = 19;
  }

  // if the art has no focus stat, then the limit is 99 for anyone
  if ((art_info[i].stat == Stats::NO_STAT) ||
      (art_info[i].stat == Stats::DREAMSOUL)) {
    the_max = MIN(the_max, 99);
    return the_max;
  }

  // if player's focus stat matches the art's focus stat, no limit (99)
  if (db_.Stats().FocusStat() == art_info[i].stat) {
    the_max = MIN(the_max, 99);
    return the_max;
  }

  // otherwise, focus stats do not match, so limit is below 3rd plateau (29),
  // and for non-focus arts can be learned only to 2 spheres less than
  // the player's sphere

  // player's orbit high enough?  (must be 2 spheres above art's sphere)
  int sphere = db_.Stats().Sphere();
  int min_sphere = art_info[i].min_orbit/10;
  min_sphere += 2;
  int sphere_diff = sphere - min_sphere;
  if (sphere_diff < 0)
    sphere_diff = 0;
  int art_max = sphere_diff*10 + 9;

  the_max = MIN(the_max, art_max);

  the_max = MIN(the_max, 29);

  return the_max;


}

////
// can_learn - return true if player can learn art (based on current orbit and focus)
////

bool GsPlayer::can_learn(int art_num) const
{
  // find entry for art
  //int i = find_art_entry(art_num);
	int i = art_num;
  if (i == -1) {
    return false; // not found, can't learn it
  }
  // if the art has no focus stat, then depends on orbit
  if ((art_info[i].stat == Stats::NO_STAT) ||
      (art_info[i].stat == Stats::DREAMSOUL)) {
    if (db_.Stats().Orbit() < art_info[i].min_orbit) {
      return false;
    }
    // ok
    return true;
  }
  // focus stat match?
  if (db_.Stats().FocusStat() == art_info[i].stat) {
    // orbit high enough?
    if (db_.Stats().Orbit() < art_info[i].min_orbit) {
      return false;
    }
    // ok
    return true;
  }
  // focus stat doesn't match, and art does have a focus stat
  // is focus stat required to learn?
  if (art_info[i].restricted()) {
    return false;
  }
  // player's orbit high enough?  (must be 2 spheres above art's sphere)
  if (db_.Stats().Sphere() < ((art_info[i].min_orbit / 10) + 2)) {
    return false;
  }
  // ok
  return true;
}

////
// save_to_file - save player db to local file, non-locking
////

int GsPlayer::save_to_file() const
{

  DEFMETHOD(GsPlayer, save_to_file);

  // if player is an agent, don't do squat
  if (db_.AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return 0;
  }
  TCHAR pfile_tmp[FILENAME_MAX];
  TCHAR pfile[FILENAME_MAX];
  main_->GlobalDB()->GetPlayerFile(pfile, db_.PlayerID());
 _stprintf(pfile_tmp, _T("%s.tmp"), pfile);
  // write to temporary file, and if save succeeds rename it
  LmDatabase db;
  if (db.Open(pfile_tmp, GDBM_NEWDB) < 0) {
    main_->Log()->Error(_T("%s: could not open '%s' for player database"), method, pfile_tmp);
    return -1;
  }
    if (LmPlayerDBF::SaveToFile(db, db_) < 0) {
      main_->Log()->Error(_T("%s: could not save player database"), method);
      db.Close();
      _tunlink(pfile_tmp);
      return -1;
    }
  db.Close();
  // rename temp file
  if (_trename(pfile_tmp, pfile) < 0) {
    main_->Log()->Error(_T("%s: could not rename '%s' to '%s'"), method, pfile_tmp, pfile);
    _tunlink(pfile_tmp);
    return -1;
  }
  main_->Log()->Debug(_T("%s: saved player %u info to disk"), method, db_.PlayerID());

  return 0;
}

////
// save_to_db - save player db to db, non-locking
////

int GsPlayer::save_to_db(bool force)
{
  DEFMETHOD(GsPlayer, save_to_db);
  // if player is an agent, don't do squat
  if (db_.AccountType() == LmPlayerDB::ACCT_MONSTER) {
    return 0;
  }
  int retval = 0;
  // save to database
  db_.SetLevelID(this->LevelID());
  int rc = main_->PlayerDBC()->SavePlayer(db_, true);
  int sc = main_->PlayerDBC()->LastSQLCode();
  int lt = main_->PlayerDBC()->LastCallTime();
  //main_->Log()->Debug(_T("%s: LmPlayerDBC::SavePlayer took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not save player; rc=%d, sqlcode=%d"), method, rc, sc);
    GsUtil::HandlePlayerError(main_, method, rc, sc, false);
    retval = -1;
  }
  // store inventory
  rc = main_->ItemDBC()->SavePlayerInventory(db_.PlayerID(), ((class LmInventory&)db_.Inventory()));
  sc = main_->ItemDBC()->LastSQLCode();
  lt = main_->ItemDBC()->LastCallTime();
  main_->Log()->Debug(_T("%s: LmItemDBC::SavePlayerInventory took %d ms"), method, lt);
  if (rc < 0) {
    main_->Log()->Error(_T("%s: could not store inventory; rc=%d, sqlcode=%d"), method, rc, sc);
    GsUtil::HandleItemError(main_, method, rc, sc);
    retval = -1;
  } 
  // unlink state file
  TCHAR pfile[FILENAME_MAX];
  main_->GlobalDB()->GetPlayerFile(pfile, db_.PlayerID());
  _tunlink(pfile);

  return retval;
}


////
// CanSelfTrain - return true if player can self train in the given art
//    this needs to be in this file so it can access the arts table
////

bool GsPlayer::CanSelfTrain(int art, TCHAR* names_buffer) 
{
  DEFMETHOD(GsPlayer, CanSelfTrain);
  LmLocker mon(lock_); // lock object during method duration
  // must have art ready to be trained

	if ((art == Arts::NONE) || (art > NUM_ARTS)) {
		main_->Log()->Debug(_T("%s: player %u: self training with illegal art number %d"), method, db_.PlayerID(), art);
		return false;
	}

  // _stprintf(names_buffer, "Supporters: ");

  // can't self train focus restricted
  if ((art_info[art].restricted()) &&
      (art_info[art].stat != db_.Stats().FocusStat())) {
    return false;
  }

  // can't self train minor arts above 20 
  if ((art_info[art].stat != db_.Stats().FocusStat()) &&
      (art_info[art].stat != Stats::DREAMSOUL) &&
      (art_info[art].stat != Stats::NO_STAT) &&
      (db_.Arts().Skill(art) > 19))
    return false;

  // can't self train above own orbit
  if ((db_.Arts().Skill(art)+1) > db_.Stats().Orbit())
    return false;

  int num_required_tokens = (int)((db_.Arts().Skill(art)+1)/10);
  if (num_required_tokens < 3)
    num_required_tokens = 3;

  // must have TRAIN_SELF skill
  if (db_.Arts().Skill(Arts::TRAIN_SELF) < 1) {
    return false;
  }
  // must have art maxed
  if ((db_.Arts().Skill(art)%10) != 9) {
    return false;
  }

  /* new restriction: can only plat one level above skill (Arts::TRAIN) */
  if ((db_.Arts ().Skill (Arts::TRAIN) / 10) < (db_.Arts ().Skill (art) / 10))
    return false;

  // must have enough unique ascension tokens
  int have_tokens = 0;
  std::list<lyra_id_t> creators;
  for (int i = 0; i < db_.Inventory().NumItems(); ++i) {
    const LmItem& item = db_.Inventory().ItemByIndex(i);
    // check item type; ascension tokens only have a single function
    if (LyraItem::StateFunction(item.StateField(0)) == LyraItem::SUPPORT_TRAIN_FUNCTION) {
      lyra_item_train_support_t support;
      memcpy(&support, item.StateField(0), sizeof(support));
      if ((support.art_id == art) &&
	//  ((int)(support.art_level/10) >= (int)(db_.Arts().Skill(art)/10)) && // no longer relevant.
	  (support.target_id() == db_.PlayerID())) {
	have_tokens++;
	creators.push_back(support.creator_id());
	// log use (disabled due to inability to store full player id)
	//_tcscat(names_buffer, main_->PlayerNameMap()->PlayerName((lyra_id_t)support.creator_id()));
      }
    }
  }


  // have enough tokens?
  if (have_tokens < num_required_tokens) {
	main_->Log()->Debug(_T("%s: self training failed; player has only %d of %d tokens"), method, have_tokens, num_required_tokens);
    return false;
  }
  // and, each one must come from a different person
  creators.sort();
  creators.unique();
  if (creators.size() < num_required_tokens) {
	main_->Log()->Debug(_T("%s: self training failed; player has only %d of %d UNIQUE tokens"), method, have_tokens, num_required_tokens);
    return false;
  }
  // ok - log use

  return true;
}
