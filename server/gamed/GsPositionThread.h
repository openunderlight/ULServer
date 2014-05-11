// GsPositionThread.h  -*- C++ -*-
// $Id: GsPositionThread.h,v 1.15 1997-11-20 16:45:53-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// position update thread class

#ifndef INCLUDED_GsPositionThread
#define INCLUDED_GsPositionThread

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>

#include "LyraDefs.h"
#include "LmThread.h"
#include "LmSocket.h"
#include "LmLogFile.h"

// class forward declarations

class GsMain;
class LmSockAddrInet;
class LmPeerUpdate;

// the class

class GsPositionThread : public LmThread {

public:

  GsPositionThread(GsMain* gsmain);
  ~GsPositionThread();

  void Dump(FILE* f, int indent = 0) const;

protected:

  void HandleUDP();
  void Run();

private:

  // not implemented
  GsPositionThread(const GsPositionThread&);
  //operator=(const GsPositionThread&);

  // default message handler
  void handle_Default(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // SMsg_GS_* message handlers
  void handle_SMsg_GS_Action(LmSrvMesgBuf* msgbuf, LmConnection* conn);

  // RMsg_* message handlers (redirectors)
  void handle_RMsg_PlayerUpdate(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr);
//  void handle_RMsg_Update_TCP(LmSrvMesgBuf* msgbuf, LmConnection* conn);
  void handle_RMsg_Update_UDP(LmSrvMesgBuf* msgbuf, LmSockAddrInet& caddr);
  
  // SMsg_ send
  void send_SMsg_ResetPort(lyra_id_t playerid, int port, LmConnection* level_conn);

  void open_log();
  void close_log();
  void register_message_handlers();

  GsMain* main_;

  LmSocket* usock_;       // UDP socket to read messages from
  LmSrvMesgBuf* msgbuf_;  // buffer used to read in UDP messages

  // bandwidth tracking
  int ls_udp_in_bytes_;
  int ls_udp_in_msgs_;
  int ls_udp_out_bytes_;
  int ls_udp_out_msgs_;
  int cl_udp_in_bytes_;
  int cl_udp_in_msgs_;
  int cl_udp_out_bytes_;
  int cl_udp_out_msgs_;

  // LmLogFile logf_;         // log

};

#ifdef USE_INLINE
#include "GsPositionThread.i"
#endif

#endif /* INCLUDED_GsPositionThread */
