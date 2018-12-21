// LsInputDispatch.cpp  -*- C++ -*-
// $Id: LsInputDispatch.cpp,v 1.28 1998-04-17 17:40:32-07 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved.
//
// implementation

#ifdef __GNUC__
#pragma implementation "LsInputDispatch.h"
#endif

#include <stdio.h>
#ifndef WIN32
#include <stdlib.h>
#include <unistd.h>
#endif

#include "LsInputDispatch.h"
#include "LyraDefs.h"
#include "LsMain.h"
#include "LmSrvMesgBuf.h"
#include "LmConnection.h"
#include "LsPlayer.h"
#include "LsPlayerSet.h"
#include "LmThreadPool.h"
#include "RMsg.h"
#include "SMsg.h"
#include "LyraMessage.h"

#ifndef USE_INLINE
#include "LsInputDispatch.i"
#endif

////
// Constructor
////

LsInputDispatch::LsInputDispatch(LsMain* lsmain)
  : LmDispatch(lsmain->BufferPool()),
    main_(lsmain)
{
  initialize_table();
}

////
// Destructor
////

LsInputDispatch::~LsInputDispatch()
{
  // empty
}

////
// ComputeTarget
////

LmThread* LsInputDispatch::ComputeTarget(LmSrvMesgBuf* mbuf, LmConnection* /* conn */)
{
  // check if message type is in dispatch table
  int target = GetTarget(mbuf->Header().MessageType());
  if (target == 0) {
    return 0;  // not found, no thread to handle message
  }
  // case on dispatch target
  switch (target) {
  case DT_LEVEL: {  // level thread
    return main_->ThreadPool()->GetThread(LsMain::THREAD_LEVELSERVER);
  }
  break;
  case DT_ROOM: {  // room thread
    return main_->ThreadPool()->GetThread(LsMain::THREAD_ROOMSERVER);
  }
  break;
  default:
    break;
  }
  // default, no dispatch type match found
  return 0;
}

////
// Dump
////

void LsInputDispatch::Dump(FILE* f, int indent) const
{
  INDENT(indent, f);
 _ftprintf(f, _T("<LsInputDispatch[%p,%d]: main=[%p]>\n"), this, sizeof(LsInputDispatch), main_);
  // base class
  LmDispatch::Dump(f, indent + 1);
}

////
// message dispatch tables
////

// messages that the level thread accepts
static const int level_messages[] = {
  SMsg::LEVELLOGIN,
  SMsg::LOGIN,
  SMsg::LOGOUT,
  SMsg::SERVERERROR,
  SMsg::PING,
  SMsg::GETSERVERSTATUS,
  SMsg::ROTATE_LOGS, 
  SMsg::DUMP_STATE,
  SMsg::RESETPORT,
  SMsg::UNIVERSEBROADCAST
};
static const int num_level_messages = sizeof(level_messages) / sizeof(int);

// messages that the room thread accepts
static const int room_messages[] = {
  SMsg::GETITEM,
  SMsg::PUTITEM,
  SMsg::PROXY,
  SMsg::DESTROYROOMITEM,
  SMsg::GETLEVELPLAYERS,
  SMsg::GIVEITEM,
  SMsg::TAKEITEMACK,
  SMsg::LOCATEAVATAR,
  SMsg::SHOWITEM,
  SMsg::SETAVATARDESCRIPTION,
};
static const int num_room_messages = sizeof(room_messages) / sizeof(int);
  
////
// initialize_table
////

void LsInputDispatch::initialize_table()
{
  AddMappings(level_messages, num_level_messages, DT_LEVEL);
  AddMappings(room_messages, num_room_messages, DT_ROOM);
}
