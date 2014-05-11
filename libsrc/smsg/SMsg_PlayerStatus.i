// SMsg_PlayerStatus.i  -*- C++ -*-
// $Id: SMsg_PlayerStatus.i,v 1.2 1997-10-08 18:45:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods

INLINE lyra_id_t SMsg_PlayerStatus::PlayerID() const
{
  return data_.playerid;
}

INLINE const TCHAR* SMsg_PlayerStatus::PlayerName() const
{
  return data_.playername;
}

INLINE lyra_id_t SMsg_PlayerStatus::LevelID() const
{
  return data_.levelid;
}

INLINE lyra_id_t SMsg_PlayerStatus::RoomID() const
{
  return data_.roomid;
}

INLINE int SMsg_PlayerStatus::X() const
{
  return data_.x;
}

INLINE int SMsg_PlayerStatus::Y() const
{
  return data_.y;
}

INLINE int SMsg_PlayerStatus::AccountType() const
{
  return data_.account_type;
}

INLINE int SMsg_PlayerStatus::Online() const
{
  return data_.online;
}

INLINE unsigned long SMsg_PlayerStatus::IPAddress() const
{
  return data_.ip_addr;
}

INLINE int SMsg_PlayerStatus::Idle() const
{
  return data_.idle;
}

INLINE void SMsg_PlayerStatus::SetPlayerID(lyra_id_t playerid)
{
  data_.playerid = playerid;
}

INLINE void SMsg_PlayerStatus::SetLevelID(lyra_id_t levelid)
{
  data_.levelid = levelid;
}

INLINE void SMsg_PlayerStatus::SetRoomID(lyra_id_t roomid)
{
  data_.roomid = roomid;
}

INLINE void SMsg_PlayerStatus::SetPosition(int x, int y)
{
  data_.x = x;
  data_.y = y;
}

INLINE void SMsg_PlayerStatus::SetAccountType(int acct_type)
{
  data_.account_type = acct_type;
}

INLINE void SMsg_PlayerStatus::SetOnline(int online)
{
  data_.online = online;
}

INLINE void SMsg_PlayerStatus::SetIPAddress(unsigned long ip_addr)
{
  data_.ip_addr = ip_addr;
}

INLINE void SMsg_PlayerStatus::SetIdle(int idle)
{
  data_.idle = idle;
}
