// GsNetworkOutput.h  -*- C++ -*-
// $Id: GsNetworkOutput.h,v 1.14 1997-11-20 16:45:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// game server network output thread

#ifndef INCLUDED_GsNetworkOutput
#define INCLUDED_GsNetworkOutput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmNetworkOutput.h"
#include "LmLogFile.h"

// class forward declarations

class GsMain;

// class declarations

class GsNetworkOutput : public LmNetworkOutput {

public:

  GsNetworkOutput(GsMain* gsmain);
  ~GsNetworkOutput();

  void Dump(FILE* f, int indent = 0) const;

private:

  // not implemented
  GsNetworkOutput(const GsNetworkOutput&);
  //operator=(const GsNetworkOutput&);

  // message handlers
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  void handle_SMsg_GS_Action_Ping();

  void open_log();
  void close_log();
  void register_message_handlers();

  GsMain* main_;

  // LmLogFile logf_;

};

#ifdef USE_INLINE
#include "GsNetworkOutput.i"
#endif

#endif /* INCLUDED_GsNetworkOutput */
