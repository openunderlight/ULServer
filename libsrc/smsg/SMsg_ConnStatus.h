// SMsg_ConnStatus.h  -*- C++ -*-
// $Id: SMsg_ConnStatus.h,v 1.2 1998-02-03 23:34:20-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// connection status data

#ifndef INCLUDED_SMsg_ConnStatus
#define INCLUDED_SMsg_ConnStatus

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>
#include <sys/types.h>

#include "../../libsrc/shared/LyraDefs.h"
#include "../../libsrc/shared/LmMesg.h"
#include "SMsg.h"

// forward references

class LmConnection;

// message class

class SMsg_ConnStatus : public LmMesg {

public:

  enum {
    MAX_CONNS = 200  // max # of connections in message
  };

public:

  SMsg_ConnStatus();
  ~SMsg_ConnStatus();

  void Init(int num_connections);

  void InitConnection(int conn_num, LmConnection* conn);

  // standard public methods
  void Dump(FILE* f, int indent = 0) const;

  // selectors
  int NumConnections() const;
  int ConnectionType(int conn_num) const;
  int ConnectionID(int conn_num) const;
  unsigned long RemoteIP(int conn_num) const;
  int RemotePort(int conn_num) const;
  unsigned long LocalIP(int conn_num) const;
  int LocalPort(int conn_num) const;
  int Online(int conn_num) const;
  int IdleIn(int conn_num) const;
  int IdleOut(int conn_num) const;
  int MessagesIn(int conn_num) const;
  int BytesIn(int conn_num) const;
  int MessagesOut(int conn_num) const;
  int BytesOut(int conn_num) const;
  int MinMessageType(int conn_num) const;
  int MaxMessageType(int conn_num) const;

  // mutators
  void SetNumConnections(int num_conns);
  void SetConnectionType(int conn_num, int conn_type);
  void SetConnectionID(int conn_num, int conn_id);
  void SetRemoteIP(int conn_num, unsigned long conn_ip);
  void SetRemotePort(int conn_num, int port);
  void SetLocalIP(int conn_num, unsigned long conn_ip);
  void SetLocalPort(int conn_num, int port);
  void SetOnline(int conn_num, int online);
  void SetIdleIn(int conn_num, int idle_in);
  void SetIdleOut(int conn_num, int idle_out);
  void SetMessagesIn(int conn_num, int msgs_in);
  void SetBytesIn(int conn_num, int bytes_in);
  void SetMessagesOut(int conn_num, int msgs_out);
  void SetBytesOut(int conn_num, int bytes_out);
  void SetMinMessageType(int conn_num, int min_msg);
  void SetMaxMessageType(int conn_num, int max_msg);

public: // logically private

  // connection info
  struct ci_t {
    int conn_type;
    int conn_id;
    unsigned long remote_ip;
    int remote_port;
    unsigned long local_ip;
    int local_port;
    int online;
    int idle_in;
    int idle_out;
    int msgs_in;
    int bytes_in;
    int msgs_out;
    int bytes_out;
    int min_msg;
    int max_msg;
  };

private:

  // standard non-public methods
  void hton();
  void ntoh();

  bool conn_num_ok(int conn_num) const;

  void calc_size();

  // message data structure
  struct data_t {
    int num_conns;
    ci_t conns[MAX_CONNS];
  } data_;

};

#ifdef USE_INLINE
#include "SMsg_ConnStatus.i"
#endif

#endif /* INCLUDED_SMsg_ConnStatus */
