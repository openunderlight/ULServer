// LmMessageReader.h  -*- C++ -*-
// $Id: LmMessageReader.h,v 1.17 1997-09-23 02:32:49-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message reader thread class

#ifndef INCLUDED_LmMessageReader
#define INCLUDED_LmMessageReader

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>

#include "LyraDefs.h"
#include "LmThread.h"
#include "PThMutex.h"

// class forward declarations

class LmDispatch;
class LmMesgBufPool;
class LmConnection;
class LmLog;

// the class

class LmMessageReader : public LmThread {

public:

  LmMessageReader(LmDispatch* dispatch, LmMesgBufPool* mpool, LmLog* log);
  virtual ~LmMessageReader();

  void SetCurrentConnection(LmConnection* conn);
  LmConnection* CurrentConnection() const;

  virtual void Dump(FILE* f, int indent = 0) const;

protected:

  virtual void Run();
  virtual bool HandleError(LmConnection* conn, int errcode, int mtype, int msize);

  enum {
    // error code constants
    ERR_DISPATCH   = 'D',  // couldn't dispatch message
    ERR_EOF        = 'E',  // EOF detected
    ERR_HEADERPEEK = 'H',  // couldn't peek at full header
    ERR_NODATA     = 'N',  // no data available (but not EOF)
    ERR_RECEIVE    = 'R',  // couldn't receive message
    ERR_MSGSIZE    = 'S'   // message size too large
  };

  LmDispatch* Dispatch() const;

private:

  // not implemented
  LmMessageReader(const LmMessageReader&);
  //operator=(const LmMessageReader&);

  // message handlers
  void handle_SMsg_RecvMesg(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  int uses_;
  int msgs_read_;
  int bytes_read_;

  LmDispatch* dispatch_;

  LmConnection* conn_;
  PThMutex conn_lock_;
};

#ifdef USE_INLINE
#include "LmMessageReader.i"
#endif

#endif /* INCLUDED_LmMessageReader */
