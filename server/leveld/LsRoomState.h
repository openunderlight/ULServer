// LsRoomState.h  -*- C++ -*-
// $Id: LsRoomState.h,v 1.21 1997-09-23 02:33:26-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// dynamic room state class

#ifndef INCLUDED_LsRoomState
#define INCLUDED_LsRoomState

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"
#include "PThMutex.h"
#include "LmRoomItemList.h"

// forward declarations

class LmItemHdr;
class LmItem;
class LmRoomDB;
class LsMain;
class LsItemGen;
class LmRoomItem;
class LmDatabase;
class LsPlayer;

// the class

class LsRoomState {

public:

  LsRoomState();
  ~LsRoomState();

  void Init(LsMain* lsmain, const LmRoomDB* roomdb);

  int LoadFromDB();
  int SaveToDB();

  void LoadFromDisk(LmDatabase& db);
  void SaveToDisk(LmDatabase& db);

  const LmRoomDB* DB() const;

  bool HasPlayer(lyra_id_t playerid) const;
  void AddPlayer(LsPlayer* player);
  void RemovePlayer(lyra_id_t playerid);
  bool IsEmpty() const;
  bool IsFull() const;
  const std::list<lyra_id_t>& PlayerList() const;
  int NumPlayers() const;
  int NumPlayers(int ptype) const;

  bool HasItem(const LmItemHdr& itemhdr) const;
  const LmItemHdr* HasPrime(const lyra_id_t guild_id);
  void AddItem(const LmRoomItem& roomitem);
  void RemoveItem(const LmItemHdr& itemhdr);
  const LmItem& GetItem(const LmItemHdr& itemhdr) const;
  bool CanAddItem() const;
  const LmRoomItemList& ItemList() const;
  void MakeSpaceForItem(const LmItemHdr& itemhdr, LmRoomItem& reaped);
  int NumActiveRazorwindBlades();
  void ReapItems(LmRoomItemList& reaped);
  void GenerateItems(LmRoomItemList& generated);

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsRoomState(const LsRoomState&);
  //operator=(const LsRoomState&);

  void add_item(const LmRoomItem& roomitem);
  bool has_item(const LmItemHdr& itemhdr) const;
  const LmItemHdr* has_prime(const lyra_id_t guild_id);
  void remove_item(const LmItemHdr& itemhdr);
  bool can_add_item() const;

  // related objects
  LsMain* main_;
  const LmRoomDB* db_;

  PThMutex lock_;

  // room state
  std::list<lyra_id_t> players_;
  LmRoomItemList items_;
  LsItemGen* itemgens_;

};

#ifdef USE_INLINE
#include "LsRoomState.i"
#endif

#endif /* INCLUDED_LsRoomState */
