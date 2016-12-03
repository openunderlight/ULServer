// SMsg_LevelLogin.i  -*- C++ -*-
// $Id: SMsg_LevelLogin.i,v 1.11 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inlined methods/functions

INLINE lyra_id_t SMsg_LevelLogin::PlayerID() const
{
  return data_.playerid;
}

INLINE const TCHAR* SMsg_LevelLogin::PlayerName() const
{
  return data_.playername;
}

INLINE int SMsg_LevelLogin::AccountType() const
{
  return data_.acct_type;
}

INLINE unsigned long SMsg_LevelLogin::IPAddress() const
{
  return data_.ipaddr;
}

INLINE int SMsg_LevelLogin::ServerPort() const
{
  return data_.udp_portnum;
}

INLINE const LmAvatar& SMsg_LevelLogin::Avatar() const
{
  return data_.avatar;
}

INLINE lyra_id_t SMsg_LevelLogin::RoomID() const
{
  return data_.roomid;
}

INLINE const LmPeerUpdate& SMsg_LevelLogin::PeerUpdate() const
{
  return data_.update;
}


INLINE unsigned char SMsg_LevelLogin::Hidden() const
{
  return data_.hidden;
}

INLINE unsigned char SMsg_LevelLogin::NewlyAlert() const
{
  return data_.newly_alert;
}

INLINE unsigned char SMsg_LevelLogin::NewlyAwakened() const
{
  return data_.newly_awakened;
}

//INLINE unsigned char SMsg_LevelLogin::TCPOnly() const
//{  return data_.tcp_only;}

INLINE const TCHAR* SMsg_LevelLogin::Description() const
{
  return data_.description;
}

INLINE void SMsg_LevelLogin::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_LevelLogin::SetAccountType(int acct_type)
{
  data_.acct_type = acct_type;
}

INLINE void SMsg_LevelLogin::SetIPAddress(unsigned long ipaddr)
{
  data_.ipaddr = ipaddr;
}

INLINE void SMsg_LevelLogin::SetServerPort(int sport)
{
  data_.udp_portnum = sport;
}

INLINE void SMsg_LevelLogin::SetAvatar(const LmAvatar& avatar)
{
  data_.avatar = avatar;
}

INLINE void SMsg_LevelLogin::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void SMsg_LevelLogin::SetPeerUpdate(const LmPeerUpdate& update)
{
  data_.update = update;
}

INLINE void SMsg_LevelLogin::SetHidden(unsigned char hidden)
{
  data_.hidden = hidden;
}

INLINE void SMsg_LevelLogin::SetNewlyAlert(unsigned char alert)
{
  data_.newly_alert = alert;
}

INLINE void SMsg_LevelLogin::SetNewlyAwakened(unsigned char newly_awakened)
{
  data_.newly_awakened = newly_awakened;
}

//INLINE void SMsg_LevelLogin::SetTCPOnly(unsigned char tcp_only)
//{  data_.tcp_only = tcp_only; }
