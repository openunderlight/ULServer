// LsComputeGroups.cpp  -*- C++ -*-
// $Id: LsComputeGroups.cpp,v 1.15 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation: note that this code is not MT-safe, and should only
// be called by one thread at any time!

#include <assert.h>
#include <stdlib.h>
#include <sys/types.h>
//#include <algo.h>
#include <algorithm>
#include "LsComputeGroups.h"
#include "../../libsrc/shared/LyraDefs.h"
#include "LsMain.h"
#include "LsRoomState.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "LmLogFile.h"
#include "LmRoomDB.h"
#include "LsPlayerList.h"

DECLARE_TheFileName;

////
// file-scope types, objects, functions
////

// distance between two players, plus pointers to their information
struct cg_dist_t {
  LsPlayer* p1;
  LsPlayer* p2;
  int distance;
};

// distance computation scratch space (this is larger than necessary to handle
// a potential situation where a room is "over full")
static cg_dist_t dist[Lyra::ROOMPEOPLE_MAX * Lyra::ROOMPEOPLE_MAX];

// functions
static int cg_dist_t_cmp(const void *d1, const void *d2);

////
// Inlined local functions
////

// distance equality operator
inline int operator==(const cg_dist_t& left, const cg_dist_t& right)
{
  return left.distance == right.distance;
}

// distance less-than operator
inline int operator<(const cg_dist_t& left, const cg_dist_t& right)
{
  return left.distance < right.distance;
}

// player_distance - return distance between two players (actually, it is the X/Y distance squared)
inline static int player_distance(LsPlayer* p1, LsPlayer* p2)
{
  return (SQUARE(p1->Position().X() - p2->Position().X()) + 
	  SQUARE(p1->Position().Y() - p2->Position().Y()));
}

// add_to_groups - add to local groups
inline static void add_to_groups(LsPlayer* p1, LsPlayer* p2)
{
  // avoid duplicate adds
  if (!p1->LocalGroup().HasMember(p2->PlayerID())) {
    p1->AddGroupMember(p2->PlayerID());
    p2->AddGroupMember(p1->PlayerID());
  }
}

// add_to_neighbors
inline static void add_to_neighbors(LsPlayer* p1, LsPlayer* p2)
{
  // only add if p2 is not in p1's local group
  if (!p1->LocalGroup().HasMember(p2->PlayerID())) {
    p1->AddNeighbor(p2->PlayerID());
  }
}

////
// ComputeGroups
////

// Algorithm:
//
// Compute all inter-player distances.
// Sort in increasing order.
// Starting with smallest distance, add players to local groups
// unless full (then add to neighbors).
// This will put the players closest to one another into each
// other's groups, if at all possible.

void ComputeGroups(LsMain* main, LsRoomState* room)
{
  DECLARE_TheLineNum;
  // set dist to zero on first run of this function
  static bool dist_cleared = false;
  if (!dist_cleared) {
    memset(&dist, 0, sizeof(dist));
    dist_cleared = true;
  }

  std::list<lyra_id_t> pids = room->PlayerList();
  // quick return for rooms with 0 players
  if (pids.size() == 0) {
    return;
  }
  // get player objects
  LsPlayerList players;
  main->PlayerSet()->GetPlayerList(players, room->DB()->RoomID());

  // wipe out everybody's group information and copy party information into local group
  for (LsPlayerList::iterator p = players.begin(); !(bool)(p == players.end()); ++p) {
    (*p)->ClearGroupsAndCopyParty();
  }

  // if there's only one player, then we don't need to do anything
  // (can't do this before clearing groups)
  if (players.size() == 1) {
    return;
  }

  // compute/initialize distances
  int num_dist = 0;
  LsPlayerList::iterator p1, p2;
  for (p1 = players.begin(); !(bool)(p1 == players.end()); ++p1) {
    for (p2 = players.begin(); !(bool)(p2 == players.end()); ++p2) {
      // check for two different players
      LsPlayer* P1 = *p1;
      LsPlayer* P2 = *p2;
      if (P1->PlayerID() != P2->PlayerID()) {
	dist[num_dist].p1 = P1;
	dist[num_dist].p2 = P2;;
	dist[num_dist].distance = player_distance(P1, P2);
	num_dist++;
      }
    }
  }

  // one person in room?
  if (num_dist == 0) {
    return;
  }

  // sort by distance
  //  qsort(&dist, (size_t) num_dist, sizeof(cg_dist_t), cg_dist_t_cmp);

  // STL sort routine
  std::sort(dist, dist + num_dist);

  // add to groups
  for (int j = 0; j < num_dist; ++j) {
    LsPlayer* pp1 = dist[j].p1;
    LsPlayer* pp2 = dist[j].p2;
    if (pp1->LocalGroup().IsFull() || pp2->LocalGroup().IsFull()) {
      // one of the two groups are full, add to neighbors
      add_to_neighbors(pp1, pp2);
    }
    else {
      // add to each other's local group
      add_to_groups(pp1, pp2);
    }
  }
}

////
// local functions
////

// cg_dist_t_cmp - compare two distances (for sorting purposes)
static int cg_dist_t_cmp(const void *d1, const void *d2)
{
  return ((cg_dist_t*) d1)->distance - ((cg_dist_t*) d2)->distance;
}
