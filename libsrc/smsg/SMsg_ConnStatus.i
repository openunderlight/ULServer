// SMsg_ConnStatus.i  -*- C++ -*-
// $Id: SMsg_ConnStatus.i,v 1.2 1998-02-03 23:34:20-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// conditionally inline methods/functions

INLINE bool SMsg_ConnStatus::conn_num_ok(int conn_num) const
{
  return INDEX_OK(conn_num, 0, MAX_CONNS);
}

INLINE int SMsg_ConnStatus::NumConnections() const
{
  return data_.num_conns;
}

INLINE int SMsg_ConnStatus::ConnectionType(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].conn_type : 'U';
}

INLINE int SMsg_ConnStatus::LocalPort(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].local_port : 0;
}

INLINE int SMsg_ConnStatus::RemotePort(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].remote_port : 0;
}

INLINE int SMsg_ConnStatus::ConnectionID(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].conn_id : 0;
}

INLINE unsigned long SMsg_ConnStatus::RemoteIP(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].remote_ip : 0;
}

INLINE unsigned long SMsg_ConnStatus::LocalIP(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].local_ip : 0;
}

INLINE int SMsg_ConnStatus::Online(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].online : 0;
}

INLINE int SMsg_ConnStatus::IdleIn(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].idle_in : 0;
}

INLINE int SMsg_ConnStatus::IdleOut(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].idle_out : 0;
}

INLINE int SMsg_ConnStatus::MessagesIn(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].msgs_in : 0;
}

INLINE int SMsg_ConnStatus::BytesIn(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].bytes_in : 0;
}

INLINE int SMsg_ConnStatus::MessagesOut(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].msgs_out : 0;
}

INLINE int SMsg_ConnStatus::BytesOut(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].bytes_out : 0;
}

INLINE int SMsg_ConnStatus::MinMessageType(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].min_msg : 0;
}

INLINE int SMsg_ConnStatus::MaxMessageType(int conn_num) const
{
  return conn_num_ok(conn_num) ? data_.conns[conn_num].max_msg : 0;
}

INLINE void SMsg_ConnStatus::SetConnectionType(int conn_num, int conn_type)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].conn_type = conn_type;
}

INLINE void SMsg_ConnStatus::SetConnectionID(int conn_num, int conn_id)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].conn_id = conn_id;
}

INLINE void SMsg_ConnStatus::SetLocalIP(int conn_num, unsigned long conn_ip)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].local_ip = conn_ip;
}

INLINE void SMsg_ConnStatus::SetRemoteIP(int conn_num, unsigned long conn_ip)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].remote_ip = conn_ip;
}

INLINE void SMsg_ConnStatus::SetOnline(int conn_num, int online)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].online = online;
}

INLINE void SMsg_ConnStatus::SetLocalPort(int conn_num, int port)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].local_port = port;
}

INLINE void SMsg_ConnStatus::SetRemotePort(int conn_num, int port)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].remote_port = port;
}

INLINE void SMsg_ConnStatus::SetIdleIn(int conn_num, int idle_in)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].idle_in = idle_in;
}

INLINE void SMsg_ConnStatus::SetIdleOut(int conn_num, int idle_out)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].idle_out = idle_out;
}

INLINE void SMsg_ConnStatus::SetMessagesIn(int conn_num, int msgs_in)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].msgs_in = msgs_in;
}

INLINE void SMsg_ConnStatus::SetBytesIn(int conn_num, int bytes_in)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].bytes_in = bytes_in;
}

INLINE void SMsg_ConnStatus::SetMessagesOut(int conn_num, int msgs_out)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].msgs_out = msgs_out;
}

INLINE void SMsg_ConnStatus::SetBytesOut(int conn_num, int bytes_out)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].bytes_out = bytes_out;
}

INLINE void SMsg_ConnStatus::SetMinMessageType(int conn_num, int min_msg)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].min_msg = min_msg;
}

INLINE void SMsg_ConnStatus::SetMaxMessageType(int conn_num, int max_msg)
{
  if (conn_num_ok(conn_num)) data_.conns[conn_num].max_msg = max_msg;
}
