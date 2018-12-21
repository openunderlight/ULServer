// SMsg_LevelLogin.h  -*- C++ -*-
// $Id: SMsg_LevelLogin.h,v 1.12 1998-04-17 16:32:00-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level login message

#ifndef INCLUDED_SMsg_LevelLogin
#define INCLUDED_SMsg_LevelLogin

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"
#include "../../libsrc/shared/LmPeerUpdate.h"
#include "../../libsrc/shared/LmAvatar.h"

// forward references

// message class

class SMsg_LevelLogin : public LmMesg {

public:

  SMsg_LevelLogin();
  ~SMsg_LevelLogin();

  void Init(lyra_id_t playerid, const TCHAR* playername, int acct_type, unsigned long ipaddr,
	    int serverport, const LmAvatar& avatar, lyra_id_t roomid, const LmPeerUpdate& update,
	    unsigned char hidden, const TCHAR* description, unsigned char newly_alert, 
		unsigned char newly_awakened, unsigned char unused);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  const TCHAR* PlayerName() const;
  int AccountType() const;
  unsigned long IPAddress() const;
  int ServerPort() const;
  const LmAvatar& Avatar() const;
  lyra_id_t RoomID() const;
  const LmPeerUpdate& PeerUpdate() const;
  unsigned char Hidden() const;
  const TCHAR* Description() const;
  unsigned char NewlyAlert() const;
  unsigned char NewlyAwakened() const;
//  unsigned char TCPOnly() const;

  // mutators
  void SetPlayerName(const TCHAR* playername);
  void SetPlayerID(lyra_id_t playerid);
  void SetAccountType(int acct_type);
  void SetIPAddress(unsigned long ipaddr);
  void SetServerPort(int sport);
  void SetAvatar(const LmAvatar& avatar);
  void SetRoomID(lyra_id_t roomid);
  void SetPeerUpdate(const LmPeerUpdate& update);
  void SetHidden(unsigned char hidden);
  void SetDescription(const TCHAR* description);
  void SetNewlyAlert(unsigned char alert);
  void SetNewlyAwakened(unsigned char newly_awakened);
//  void SetTCPOnly(unsigned char tcp_only);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual message size
  void calc_size();

  // message data structure
  struct data_t {
    lyra_id_t playerid;                     // player id
    TCHAR playername[Lyra::PLAYERNAME_MAX];  // player name
    int acct_type;                          // player account type
    unsigned long ipaddr;                   // player's IP address (client!)
    int udp_portnum;                        // UDP port number for S->C local group updates
    LmAvatar avatar;                        // avatar type
    lyra_id_t roomid;                       // what room to enter
    LmPeerUpdate update;                    // where they are initially, other info
    unsigned char hidden;                           // are they hidden?
	unsigned char newly_alert;				// receive alerts when newly awakened enter the level
	unsigned char newly_awakened;			// are they newly awakened?
	unsigned char unused;				    // 
    TCHAR description[Lyra::MAX_AVATARDESC]; // avatar description
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_LevelLogin.i"
#endif

#endif /* INCLUDED_SMsg_LevelLogin */
