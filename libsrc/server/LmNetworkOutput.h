// LmNetworkOutput.h  -*- C++ -*-
// $Id: LmNetworkOutput.h,v 1.13 1997-08-29 18:04:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// network output thread class

#ifndef INCLUDED_LmNetworkOutput
#define INCLUDED_LmNetworkOutput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "LmThread.h"

// class forward declarations

class LmConnectionSet;
class LmLog;

// the class

class LmNetworkOutput : public LmThread {

public:

  LmNetworkOutput(LmConnectionSet* cset, LmMesgBufPool* bufpool, LmLog* log);
  ~LmNetworkOutput();

  void Dump(FILE* f, int indent = 0) const;

protected:

  virtual void Run();

  virtual void SendMessage(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  virtual void HandleError(LmConnection* conn, LmSrvMesgBuf* mbuf, int errcode);

  enum {
    // error codes
    ERR_SEND = 1
  };

private:

  // not implemented
  LmNetworkOutput(const LmNetworkOutput&);
  //operator=(const LmNetworkOutput&);

  // SMsg message handlers
  void handle_SMsg_SendMesg(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  LmConnectionSet* cset_;
};

#ifdef USE_INLINE
#include "LmNetworkOutput.i"
#endif

#endif /* INCLUDED_LmNetworkOutput */
