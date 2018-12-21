// LsSignalThread.h  -*- C++ -*-
// $Id: LsSignalThread.h,v 1.21 1998-02-24 22:35:00-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server signal handling thread

#ifndef INCLUDED_LsSignalThread
#define INCLUDED_LsSignalThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include "../../libsrc/shared/LyraDefs.h"
#include "LmSignalThread.h"
#include "LmLogFile.h"

// class forward declarations

class LsMain;
class LmSrvMesgBuf;
class LmMesg;
class LmConnection;
class LsPlayer;

// class declarations

class LsSignalThread : public LmSignalThread {

public:

  enum {
};

public:

  LsSignalThread(LsMain* lsmain);
  ~LsSignalThread();

  int last_signal_;

  void Dump(FILE* f, int indent = 0) const;


protected:

  void Run();

private:

  // not implemented
  LsSignalThread(const LsSignalThread&);
  //operator=(const LsSignalThread&);

  // message handling methods
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // signal handling methods
  void handle_SIG_Default(int sig);
  void handle_SIGTERM(int sig);

  void register_message_handlers();
  void register_signal_handlers();

  LsMain* main_;

  LmSrvMesgBuf* msgbuf_;  // buffer used for sending player updates

};

#ifdef USE_INLINE
#include "LsSignalThread.i"
#endif

#endif /* INCLUDED_LsSignalThread */
