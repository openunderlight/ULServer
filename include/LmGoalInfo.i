// LmGoalInfo.i  -*- C++ -*-
// $Id: LmGoalInfo.i,v 1.3 1997-11-20 16:26:34-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE lyra_id_t LmGoalInfo::GoalID() const
{
  return gi_.goalid;
}

INLINE int LmGoalInfo::CreatorID() const
{
  return gi_.creator;
}

INLINE int LmGoalInfo::Guild() const
{
  return gi_.guild;
}

INLINE int LmGoalInfo::Rank() const
{
  return gi_.rank;
}

INLINE int LmGoalInfo::MaxAccepts() const
{
  return gi_.maxaccepts;
}

INLINE int LmGoalInfo::ExpirationTime() const
{
  return gi_.expirationtime;
}

INLINE int LmGoalInfo::NumberYes() const
{
  return gi_.numberyes;
}

INLINE int LmGoalInfo::NumberNo() const
{
  return gi_.numberno;
}

INLINE int LmGoalInfo::VoteExpiration() const
{
  return gi_.voteexpiration;
}

INLINE int LmGoalInfo::Status() const
{
  return gi_.status;
}

INLINE int LmGoalInfo::NumAccepted() const
{
  return gi_.numaccepted;
}

INLINE int LmGoalInfo::Flags() const
{
  return gi_.flags;
}

INLINE bool LmGoalInfo::IsGuardianManaged() const
{
  return (gi_.flags & 1) ? true : false;
}

INLINE void LmGoalInfo::SetGoalID(lyra_id_t goalid)
{
  gi_.goalid = goalid;
}

INLINE void LmGoalInfo::SetCreatorID(int creator)
{
  gi_.creator = creator;
}

INLINE void LmGoalInfo::SetGuild(int guild)
{
  gi_.guild = guild;
}

INLINE void LmGoalInfo::SetRank(int rank)
{
  gi_.rank = rank;
}

INLINE void LmGoalInfo::SetMaxAccepts(int maxaccepts)
{
  gi_.maxaccepts = maxaccepts;
}

INLINE void LmGoalInfo::SetExpirationTime(int expirationtime)
{
  gi_.expirationtime = expirationtime;
}

INLINE void LmGoalInfo::SetNumberYes(int numberyes)
{
  gi_.numberyes = numberyes;
}

INLINE void LmGoalInfo::SetNumberNo(int numberno)
{
  gi_.numberno = numberno;
}

INLINE void LmGoalInfo::SetVoteExpiration(int voteexpiration)
{
  gi_.voteexpiration = voteexpiration;
}

INLINE void LmGoalInfo::SetStatus(int status)
{
  gi_.status = status;
}

INLINE void LmGoalInfo::SetNumAccepted(int numaccepted)
{
  gi_.numaccepted = numaccepted;
}

INLINE void LmGoalInfo::SetFlags(int flags)
{
  gi_.flags = flags;
}
