// LmConnection.h  -*- C++ -*-
// $Id: LmConnection.h,v 1.17 1998-02-03 23:33:55-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// connection class

#ifndef INCLUDED_LmConnection
#define INCLUDED_LmConnection

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <time.h>
#include "../../libsrc/shared/LyraDefs.h"
#include "LmSocket.h"
#include "LmUtil.h"
#include "../../libsrc/pth/PThMutex.h"

// forward declarations

class LmSrvMesgBuf;
class LmMesgHdr;

// class declarations

class LmConnection {

public:

  enum {
    // connection type (ie. what's at the other end)
    CT_UNKNOWN = 'U',  // unknown connection

    CT_CLIENT  = 'C',  // game client
	CT_DSRV    = 'D', // database proxy server
    CT_GSRV    = 'G',  // game server
    //    CT_GASP    = 'g',  // gasp (frontend to game server)
    CT_LSRV    = 'L',  // level server
    CT_MONITOR = 'M',   // server monitor,
  };

// CODE REMOVED: LjH [18/Jun/2004] - DEAD_CONNECTION no longer in use

  enum { // special ID's for connections
	DEAD_CONNECTION = 1	// dead connection (waiting to be reaped)
	};	  

public:

  LmConnection();
  ~LmConnection();

  void Init(LmSocket& sock, lyra_id_t id = Lyra::ID_UNKNOWN, int contype = CT_UNKNOWN);
  void Disconnect();
//  void Disable();
  //void CloseSocket();

  int SendMessage(LmSrvMesgBuf& msgbuf);
  int ReceiveMessage(LmSrvMesgBuf& msgbuf);
  int ReceiveMessage(LmSrvMesgBuf& msgbuf, int min_mtype, int max_mtype);
  int PeekAtHeader(LmMesgHdr& msghdr);

  bool IsConnected() const;
  //bool IsClosed() const;

  time_t Online() const;
  time_t IdleIn() const;
  time_t IdleOut() const;

  time_t ConnectTime() const;
  time_t LastIn() const;
  time_t LastOut() const;
  int MessagesIn() const;
  int MessagesInRate() const;
  int MessagesOut() const;
  int MessagesOutRate() const;
  int BytesIn() const;
  int BytesInRate() const;
  int BytesOut() const;
  int BytesOutRate() const;

  int MinMessageType() const;
  int MaxMessageType() const;
  time_t ChallengeTime() const;
  TCHAR* Challenge() const;

  void SetMessageRange(int min_mtype, int max_mtype);
  void SetID(lyra_id_t id);
  void SetType(int contype);
  void SetChallengeTime();
  void SetChallenge(TCHAR* value);

  void StartMessageReading();
  void EndMessageReading();
  bool MessageBeingRead() const;

  LmSocket& Socket();
  lyra_id_t ID() const;
  int Type() const;

  void Dump(FILE* f, int indent = 0) const;
  void Dump1(FILE* f) const;

private:

  // not implemented
  LmConnection(const LmConnection&);
  //operator=(const LmConnection&);

  void init_stats();

  LmSocket sock_;         // socket connection
  lyra_id_t id_;          // id of other end
  int type_;              // connection type

  TCHAR challenge_[Lyra::CHALLENGE_LENGTH]; // stores challenge sent on login

  time_t time_connect_;   // time of connection (approximately)
  time_t time_in_;        // time of last incoming message
  time_t time_out_;       // time of last outgoing message

  int msg_min_;           // minimum message type allowed
  int msg_max_;           // maximum message type allowed

  int msgs_in_;           // number of messages read
  int msgs_out_;          // number of messages sent
  int bytes_in_;          // number of bytes read
  int bytes_out_;         // number of bytes sent

  bool is_connected_;     // connection currently in use?
  bool is_closed_;		  // is socket closed? diagnostic use
  time_t challenge_time_;   // have sent challenge, waiting on response; this is the time it was issued

  bool msg_read_;         // message currently being read?
  PThMutex mr_lock_;  // lock for above

};

#ifdef USE_INLINE
#include "LmConnection.i"
#endif

#endif /* INCLUDED_LmConnection */
