// LsNetworkOutput.h  -*- C++ -*-
// $Id: LsNetworkOutput.h,v 1.12 1997-10-14 21:39:58-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// level server network output thread

#ifndef INCLUDED_LsNetworkOutput
#define INCLUDED_LsNetworkOutput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmNetworkOutput.h"
#include "LmLogFile.h"

// class forward declarations

class LsMain;

// class declarations

class LsNetworkOutput : public LmNetworkOutput {

public:

  LsNetworkOutput(LsMain* lsmain);
  ~LsNetworkOutput();

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  LsNetworkOutput(const LsNetworkOutput&);
  //operator=(const LsNetworkOutput&);

  // message handlers
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_LS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void handle_SMsg_LS_Action_Ping();
  void handle_SMsg_LS_Action_Exit();

  void open_log();
  void close_log();
  void register_message_handlers();

  LsMain* main_;

  // LmLogFile logf_;

};

#ifdef USE_INLINE
#include "LsNetworkOutput.i"
#endif

#endif /* INCLUDED_LsNetworkOutput */
