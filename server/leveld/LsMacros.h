// LsMacros.h  -*- C++ -*-
// $Id: LsMacros.h,v 1.5 1998-02-12 15:33:40-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// message-handling convenience macros

#ifndef INCLUDED_LsMacros
#define INCLUDED_LsMacros

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
      TLOG_Error(_T("%s: internal msg received from conn [%p] (%c,%u)!"), method, conn, conn->Type(), conn->ID()); \
      LsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("illegal message")); \
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
      LsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("already logged in")); \
      return; \
    } \
  }

// check that connection is a game server
#define CHECK_CONN_ISGAME() \
  { \
    if (conn->Type() != LmConnection::CT_GSRV) { \
      TLOG_Error(_T("%s: conn [%p] (%c, %u) not game server"), method, conn, conn->Type(), conn->ID()); \
      LsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("incorrect connection type")); \
      return; \
    } \
  }

// check that connection's id is same as player's id
#define CHECK_CONN_ID() \
  { \
    if (conn->ID() != player_->PlayerID()) { \
      TLOG_Error(_T("%s: message from client id %u, not player %u"), method, conn->ID(), player_->PlayerID()); \
      LsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("id mismatch")); \
      return; \
    } \
  }

// read message buffer into message (optionally sending error message)
#define ACCEPT_MSG(T, se) \
  T msg; \
  if (msg.Read(*msgbuf) < 0) { \
    TLOG_Error(_T("%s: could not read message, type %d, size %d"), method, msg_type, msg_size); \
    if (conn && se) { \
      LsUtil::Send_SMsg_Error(main_, conn, msg_type, _T("corrupt ") #T _T(" message")); \
    } \
    return; \
  }

// define message size/type, print debugging info
#define HANDLER_ENTRY(verbose) \
  int msg_type = msgbuf->Header().MessageType(); \
  int msg_size = msgbuf->Header().MessageSize(); 
//  if (!conn) { \
//    if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d) conn=null", method, msgbuf, msg_type, msg_size); \
//  } \
//  else { \
//    if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d) conn=[%p] (%c,%u)", method, msgbuf, msg_type, msg_size, conn, conn->Type(), conn->ID()); \
//  }

// read message buffer into message (optionally sending error message)
#define ACCEPT_PLAYERMSG(T, se) \
  T msg; \
  if (msg.Read(*msgbuf) < 0) { \
    TLOG_Error(_T("%s: could not read message, type %d, size %d"), method, msg_type, msg_size); \
    if (source && se) { \
      LsUtil::Send_RMsg_Error(main_, source, msg_type, _T("corrupt ") #T _T(" message")); \
    } \
    return; \
  }

// define message size/type, print debugging info
#define PROXY_HANDLER_ENTRY(verbose) \
  int msg_type = msgbuf->Header().MessageType(); \
  int msg_size = msgbuf->Header().MessageSize(); \
  lyra_id_t source_id = source->PlayerID(); 
//  if (verbose) TLOG_Debug("%s: msgbuf=[%p] (t=%d, s=%d) source=[%p] (%u)", method, msgbuf, msg_type, msg_size, source, source_id);

#endif /* INCLUDED_LsMacros */
