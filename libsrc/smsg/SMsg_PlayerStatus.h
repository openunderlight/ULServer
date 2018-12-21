// SMsg_PlayerStatus.h  -*- C++ -*-
// $Id: SMsg_PlayerStatus.h,v 1.2 1997-10-08 18:45:57-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// player status data

#ifndef INCLUDED_SMsg_PlayerStatus
#define INCLUDED_SMsg_PlayerStatus

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

// message class

class SMsg_PlayerStatus : public LmMesg {

public:

  SMsg_PlayerStatus();
  ~SMsg_PlayerStatus();

  void Init(lyra_id_t playerid, const TCHAR* name, lyra_id_t levelid, lyra_id_t roomid,
	    int x, int y, int acct_type, int online, unsigned long ip_addr, int idle);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors 
  lyra_id_t PlayerID() const;
  const TCHAR* PlayerName() const;
  lyra_id_t LevelID() const;
  lyra_id_t RoomID() const;
  int X() const;
  int Y() const;
  int AccountType() const;
  int Online() const;
  unsigned long IPAddress() const;
  int Idle() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetPlayerName(const TCHAR* playername);
  void SetLevelID(lyra_id_t levelid);
  void SetRoomID(lyra_id_t roomid);
  void SetPosition(int x, int y);
  void SetAccountType(int acct_type);
  void SetOnline(int online);
  void SetIPAddress(unsigned long ip_addr);
  void SetIdle(int idle);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // message data structure
  struct data_t {
    lyra_id_t playerid;
    TCHAR playername[Lyra::PLAYERNAME_MAX];
    lyra_id_t levelid;
    lyra_id_t roomid;
    short x;
    short y;
    int account_type;
    int online;
    unsigned long ip_addr;
    int idle;
    // other: avatar, xp (gs only), ???
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_PlayerStatus.i"
#endif

#endif /* INCLUDED_SMsg_PlayerStatus */
