// GsMacros.h  -*- C++ -*-
// $Id: GsMacros.h,v 1.11 1998-02-12 15:33:31-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message-handling convenience macros

#ifndef INCLUDED_GsMacros
#define INCLUDED_GsMacros

////
// arg-checking macros (yes, these are gross, maybe they'll be made functions later,
//   but as macros they do make the message handler functions much cleaner, and
//   they can be easily shared between the various thread types if all message handlers
//   follow the naming conventions)
////

// check that connection is null
#define CHECK_CONN_NULL() \
  { \
    if (conn) { \
      TLOG_Error(_T("%s: internal message received from conn [%p] (%c,%u)!"), method, conn, conn->Type(), conn->ID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("illegal message")); \
      return; \
    } \
  }

// check that connection is non-null
#define CHECK_CONN_NONNULL() \
  { \
    if (!conn) { \
      TLOG_Error(_T("%s: null connection"), method); \
      return; \
    } \
  }

// check that connection type is unknown
#define CHECK_CONN_ISUNKNOWN() \
  { \
    if (conn->Type() != LmConnection::CT_UNKNOWN) { \
      TLOG_Error(_T("%s: conn [%p] (%c,%u) not unknown"), method, conn, conn->Type(), conn->ID()); \
      GsUtil::Send_Error(main_, conn, msg_type, "already logged in"); \
      return; \
    } \
  }

// check that connection is a client
#define CHECK_CONN_ISCLIENT() \
  { \
    if (conn->Type() != LmConnection::CT_CLIENT) { \
      TLOG_Error(_T("%s: conn [%p] (%c,%u) not client"), method, conn, conn->Type(), conn->ID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("never logged in")); \
      return; \
    } \
  }

// check that connection is a level server
#define CHECK_CONN_ISLEVEL() \
  { \
    if (conn->Type() != LmConnection::CT_LSRV) { \
      TLOG_Error(_T("%s: conn [%p] (%c,%u) not level server"), method, conn, conn->Type(), conn->ID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("incorrect connection type")); \
      return; \
    } \
  }


// check that connection is a database proxy server
#define CHECK_CONN_ISDBPROXY() \
  { \
    if (conn->Type() != LmConnection::CT_DSRV) { \
      TLOG_Error(_T("%s: conn [%p] (%c,%u) not a database proxy server"), method, conn, conn->Type(), conn->ID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("incorrect connection type")); \
      return; \
    } \
  }

// check that the player object is non-null
#define CHECK_PLAYER_NONNULL() \
  { \
    if (!player_) { \
      main_->Log()->Error(_T("GsPlayerThread::%s: player is null"), method); \
      return; \
    } \
  }

// check that the player object is null
#define CHECK_PLAYER_NULL() \
  { \
    if (player_) { \
      main_->Log()->Error(_T("GsPlayerThread::%s: player is not null"), method); \
      return; \
    } \
  }

// check that connection's id is same as player's id
#define CHECK_CONN_ID() \
  { \
    if (conn->ID() != player_->PlayerID()) { \
      TLOG_Error(_T("%s: message from client id %u, not player %u"), method, conn->ID(), player_->PlayerID()); \
      GsUtil::Send_Error(main_, conn, msg_type, _T("id mismatch")); \
      return; \
    } \
  }

// check that player is in a level, and that the level connection is non-null
#define CHECK_PLAYER_INLEVEL() \
  { \
    if (!player_->InLevel()) { \
      TLOG_Error(_T("%s: player %u not in a level"), method, player_->PlayerID()); \
      return; \
    } \
    if (!player_->LevelConnection()) { \
      TLOG_Error(_T("%s: player is in level, but level connection is null!"), method); \
      return; \
    } \
    if (!player_->LevelDBC()) { \
      TLOG_Error(_T("%s: player is in level, but level database is null!"), method); \
      return; \
    } \
  }

// read message buffer into message (optionally sending error message)
#define ACCEPT_MSG(T, se) \
  T msg; \
  if (msg.Read(*msgbuf) < 0) { \
    TLOG_Error(_T("%s: could not read message, type %d, size %d"), method, msg_type, msg_size); \
    if (conn && se) { \
      GsUtil::Send_Error(main_, conn, msg_type, _T("corrupt ") #T _T(" message")); \
    } \
    return; \
  }
// read database message buffer into message (optionally sending error message)
#define ACCEPT_DB_MSG(T, accept_thread_type, se) \
  T msg(accept_thread_type); \
  if (msg.Read(*msgbuf) < 0) { \
    TLOG_Error(_T("%s: could not read message, type %d, size %d"), method, msg_type, msg_size); \
    if (conn && se) { \
      GsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("corrupt ") #T _T(" message")); \
    } \
    return; \
  }

// define message size/type, print debugging info
#define HANDLER_ENTRY(verbose) \
  int msg_type = msgbuf->Header().MessageType(); \
  int msg_size = msgbuf->Header().MessageSize(); \
//  if (!conn) { \
//    if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d) conn=null", method, msgbuf, msg_type, msg_size); \
//  } \
//  else { \
//    if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d) conn=[%p] (%c,%u)", method, msgbuf, msg_type, msg_size, conn, conn->Type(), conn->ID()); \
//  }

// define message size/type, print debugging info
#define PROXY_HANDLER_ENTRY(verbose) \
  int msg_type = msgbuf->Header().MessageType(); \
  int msg_size = msgbuf->Header().MessageSize(); \
//  if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d)", method, msgbuf, msg_type, msg_size);

// read message buffer into message
#define PROXY_ACCEPT_MSG(T) \
  T msg; \
  if (msg.Read(*msgbuf) < 0) { \
    TLOG_Error(_T("%s: could not read message, type %d, size %d"), method, msg_type, msg_size); \
    return; \
  }

#endif /* INCLUDED_GsMacros */
