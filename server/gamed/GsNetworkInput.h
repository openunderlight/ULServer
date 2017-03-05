// GsNetworkInput.h  -*- C++ -*-
// $Id: GsNetworkInput.h,v 1.19 1997-11-20 16:45:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server network input thread

#ifndef INCLUDED_GsNetworkInput
#define INCLUDED_GsNetworkInput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmNetworkInput.h"
#include "LmLogFile.h"
#include "PThMutex.h"

// class forward declarations

class LmConnection;
class GsMain;
class GsMessageReader;

// class declarations

class GsNetworkInput : public LmNetworkInput {

public:

  enum {
    // timeouts in seconds
#ifdef DEBUG
    CLIENT_TIMEOUT = 60,     // idle timeout for client connection
    PLAYER_TIMEOUT = 60,    // idle timeout for a player
    AGENT_TIMEOUT = 1500,    // idle timeout for a player
    MSG_THREAD_TIMEOUT = 60  // lifetime timeout for a message reader thread
#else
    CLIENT_TIMEOUT = 1800,     // idle timeout for client connection
    PLAYER_TIMEOUT = 300,    // idle timeout for a player
    AGENT_TIMEOUT = 1500,    // idle timeout for a player
    MSG_THREAD_TIMEOUT = 1800  // lifetime timeout for a message reader thread
#endif
  };

public:

  GsNetworkInput(GsMain* gsmain);
  ~GsNetworkInput();

  void StartIdleReaders(int num_readers = 4);

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();
  LmMessageReader* StartReader();

private:

  // not implemented
  GsNetworkInput(const GsNetworkInput&);
  //operator=(const GsNetworkInput&);

  // message handlers
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void handle_SMsg_GS_Action_CheckIdlePlayers();
  void handle_SMsg_GS_Action_CheckIdleClients();
  void handle_SMsg_GS_Action_CheckMessageThreads();

  void open_log();
  void close_log();
  void register_message_handlers();

  GsMain* main_;

  // LmLogFile logf_;

};

#ifdef USE_INLINE
#include "GsNetworkInput.i"
#endif

#endif /* INCLUDED_GsNetworkInput */
