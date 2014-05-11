// LsNetworkInput.h  -*- C++ -*-
// $Id: LsNetworkInput.h,v 1.20 1997-11-17 14:05:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server network input thread

#ifndef INCLUDED_LsNetworkInput
#define INCLUDED_LsNetworkInput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"
#include "LmNetworkInput.h"
#include "LmLogFile.h"
#include "PThMutex.h"

// class forward declarations

class LmConnection;
class LsMain;
class LsMessageReader;
class LsPlayer;

// class declarations

class LsNetworkInput : public LmNetworkInput {

public:

  enum {
    // timeouts in seconds
    CLIENT_TIMEOUT = 60,  // idle timeout for client connection
    PLAYER_TIMEOUT = 75   // idle timeout for a player
  };

public:

  LsNetworkInput(LsMain* lsmain);
  ~LsNetworkInput();

  void StartIdleReaders(int num_readers = 4);

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();
  LmMessageReader* StartReader();
  bool AllowConnection(LmSocket* sock);

private:

  // not implemented
  LsNetworkInput(const LsNetworkInput&);
  //operator=(const LsNetworkInput&);

  // message handlers
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void handle_SMsg_LS_Action_CheckIdlePlayers();
  void handle_SMsg_LS_Action_CheckIdleClients();

  void fake_player_logout(LsPlayer* player);
  void close_connection(LmConnection* conn);

  void open_log();
  void close_log();
  void register_message_handlers();

  LsMain* main_;
  // LmLogFile logf_;

};

#ifdef USE_INLINE
#include "LsNetworkInput.i"
#endif

#endif /* INCLUDED_LsNetworkInput */
