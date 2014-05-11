// GsSignalThread.h  -*- C++ -*-
// $Id: GsSignalThread.h,v 1.19 1998-03-11 15:30:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server signal handling thread

#ifndef INCLUDED_GsSignalThread
#define INCLUDED_GsSignalThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmSignalThread.h"
#include "LmLogFile.h"
#include "LmTimer.h"

// class forward declarations

class GsMain;
class LmSrvMesgBuf;
class LmMesg;
class LmConnection;

// class declarations

class GsSignalThread : public LmSignalThread {

public:

  enum {

  };

public:

  GsSignalThread(GsMain* gsmain);
  ~GsSignalThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void Run();

private:

  // not implemented
  GsSignalThread(const GsSignalThread&);
  //operator=(const GsSignalThread&);

  // message handling methods
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // signal handling methods
  void handle_SIG_Default(int sig);
  void handle_SIGTERM(int sig);

  void register_message_handlers();
  void register_signal_handlers();

  GsMain* main_;

  //  timeval timer_;

};

#ifdef USE_INLINE
#include "GsSignalThread.i"
#endif

#endif /* INCLUDED_GsSignalThread */
