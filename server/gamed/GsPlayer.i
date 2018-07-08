// GsPlayer.i  -*- C++ -*-
// $Id: GsPlayer.i,v 1.24 1998-04-17 17:40:23-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE void GsPlayer::SetMain(GsMain* gsmain)
{
  main_ = gsmain;
}

INLINE lyra_id_t GsPlayer::PlayerID() const
{
  return db_.PlayerID();
}

INLINE const LmPlayerDB& GsPlayer::DB() const
{
  return db_;
}

INLINE LmConnection* GsPlayer::Connection() const
{
  return conn_;
}

INLINE LmConnection* GsPlayer::LevelConnection() const
{
  return lsconn_;
}

INLINE const LmSockAddrInet& GsPlayer::LevelAddress() const
{
  return level_addr_;
}

INLINE const LmSockAddrInet& GsPlayer::ClientAddress() const
{
  return client_addr_;
}

INLINE const LmSockAddrInet& GsPlayer::UpdateAddress() const
{
  return update_addr_;
}

INLINE const LmPeerUpdate& GsPlayer::PlayerUpdate() const
{
  return update_;
}

INLINE bool GsPlayer::NewlyNeedsAnnounce() 
{ 
  return newly_announce;
}

INLINE void GsPlayer::SetNewlyNeedsAnnounce(bool announce)
{
        newly_announce = announce;
}


INLINE time_t GsPlayer::LastUpdateTime() const
{
  return last_update_;
}

INLINE const LmAvatar& GsPlayer::Avatar() const
{
  return avatar_;
}


INLINE void GsPlayer::SetLog(LmLog* log)
{
  log_ = log;
}

INLINE int GsPlayer::NumKills() const
{
  return num_kills_;
}

INLINE int GsPlayer::NumRecentDeaths() const
{
  return num_recent_deaths_;
}

INLINE int GsPlayer::NumOrbitKills() const
{
  return num_orbit_kills_;
}

INLINE void GsPlayer::SetNumRecentDeaths(int value)
{
  num_recent_deaths_ = value;
}

INLINE void GsPlayer::SetNumOrbitKills(int value)
{
  num_orbit_kills_ = value;
}

INLINE int GsPlayer::NumDeaths() const
{
  return num_deaths_;
}

INLINE bool GsPlayer::HasFired() const
{
  return has_fired_;
}

INLINE int GsPlayer::PPEvoking() const
{
	return pp_evoking_;
}

INLINE void GsPlayer::SetPPEvoking(int value) 
{
	pp_evoking_ = value;
}

INLINE int GsPlayer::PPSkill() const
{
	return pp_skill_;
}

INLINE void GsPlayer::SetPPSkill(int value) 
{
	pp_skill_ = value;
}


INLINE bool GsPlayer::Firewall() const
{
  return firewall_;
}


INLINE bool GsPlayer::TCPOnly() const
{
        return tcp_only_;
}

INLINE bool GsPlayer::BeenHit() const
{
  return been_hit_;
}

INLINE bool GsPlayer::DreamsoulDecreased() const
{
  return ds_decreased_;
}

INLINE int GsPlayer::NumUpdates() const
{
  return num_updates_;
}

//INLINE const char* GsPlayer::Description() const
//{
//  return desc_;
//}
