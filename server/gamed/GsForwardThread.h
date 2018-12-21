// GsForwardThread.h  -*- C++ -*-
// $Id: GsForwardThread.h,v 1.10 1997-11-20 16:45:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// thread that forwards proxied messages to player threads

#ifndef INCLUDED_GsForwardThread
#define INCLUDED_GsForwardThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmLogFile.h"
#include "LmThread.h"

// forward declarations

class GsMain;

// the class

class GsForwardThread : public LmThread {

public:

  GsForwardThread(GsMain* gsmain);
  ~GsForwardThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();

private:

  // not implemented
  GsForwardThread(const GsForwardThread&);
  //operator=(const GsForwardThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_* message handlers (forwarding)
  void handle_SMsg_FWD(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_Ping(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_GS_* message handlers
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void register_handlers();
  void open_log();
  void close_log();

  lyra_id_t message_playerid(LmSrvMesgBuf* msgbuf);
  void forward_to_player_thread(lyra_id_t playerid, LmSrvMesgBuf* msgbuf, LmConnection* conn);

  GsMain* main_;    // pointer to game server main object

  // LmLogFile logf_;  // thread log

};

#ifdef USE_INLINE
#include "GsForwardThread.i"
#endif

#endif /* INCLUDED_GsForwardThread */
