// LsPlayer.i  -*- C++ -*-
// $Id: LsPlayer.i,v 1.15 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

// Note: these methods return immutable data, hence no locking

INLINE lyra_id_t LsPlayer::PlayerID() const
{
  return playerid_;
}

INLINE unsigned short LsPlayer::RealtimeID() const
{
  return realtimeid_;
}

INLINE void LsPlayer::SetRealtimeID(unsigned short realtimeid)
{
  realtimeid_ = realtimeid;
}

INLINE const TCHAR* LsPlayer::PlayerName() const
{
  return playername_;
}

INLINE int LsPlayer::AccountType() const
{
  return acct_type_;
}

INLINE LsUpdateSet* LsPlayer::UpdateSet() const
{
  return neighbor_updates_;
}

INLINE LmConnection* LsPlayer::Connection() const
{
  return conn_;
}

INLINE unsigned long LsPlayer::IPAddress() const
{
  return ipaddr_;
}

INLINE unsigned long LsPlayer::ClientIPAddress() const
{
  return client_ipaddr_;
}

INLINE const LmSockAddrInet& LsPlayer::UpdateAddress() const
{
  return update_addr_;
}

INLINE int LsPlayer::ServerPort() const
{
  return serv_port_;
}

INLINE const LmAvatar& LsPlayer::Avatar() const
{
  return avatar_;
}

INLINE time_t LsPlayer::LastPositionUpdate() const
{
  return last_pos_update_;
}

INLINE void LsPlayer::set_roomid(lyra_id_t roomid)
{
  roomid_ = roomid;
}

INLINE void LsPlayer::set_update(const LmPeerUpdate& update)
{
  pupdate_ = update;
  // pupdate2_ = update;
  //pupdate_.SetRealtimeID(realtime_id);
  pos_.Init(update.X(), update.Y(), 0, 0);
  last_pos_update_ = time(NULL);
}


INLINE void LsPlayer::set_party(const LmParty& party)
{
  party_ = party;
  party2_ = party;
}

INLINE const LmIdSet& LsPlayer::Joiners() const
{
  return joiners_;
}

INLINE const LsGiveTakeItemList& LsPlayer::GivingList() const
{
  return g_items_;
}

INLINE const LsGiveTakeItemList& LsPlayer::TakingList() const
{
  return t_items_;
}

INLINE const TCHAR* LsPlayer::Description() const
{
  return desc_;
}


