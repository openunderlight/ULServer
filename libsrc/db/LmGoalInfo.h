// LmGoalInfo.h  -*- C++ -*-
// $Id: LmGoalInfo.h,v 1.3 1997-11-20 16:26:34-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// goal information structure

#ifndef INCLUDED_LmGoalInfo
#define INCLUDED_LmGoalInfo

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"

// forward declarations

// base datatype

struct lyra_goalinfo_t {
  lyra_id_t goalid;
  int creator;
  int guild;
  int rank;
  int maxaccepts;
  int expirationtime;
  int numberyes;
  int numberno;
  int voteexpiration;
  int status;
  int numaccepted;
  int flags;
};

// the class

class LmGoalInfo {

public:

  LmGoalInfo();

  void Init();

  // selectors
  lyra_id_t GoalID() const;
  int CreatorID() const;
  int Guild() const;
  int Rank() const;
  int MaxAccepts() const;
  int ExpirationTime() const;
  int NumberYes() const;
  int NumberNo() const;
  int VoteExpiration() const;
  int Status() const;
  int NumAccepted() const;
  int Flags() const;

  bool IsGuardianManaged() const;

  // mutators
  void SetGoalID(lyra_id_t goalid);
  void SetCreatorID(int creator);
  void SetGuild(int guild);
  void SetRank(int rank);
  void SetMaxAccepts(int maxaccepts);
  void SetExpirationTime(int expirationtime);
  void SetNumberYes(int numberyes);
  void SetNumberNo(int numberno);
  void SetVoteExpiration(int voteexpiration);
  void SetStatus(int status);
  void SetNumAccepted(int numaccepted);
  void SetFlags(int flags);

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  lyra_goalinfo_t gi_;

};

#ifdef USE_INLINE
#include "LmGoalInfo.i"
#endif /* USE_INLINE */

#endif /* INCLUDED_LmGoalInfo */
