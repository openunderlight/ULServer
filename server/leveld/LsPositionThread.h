// LsPositionThread.h  -*- C++ -*-
// $Id: LsPositionThread.h,v 1.15 1997-10-14 21:39:58-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// position update thread class

#ifndef INCLUDED_LsPositionThread
#define INCLUDED_LsPositionThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
//#include <sys/time.h>

#include "LyraDefs.h"
#include "LmThread.h"
#include "LmSocket.h"
#include "LmLogFile.h"

// class forward declarations

class LsMain;
class LmSockAddrInet;

// the class

class LsPositionThread : public LmThread {

public:

  LsPositionThread(LsMain* lsmain);
  ~LsPositionThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void HandleUDP();
  void Run();

private:

  // not implemented
  LsPositionThread(const LsPositionThread&);
  //operator=(const LsPositionThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_LS_* message handlers
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // RMsg_* message handlers
  void handle_RMsg_Update(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr);

  void open_log();
  void close_log();
  void register_message_handlers();

  LsMain* main_;
  LmSocket* usock_;       // UDP socket to read from
  LmSrvMesgBuf* msgbuf_;  // message buffer to use for receiving

  // LmLogFile logf_;        // thread log
};

#ifdef USE_INLINE
#include "LsPositionThread.i"
#endif

#endif /* INCLUDED_LsPositionThread */
