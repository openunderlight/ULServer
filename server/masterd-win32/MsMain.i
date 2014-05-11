// MsMain.i  -*- C++ -*-
// $Id: MsMain.i,v 1.6 1998-02-12 15:33:49-08 jason Exp $
// Copyright 1996-1997 Lyra LLC, All rights reserved. 
//
// optionally inlined methods/functions

INLINE const char* MsMain::HostName() const
{
  return hostname_;
}

INLINE const char* MsMain::HostAddrString() const
{
  return hostaddrstr_;
}

INLINE const char* MsMain::HostID() const
{
  return hostid_;
}

INLINE const LmGlobalDB* MsMain::GlobalDB() const
{
  return globaldb_;
}

INLINE const LmServerDBC* MsMain::ServerDB() const
{
  return serverdbc_;
}

INLINE MsServerInfo* MsMain::ServerInfo() const
{
  return sinfo_;
}

INLINE pid_t MsMain::ServerPid() const
{
  return pid_;
}

INLINE bool MsMain::Restart() const
{
  return restart_;
}


