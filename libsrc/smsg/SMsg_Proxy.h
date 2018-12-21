// SMsg_Proxy.h  -*- C++ -*-
// $Id: SMsg_Proxy.h,v 1.11 1997-08-04 18:39:22-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// proxy message -- message includes another message, and fields on
// what should be done with it (process it as if it came from a given
// player, forward it to a given player, etc)

#ifndef INCLUDED_SMsg_Proxy
#define INCLUDED_SMsg_Proxy

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmMesg.h"
#include "SMsg.h"

// forward references

class LmMesgBuf;

// message class

class SMsg_Proxy : public LmMesg {

public:

  enum {
    // types of proxy behavior
    PROXY_UNKNOWN = 'U',  // unknown/invalid

    PROXY_FORWARD = 'F',  // forward message to given player
    PROXY_PROCESS = 'P'   // process message as if it were sent from given player
  };

public:

  SMsg_Proxy();
  ~SMsg_Proxy();

  void Init(lyra_id_t playerid, int proxy_type, LmMesg& msg);
  void Init(lyra_id_t playerid, int proxy_type, LmMesgBuf* msgbuf);

  void InitProcess(lyra_id_t playerid, LmMesg& msg);
  void InitProcess(lyra_id_t playerid, LmMesgBuf* msgbuf);
  void InitForward(lyra_id_t playerid, LmMesg& msg);
  void InitForward(lyra_id_t playerid, LmMesgBuf* msgbuf);

  void CopyToBuffer(LmMesgBuf& msgbuf);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  lyra_id_t PlayerID() const;
  int ProxyType() const;
  int EnclosedMessageType() const;
  int EnclosedMessageSize() const;
  const TCHAR* MessageBytes() const;

  // mutators
  void SetPlayerID(lyra_id_t playerid);
  void SetProxyType(int proxy_type);
  void SetEnclosedMessage(LmMesg& msg);
  void SetEnclosedMessage(LmMesgBuf* msgbuf);

private:

  // standard non-public methods
  void hton();
  void ntoh();

  // calculate actual size
  void calc_size();

  // message data structure definition
  struct data_t {
    lyra_id_t playerid;   // source player
    int ptype;            // proxy type
    int mtype;            // message type
    int msize;            // message size
    TCHAR msgbytes[8192];  // message data (should be large enough for any message)
  } data_;

  // don't allocate on stack, it's too large
  //data_t* pdata_;

};

#ifdef USE_INLINE
#include "SMsg_Proxy.i"
#endif

#endif /* INCLUDED_SMsg_Proxy */
