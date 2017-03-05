// LmNetworkInput.h  -*- C++ -*-
// $Id: LmNetworkInput.h,v 1.14 1997-10-22 19:21:36-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// network input thread class

#ifndef INCLUDED_LmNetworkInput
#define INCLUDED_LmNetworkInput

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <list>

#include "LyraDefs.h"
#include "LmThread.h"

// class forward declarations

class LmSocket;
class LmConnection;
class LmConnectionSet;
class LmDispatch;
class LmLog;
class LmMessageReader;

// the main class

class LmNetworkInput : public LmThread {

public:

  enum {
    MAX_READERS = 64
  };

public:

  LmNetworkInput(LmDispatch* dispatch, LmConnectionSet* cset, LmMesgBufPool* bufpool, LmLog* log);
  virtual ~LmNetworkInput();

  void AddListener(LmSocket* sock);

  void Dump(FILE* f, int indent = 0) const;

protected:

  virtual void Run();

  virtual bool AllowConnection(LmSocket* sock);
  virtual bool HandleError(int errcode);

  virtual LmMessageReader* StartReader() = 0;

  void HandleNetworkInput();

  int AcceptConnection(LmSocket* sock);
  void ReadMessage(LmConnection* conn);
  void AddReader(LmMessageReader* thr);
  LmMessageReader* GetIdleReader() const;
  LmMessageReader* ChooseReader() const;

  enum {
    // error codes
    ERR_SELECT = 1
  };

  LmDispatch* Dispatch();
  LmConnectionSet* ConnectionSet();

private:

  // not implemented
  LmNetworkInput(const LmNetworkInput&);
  //operator=(const LmNetworkInput&);

  bool has_reader(LmConnection* conn) const;

  LmDispatch* dispatch_;
  LmConnectionSet* cset_;

  typedef std::list<LmSocket*> socklist_t;
  socklist_t listen_;  // list of listening sockets

  typedef std::list<LmMessageReader*> readerlist_t;
  readerlist_t readers_; // list of message reader threads
};

#ifdef USE_INLINE
#include "LmNetworkInput.i"
#endif

#endif /* INCLUDED_LmNetworkInput */
